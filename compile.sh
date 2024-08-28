docker start tp1
docker exec -ti tp1 make clean -C/root
docker exec -ti tp1 make all -C/root
