#!/bin/bash
#
# Run a group of clients
# Args:
#   -server hostname
#   -server port
#   -nb server files
#   -nb clients to start
#   -bench duration in seconds


function run_one_client {
start=$(date +%s)
n=0
while [ ! -e /tmp/done ]; do
   curl -0 -s -o /dev/null http://${HOST}:${PORT}/file$((${RANDOM}%${NBFILES}));
   n=$(($n+1))
done
elapsed=$(($(date +%s)-$start))
echo "Client $1: thr=$(echo "$n / $elapsed" | bc)"
}


if [ $# -ne 5 ]; then
   echo "Usage: ./$(basename $0) server_hostname server_port nb_server_files nb_clients bench_duration"
   exit 0
fi

HOST=$1
PORT=$2
NBFILES=$3
NB_CLIENTS=$4
DURATION=$5

rm /tmp/done

c=0
while [ $c -ne $NB_CLIENTS ]; do
   run_one_client $c &
   c=$(($c+1))
done

sleep $DURATION
touch /tmp/done
