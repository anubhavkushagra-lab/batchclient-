#!/usr/bin/env bash
# run.sh - Wrapper for the batch_client

if [ $# -lt 3 ]; then
    echo "Usage: ./run.sh <THREADS> <SECONDS> <SERVER_IP> [PAYLOAD_BYTES] [BATCH_SIZE]"
    echo "Example: ./run.sh 400 60 192.168.0.109 64 20"
    exit 1
fi

chmod +x ./batch_client
./batch_client "$@"
