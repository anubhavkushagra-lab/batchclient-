#!/usr/bin/env bash
# compile.sh - Compiles the batch_client

echo "Generating proto code..."
mkdir -p proto_gen
protoc --proto_path=. --cpp_out=proto_gen --grpc_out=proto_gen --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) kv.proto

echo "Compiling batch_client..."
g++ -O3 -std=c++17 -I./proto_gen \
    batch_client.cpp \
    proto_gen/kv.pb.cc \
    proto_gen/kv.grpc.pb.cc \
    $(pkg-config --libs --cflags grpc++ protobuf) \
    -lpthread \
    -o batch_client

if [ $? -eq 0 ]; then
    echo "Successfully compiled batch_client!"
    chmod +x batch_client
else
    echo "Compilation failed."
fi
