#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "virtualMemoryManager.h"

#define PHYSICAL_MEMORY_SIZE 65536
#define PAGE_FRAME_SIZE 256
#define PAGE_TABLE_ENTRIES 256

#define GET_OFFSET(binaryNumber) (binaryNumber & 0xff)
#define GET_PAGE(binaryNumber) ((binaryNumber >> 8) & 0xff)
#define GET_PHYSICAL_ADDRESS(frame, offset) ((frame << 8 ) | offset)

typedef struct {
    int totalAddressReferences;
    int pageFaultCount, tlbHitCount;
    int pageFaultRate, tlbHitRate;
}Statistics;

typedef struct {
    int frame;
    char validInvalidChar;
}pageTableEntry;    

typedef struct {
    char physicalMemory[PHYSICAL_MEMORY_SIZE];
    int nextFreeFrameInPhysicalMemory;
    pageTableEntry pageTable[PAGE_TABLE_ENTRIES];
    //TLB tlb;
    FILE* backingStore;
    Statistics stats;
}virtualMemoryManager;  

VirtualMemoryManager newVirtualMemoryManager(char* backingStorePath){
    virtualMemoryManager* vmm = (virtualMemoryManager*) malloc(sizeof(virtualMemoryManager));
    if(vmm == NULL) return NULL;
    vmm->nextFreeFrameInPhysicalMemory = 0;
    vmm->backingStore = fopen(backingStorePath, "rb");
    if(vmm->backingStore == NULL){
        free(vmm);
        return NULL;   
    }
    vmm->stats.totalAddressReferences = 0;
    vmm->stats.pageFaultCount = 0;
    vmm->stats.tlbHitCount = 0;
    vmm->stats.pageFaultRate = -1;
    vmm->stats.tlbHitRate = -1;
    for(int i = 0; i<PAGE_TABLE_ENTRIES; i++)
        vmm->pageTable[i].validInvalidChar = 'i';
    //tlb
    return vmm;
}


void handlePageFault(virtualMemoryManager* vmm, int pageNumber){
    //reading page from backing store...and writing page to a free physical memory frame...
    fseek(vmm->backingStore, PAGE_FRAME_SIZE * pageNumber, SEEK_SET);
    int framePhysicalAddress = (vmm->nextFreeFrameInPhysicalMemory) * PAGE_FRAME_SIZE; //(FUTURE: keep track of free page frames and perform page replacement when memory is full - FIFO or LRU policy - Section 9.4)
    fread(&(vmm->physicalMemory[framePhysicalAddress]), PAGE_FRAME_SIZE, 1, vmm->backingStore);

    //update page table... (FUTURE: update TLB too and perform replacement when TLB is full - FIFO or LRU policy)
    vmm->pageTable[pageNumber].validInvalidChar = 'v';
    vmm->pageTable[pageNumber].frame = vmm->nextFreeFrameInPhysicalMemory;
    vmm->nextFreeFrameInPhysicalMemory++;
}   

int seekPageTable(virtualMemoryManager* vmm, int pageNumber){
    if (vmm->pageTable[pageNumber].validInvalidChar == 'i')
        return -1;
    else
        return vmm->pageTable[pageNumber].frame;
}

// int seekTLB(virtualMemoryManager* vmm, int pageNumber){

// }

//private function:
int seekFrameNumber(virtualMemoryManager* vmm, int pageNumber){
    int frameNumber;
    
    // frameNumber = seekTLB(vmm, pageNumber);
    // if(frameNumber >= 0){ //TLB hit
            //vmm->stats.tlbHitCount++;
    //      return frameNumber;

    // }else{ //TLB miss

        frameNumber = seekPageTable(vmm, pageNumber);
        if(frameNumber >= 0){ //found
            return frameNumber;
        
        }else{ //page fault  
            vmm->stats.pageFaultCount++;
            handlePageFault(vmm, pageNumber); //demand paging technique.
            return seekFrameNumber(vmm, pageNumber);
        }
    //}
    /*
    - getPageFrame(pg): (Section 8.5) 
        - consult TLB: (FUTURE) x
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
                        -return getPageFrame(pg)
    -

    */
}

uint16_t translateAddress(VirtualMemoryManager Vmm, uint32_t logicalAddress){
    
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    
    int pageNumber = GET_PAGE(logicalAddress);
    int offset = GET_OFFSET(logicalAddress);
    int frameNumber = seekFrameNumber(vmm, pageNumber);

    uint16_t physicalAddress = GET_PHYSICAL_ADDRESS(frameNumber, offset);
    vmm->stats.totalAddressReferences++;

    return physicalAddress;
}

char readPhysicalMemory(VirtualMemoryManager Vmm, uint16_t physicalAddress){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    return vmm->physicalMemory[physicalAddress];
}   

void printMemory(VirtualMemoryManager Vmm){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    printf("\nPhysical memory:\n");
    for(int i = 0 ; i<PHYSICAL_MEMORY_SIZE; i++){
        printf("(%d) %d\n", i, vmm->physicalMemory[i]);
    }
}

void printStatistics(VirtualMemoryManager Vmm){
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    double pageFaultRate = ((double) (vmm->stats.pageFaultCount) / vmm->stats.totalAddressReferences) * 100;
    double tlbHitRate = ((double) (vmm->stats.tlbHitCount) / vmm->stats.totalAddressReferences) * 100;
    printf("\nStatistics:\nPage-fault rate: %.2lf percent | TLB hit rate: %.2lf percent\n", pageFaultRate, tlbHitRate);
}

void freeVirtualMemoryManager(VirtualMemoryManager Vmm){
 //   virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
}