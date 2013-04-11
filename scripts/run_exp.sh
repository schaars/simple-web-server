#!/bin/bash
# Run an experiment


NODES="node1
node2
etc."

SERVER="server_node"
SERVERPORT=7777
NBFILES=1000
DURATION=60
NBMACHINES=$1
EXP=$2

echo "Starting server on $SERVER:$SERVERPORT"
# Rust VERSION:
#ssh $SERVER "cd simple-web-server/rust; pkill ws; ./ws -p $SERVERPORT -s 1 -d ../www" &
# C VERSION:
ssh $SERVER "cd simple-web-server/c; pkill ws; sleep 2; ./ws -p $SERVERPORT -s 1 -d ../www" &
sleep 2
ssh $SERVER "cd simple-web-server/scripts; perl tasksetall.pl"
ssh $SERVER "sar -P ALL 1 1000" &> /tmp/server.sar.out &

for node in $NODES; do
   ssh -n $node "pkill run_injector"
done
rm /tmp/client.*

n=0
for node in $NODES; do
   echo Starting injector on $node
   ssh -n $node "cd simple-web-server/scripts; pkill run_injector; ./run_injector.sh $SERVER $SERVERPORT $NBFILES 1 $DURATION" 2>&1 | tee /tmp/client.${node}.out &
   n=$(($n+1))
   if [ $n -ge $NBMACHINES ]; then
      break
   fi
done

sleep $(($DURATION+10))
ssh -n $SERVER "pkill ws; pkill sar"

mkdir $2
mv /tmp/server.sar.out $2/
mv /tmp/client.* $2/
