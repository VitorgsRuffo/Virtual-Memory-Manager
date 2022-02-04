#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "virtualMemoryManager.h"

#define PHYSICAL_MEMORY_SIZE 65536
#define PAGE_FRAME_SIZE 256

#define GET_OFFSET(binaryNumber) (binaryNumber & 0xff)
#define GET_PAGE(binaryNumber) ((binaryNumber >> 8) & 0xff)
#define GET_PHYSICAL_ADDRESS(frame, offset) ((8 << frame) | offset)

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

VirtualMemoryManager newVirtualMemoryManager(char* backingStorePath){
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

uint16_t translateAddress(VirtualMemoryManager Vmm, uint32_t logicalAddress){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    /*
    translateAddress(log_address):  (Section 8.5)  
    - extract page number and offset from address().
    - getPageFrame(pg):
        - consult TLB: (FUTURE)
            -if hit:
                return frame num    
            -else:
                return consult page table():
                    if found:
                        return frame num    
                    else:
                        - handle page fault(pg) (demand paging - Section 9.2)
                            - reads page num from backing store.
                            - store it in a free page frame in physical mem. (FUTURE: keep track of free page frames and perform page replacement when memory is full - FIFO or LRU policy - Section 9.4)
                            - update TLB and page table() (FUTURE: perform replacement when TLB is full - FIFO or LRU policy)
                        - getPageFrame(pg)
    - phy_address = GET_PHYSICAL_ADDRESS(frame, offset)
    - totalAddressReferences++
    - return phy_address

    */
}

char readPhysicalMemory(VirtualMemoryManager Vmm, uint16_t physicalAddress){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    return vmm->physicalMemory[physicalAddress];
}   

void printStatistics(VirtualMemoryManager Vmm){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    double pageFaultRate = ((vmm->stats.pageFaultCount) / vmm->stats.totalAddressReferences) * 100;
    double tblHitRate = ((vmm->stats.tlbHitCount) / vmm->stats.totalAddressReferences) * 100;
    printf("Statistics:\nPage-fault rate: %.2lf percent | TLB hit rate: %.2lf percent\n", pageFaultRate, tblHitRate);
}

void freeVirtualMemoryManager(VirtualMemoryManager Vmm){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
}