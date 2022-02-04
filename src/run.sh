gcc generateAddresses.c -o gea
./gea
gcc generateBackingStore.c -geb
./geb
make
./vmm addresses.txt backing_store.bin