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



-----BEGIN CERTIFICATE-----
MIIFCTCCAvGgAwIBAgIUGFfSiDmdgbYTmfpuTXgmphc8dRMwDQYJKoZIhvcNAQEL
BQAwFDESMBAGA1UEAwwJTXlMb2NhbENBMB4XDTI2MDMyNDA2MjMxNFoXDTM2MDMy
MTA2MjMxNFowFDESMBAGA1UEAwwJTXlMb2NhbENBMIICIjANBgkqhkiG9w0BAQEF
AAOCAg8AMIICCgKCAgEA0MnDhAqfnGLt6YwjH848nm6f+rnezTwdkozE7XH9MpcK
Z2LByKSCbYSNyZZNqnNkJ32TdOwCiYxDJt5Nvc8IRfJ8riIrteSZOP1i4JWWUNMq
Tc0u54vsid7wML3imL1YI/c7J1pshG8Icl/9XlwychHRN2EineY5Q4Pd1OgAaBuW
NNNjM0Jj+MuWNXkBDk+krPp6lCulcZu2d/D7m6p87Xb79ui8wz74p60zt7ALKRup
t4HR3+sARObyG6Te1HW5rNuqckjfAuWODdY+12p2GvpuSzEDnIl+by/KbasMke47
lF/V30DOocRiiDJT1gNQPCOJ0hY7hh4uLRgY/pwOQNaKiIiXX52E1mID0A/hQPfz
UwcJmWMjt8s0IpeHUDJ0D+9tFwT6EL3yudRSPuIZGrFoBkUkmnEJxo6jXyKz2+zC
V/hEov4oed7TW1QCZcFegDitjrv7CpXZ7OxwkDLfta2U26DsIuboo09+KqWHCPLt
WrxH3BRmoRimbigGPLHWnF+T7jNm/M0nfDkn0HE4C4omt80vC4MYCZTWHayxSvEa
Xvpudi439GMRwQaAy3V8MFmvMxShhJyF9jUvmBd7hd/CNneLPcIPZoseziX5MPFS
xBiKg0pAVWIlstqNx4JCHwlKh0uootkbyj+VJ462ayqhAA3GBwq9GIzVHClarikC
AwEAAaNTMFEwHQYDVR0OBBYEFJtIA8p+GI/dRRdeGGQjnNa446GaMB8GA1UdIwQY
MBaAFJtIA8p+GI/dRRdeGGQjnNa446GaMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI
hvcNAQELBQADggIBAFiH2bY2mfire27ugqojQ2OkWDvFQEp9bUpLWrwA5wYi7smf
e6yqPZ9H96pG10NmyOVFuQeUUGPqwFNqm/+2t6a76OwK5+9/iRguusEbbgLmRPkT
EF0keoRAGvUmv8z8THmswl9oSN6Lv3eqkNNYzvYjiXNB3uxNaTl3ysKqqno/0E0a
tD2qSVJBlE9QEZLe3RVl4soe65koKVpvKazuLV8CnDarc3+b5UtmDmyFT9ALxm2R
W4wbZD9JbyvcZHq32/ga0T75Tq10QSaLKt78Pxq7dHvgFJMMtXPdXL+svnXcYMmJ
3524vJA4c/P1/JRvMPxO+rtr/4gRdc/qVjSA0TlXHf4UeGckMfP7VTiqPSBvYcXd
OZ4jFiZFIZCysiklA9+nSjp4Zmbr7iIdyPZkUC2pyOZ8cAoCvMCqNOE+O1QDVm6B
bm/0iDciWJgxgPEEOtMoS5MuDVdxTy8MN9Hz0ZvCi+zEWPVcWhh8t8xu7esOffPg
h8Cw3Lr+AwWi1RqddgoNr8cVnPYFcNwyG6V/hNtV7ane7GBdCzVgE+dv1dMjteHf
72sBdJXpqmW9U48LJlOeu0oPVPGJqQ1ATY11TpzrOxnEcsLO/rimWMXLi2DtWNMG
ciSTULuYlkNl4GSaAWUzfNCPWXC49rYYS+dfapbswzwwqhNRGM1Cx8AdRQ8Q
-----END CERTIFICATE-----

