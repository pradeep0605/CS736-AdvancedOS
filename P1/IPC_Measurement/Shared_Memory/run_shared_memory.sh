#for pkt_size in 4 16 64 256 1024 4096 16384 65536 262144 524288
for pkt_size in 4 16 64 256 1k 4k 16k 64k 256k 512k
do
  echo "====================== PACKET SIZE : $pkt_size  ================================"
  ./SharedMemory $pkt_size 
  echo "=========================================================================="
done
