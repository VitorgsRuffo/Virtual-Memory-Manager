#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "virtualMemoryManager.h"


int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "error: correct arguments must be provided.\n");
        fprintf(stderr, "usage: vmm addresses_file backing_store_file\n");
        return 1;
    }

    FILE* addresses = fopen(argv[1], "r");
    if(addresses == NULL){
        fprintf(stderr, "error: it was not possible to open addresses file.\n");
        return 1;
    } 
    char address[12];

    VirtualMemoryManager vmm = newVirtualMemoryManager(argv[2]);
    if(vmm == NULL){
        fprintf(stderr, "error: it was not possible to create virtual memory manager object.\n");
        return 1;
    }  
    uint32_t logicalAddress;
    uint16_t physicalAddress;
    char byte;

    while(fgets(address, 12, addresses) != NULL){
        logicalAddress = (uint32_t) atoi(address);
        physicalAddress = translateAddress(vmm, logicalAddress);
        byte = readPhysicalMemory(vmm, physicalAddress);
        printf("logical address: %u; physical address: %u; byte value: %d\n", logicalAddress, physicalAddress, byte);

    }
    printStatistics(vmm);
    fclose(addresses);
    freeVirtualMemoryManager(vmm);
    return 0;
}