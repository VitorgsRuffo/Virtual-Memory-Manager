#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "virtualMemoryManager.h"

#define PHYSICAL_MEMORY_SIZE 65536
#define PAGE_FRAME_SIZE 256

#define GET_OFFSET(binaryNumber) (binaryNumber & 0xff)
#define GET_PAGE(binaryNumber) ((binaryNumber >> 8) & 0xff)


typedef struct {
    int totalAddressReferences;
    int pageFaultCount, tlbHitCount;
    int pageFaultRate, tlbHitRate;
}Statistics;

typedef struct {
    char physicalMemory[PHYSICAL_MEMORY_SIZE];
    PageTable pageTable;
    TLB tlb;
    FILE* backingStore;
    Statistics stats;
}virtualMemoryManager;  

VirtualMemoryManager newVirtualMemoryManager(char* backingStorePath, int physicalMemorySize){
    virtualMemoryManager* vmm = (virtualMemoryManager*) malloc(sizeof(virtualMemoryManager));
    if(vmm == NULL) return NULL;
    vmm->backingStore = fopen(backingStorePath, "rb");
    if(vmm->backingStore) return NULL;
    vmm->stats.totalAddressReferences = 0;
    vmm->stats.pageFaultCount = 0;
    vmm->stats.tlbHitCount = 0;
    vmm->stats.pageFaultRate = -1;
    vmm->stats.tlbHitRate = -1;
    //pageTable
    //tlb
    return vmm;
}



