gcc generateAddresses.c -o gea
./gea
gcc generateBackingStore.c -o geb
./geb
make
./vmm addresses.txt backing_store.bin
make clean
rm gea geb
