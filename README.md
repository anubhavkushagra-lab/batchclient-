# Batching gRPC Load Client

This package contains a high-performance synchronous gRPC load client optimized for high throughput via the `ExecuteBatch` RPC.

## Package Contents
- `batch_client.cpp`: Source code for the batching client.
- `kv.proto`: gRPC service definition.
- `ca.crt`: SSL certificate (root CA).
- `compile.sh`: Script to generate proto code and compile the client.
- `run.sh`: Wrapper script to execute the benchmark.

## Compilation
To compile the client, ensure you have `protoc`, `grpc_cpp_plugin`, and gRPC development libraries installed, then run:
```bash
bash compile.sh
```

## Running the Benchmark
Use the `run.sh` script to start the load test:
```bash
./run.sh <THREADS> <SECONDS> <SERVER_IP> [PAYLOAD_BYTES] [BATCH_SIZE]
```
- **THREADS**: Number of concurrent benchmark threads.
- **SECONDS**: Benchmark duration.
- **SERVER_IP**: Target server IP.
- **PAYLOAD_BYTES**: (Optional) Size of the value payload (default: 64).
- **BATCH_SIZE**: (Optional) Number of requests per `ExecuteBatch` RPC (default: 10).

## Features
- **ExecuteBatch**: Satisfies server requirements for batched operations and increases throughput.
- **On-the-fly Keys**: Instant launch without pre-generation delay.
- **Mixed Requests**: Alternates between GET, PUT, and DELETE operations.
- **Sharded Routing**: Threads share 128 channels across 8 ports for maximum parallelism.
# batchclient-
