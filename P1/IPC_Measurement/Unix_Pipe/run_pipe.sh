gcc pipe_ipc.c -o pipe_ipc
for pkt_size in 4 16 64 256 1024 4096 16384 65536 262144 524288
do
  echo "====== PACKET SIZE : $pkt_size ======"
  ./pipe_ipc $pkt_size
  #./pipe_ipc $pkt_size > ./output/pipe_${pkt_size}.out
  echo "====================================="
done
