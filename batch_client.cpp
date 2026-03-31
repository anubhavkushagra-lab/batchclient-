/*
 * load_client_batch.cpp  —  High-Throughput Client-Side Batching Benchmark
 *
 * Usage: ./load_client_batch <THREADS> <SECONDS> [SERVER_IP] [PAYLOAD_BYTES] [BATCH_SIZE]
 */
#include "kv.grpc.pb.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <iomanip>
#include <vector>

// ── helpers
static std::string ReadFile(const std::string &path) {
  std::ifstream t(path);
  if (!t.is_open())
    return "";
  return std::string((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
}

// ── constants
static constexpr int FIRST_PORT = 50063;
static constexpr int LAST_PORT = 50070;
static constexpr int N_PORTS = LAST_PORT - FIRST_PORT + 1; // 8
static constexpr int CHANNELS_PER_PORT = 32;

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "Usage: ./load_client_batch <THREADS> <SECONDS> [SERVER_IP] [PAYLOAD_BYTES] [BATCH_SIZE]\n"
              << "  e.g. ./load_client_batch 400 30 192.168.0.84 64 2000\n";
    return 0;
  }

  const int threads = std::stoi(argv[1]);
  const int target_seconds = std::stoi(argv[2]);
  const std::string server_ip = (argc >= 4) ? argv[3] : "192.168.0.84";
  const int payload_bytes = (argc >= 5) ? std::stoi(argv[4]) : 64;
  const int batch_size = (argc >= 6) ? std::stoi(argv[5]) : 2000;

  // ── credentials ──
  std::string ca_cert = ReadFile("ca.crt");
  if (ca_cert.empty()) ca_cert = ReadFile("../certs/ca.crt");
  if (ca_cert.empty()) ca_cert = ReadFile("/home/devatha/Downloads/files/certs/ca.crt");
  if (ca_cert.empty()) ca_cert = ReadFile("/home/devatha/Downloads/files/ca.crt");

  grpc::SslCredentialsOptions ssl_opts;
  ssl_opts.pem_root_certs = ca_cert;
  auto channel_creds = ca_cert.empty() ? grpc::InsecureChannelCredentials() : grpc::SslCredentials(ssl_opts);

  grpc::ChannelArguments login_args;
  if (!ca_cert.empty()) login_args.SetSslTargetNameOverride("kv-server");

  auto login_ch = grpc::CreateCustomChannel("ipv4:" + server_ip + ":" + std::to_string(FIRST_PORT), channel_creds, login_args);
  auto login_stub = kv::KVService::NewStub(login_ch);

  std::string jwt;
  {
    grpc::ClientContext ctx;
    kv::LoginRequest req;
    kv::LoginResponse resp;
    req.set_api_key("initial-pass");
    req.set_client_id("batch_bench");
    auto st = login_stub->Login(&ctx, req, &resp);
    if (!st.ok() || resp.jwt_token().empty()) {
      std::cerr << "Login FAILED: " << st.error_message() << "\n";
      return 1;
    }
    jwt = resp.jwt_token();
    std::cout << "Login OK. Starting HIGH-THROUGHPUT batch benchmark (NO pre-generation)...\n"
              << "  Threads: " << threads << ", Time: " << target_seconds << "s, Payload: " << payload_bytes << "B, Batch: " << batch_size << "\n";
  }

  // ── Stub pool ──
  std::vector<std::unique_ptr<kv::KVService::Stub>> stubs;
  stubs.reserve(N_PORTS * CHANNELS_PER_PORT);
  for (int p = FIRST_PORT; p <= LAST_PORT; ++p) {
    for (int c = 0; c < CHANNELS_PER_PORT; ++c) {
      grpc::ChannelArguments args;
      args.SetInt(GRPC_ARG_USE_LOCAL_SUBCHANNEL_POOL, 1);
      if (!ca_cert.empty()) args.SetSslTargetNameOverride("kv-server");
      auto ch = grpc::CreateCustomChannel("ipv4:" + server_ip + ":" + std::to_string(p), channel_creds, args);
      stubs.push_back(kv::KVService::NewStub(std::move(ch)));
    }
  }

  const std::string value(payload_bytes, 'x');
  std::atomic<long> total_ok{0}, total_fail{0};
  auto start = std::chrono::steady_clock::now();
  std::vector<std::thread> pool;

  // ── Benchmark ──
  for (int t = 0; t < threads; ++t) {
    pool.emplace_back([&, t]() {
      long ok = 0, fail = 0, j = 0, k_pool = 0;
      unsigned int seed = t;
      const int my_shard = t % N_PORTS;
      const int stub_idx = my_shard * CHANNELS_PER_PORT + ((t / N_PORTS) % CHANNELS_PER_PORT);
      auto *stub = stubs[stub_idx].get();

      kv::BatchRequest batch_req;
      kv::BatchResponse batch_resp;
      for (int i = 0; i < batch_size; ++i) batch_req.add_requests();

      while (true) {
        if ((j & 31) == 0) {
          if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() >= target_seconds) break;
        }

        // On-the-fly shard-consistent key generation
        for (int i = 0; i < batch_size; ++i) {
            auto* req = batch_req.mutable_requests(i);
            req->set_type(kv::PUT);
            
            // Search for a key that hashes to my_shard
            char k_buf[32];
            while (true) {
                int len = std::snprintf(k_buf, sizeof(k_buf), "k_%ld_%d", k_pool++, t);
                std::string k_str(k_buf, len);
                if (std::hash<std::string>{}(k_str) % N_PORTS == (size_t)my_shard) {
                    req->set_key(std::move(k_str));
                    break;
                }
            }
            req->set_value(value);
        }

        grpc::ClientContext ctx;
        ctx.AddMetadata("authorization", jwt);
        batch_resp.Clear();
        auto st = stub->ExecuteBatch(&ctx, batch_req, &batch_resp);
        if (st.ok()) {
          for (int i = 0; i < batch_resp.responses_size(); ++i) {
            if (batch_resp.responses(i).success()) ++ok;
            else ++fail;
          }
        } else {
          fail += batch_size;
          if (t == 0 && fail <= batch_size * 2) {
              std::cerr << "[Thread 0 Error] " << st.error_message() << " (Code: " << st.error_code() << ")\n";
          }
        }
        ++j;
      }
      total_ok.fetch_add(ok, std::memory_order_relaxed);
      total_fail.fetch_add(fail, std::memory_order_relaxed);
    });
  }

  for (auto &th : pool) th.join();
  auto end = std::chrono::steady_clock::now();
  double elapsed = std::chrono::duration<double>(end - start).count();
  long ok_val = total_ok.load();

  std::cout << "\n--- BATCH Benchmark Results ---\n"
            << "  Threads:      " << threads << "\n"
            << "  Successful:   " << ok_val << "\n"
            << "  Failed:       " << total_fail.load() << "\n"
            << "  Elapsed:      " << std::fixed << std::setprecision(2) << elapsed << " s\n"
            << "  Throughput:   " << static_cast<long>(ok_val / elapsed) << " req/s\n"
            << "-------------------------------\n";
  return 0;
}