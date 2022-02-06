#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "virtualMemoryManager.h"
#include <map>
#include <queue>
using namespace std;

#define PHYSICAL_MEMORY_SIZE 32768
#define N__OF_FRAMES 128
#define PAGE_FRAME_SIZE 256
#define PAGE_TABLE_ENTRIES 256
#define TLB_ENTRIES 16

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
    char validInvalidChar; //'v': valid; 'i': invalid.
}pageTableEntry;

typedef struct {
    int page;
    int free; //1: frame is free; 0: frame is allocated.
}frameTableEntry;

typedef struct {
    map<int,int> tlb;
    queue<int> fifo;
}TLB;

typedef struct {
    char physicalMemory[PHYSICAL_MEMORY_SIZE];
    int nextFreeFrameInPhysicalMemory;
    pageTableEntry pageTable[PAGE_TABLE_ENTRIES];
    frameTableEntry frameTable[N__OF_FRAMES];
    TLB Tlb;
    FILE* backingStore;
    Statistics stats;
}virtualMemoryManager;  


VirtualMemoryManager newVirtualMemoryManager(char* backingStorePath){
    virtualMemoryManager* vmm = new virtualMemoryManager();

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
    for(int i = 0; i<N__OF_FRAMES; i++)
        vmm->frameTable[i].free = 1;
    return vmm;
}

//private function
void updateTLB(virtualMemoryManager* vmm, int pageNumber, int frameNumber){
    int tlbSize = (int) vmm->Tlb.tlb.size();
    if (tlbSize >= TLB_ENTRIES){ //tlb is full, applying fifo entry replacement policy...
        int erased;
        do {
            int entryToBeRemovedPageNumber = vmm->Tlb.fifo.front();
            vmm->Tlb.fifo.pop();
            erased = vmm->Tlb.tlb.erase(entryToBeRemovedPageNumber);
        } while(erased == 0);
    }

    vmm->Tlb.tlb.insert({ pageNumber, frameNumber });
    vmm->Tlb.fifo.push(pageNumber);
}


//private function
void handlePageFault(virtualMemoryManager* vmm, int pageNumber){

    //Bringing the needed page to physical memory...

    //Physical memory is implemented as a CIRCULAR ARRAY. 
    //Frames are allocated sequentially using a "next free frame" index starting at 0.
    //When memory gets full (i.e., "next free frame" index == N_OF_FRAMES) next free frame is set to zero.
    //That way we implement a FIFO PAGE REPLACEMENT POLICY. 


    //1. Checking next frame availability...
    if(vmm->frameTable[vmm->nextFreeFrameInPhysicalMemory].free == 0){  //next frame is actually not free. 
        //As it is the oldest (FIFO policy) 
        //its entries are going to be removed from page table and TLB (if present) 
        //and it is going to be considered as a free frame.
        int framePage = vmm->frameTable[vmm->nextFreeFrameInPhysicalMemory].page;
        vmm->pageTable[framePage].validInvalidChar = 'i';
        vmm->Tlb.tlb.erase(framePage);
    }

    //2. Reading page from backing store...and writing it to the free physical memory frame...
    fseek(vmm->backingStore, PAGE_FRAME_SIZE * pageNumber, SEEK_SET);
    int framePhysicalAddress = (vmm->nextFreeFrameInPhysicalMemory) * PAGE_FRAME_SIZE; 
    fread(&(vmm->physicalMemory[framePhysicalAddress]), PAGE_FRAME_SIZE, 1, vmm->backingStore);

    //3. Update frame table...
    vmm->frameTable[vmm->nextFreeFrameInPhysicalMemory].free = 0;
    vmm->frameTable[vmm->nextFreeFrameInPhysicalMemory].page = pageNumber;

    //4. Update page table...  
    vmm->pageTable[pageNumber].validInvalidChar = 'v';
    vmm->pageTable[pageNumber].frame = vmm->nextFreeFrameInPhysicalMemory;

    //5. Update TLB (and perform entry replacement when it is full - FIFO policy)
    updateTLB(vmm, pageNumber, vmm->pageTable[pageNumber].frame);

    vmm->nextFreeFrameInPhysicalMemory = (vmm->nextFreeFrameInPhysicalMemory + 1) % N__OF_FRAMES; //getting next circular array index.
}   

//private function
int seekPageTable(virtualMemoryManager* vmm, int pageNumber){
    if (vmm->pageTable[pageNumber].validInvalidChar == 'i')
        return -1;
    else
        return vmm->pageTable[pageNumber].frame;
}

//private function
int seekTLB(virtualMemoryManager* vmm, int pageNumber){
    int frameNumber;
    try{
        frameNumber = vmm->Tlb.tlb.at(pageNumber);
        return frameNumber;
    }
    catch (...){
        return -1;
    }
}

//private function
int seekFrameNumber(virtualMemoryManager* vmm, int pageNumber){
    int frameNumber;
    
    frameNumber = seekTLB(vmm, pageNumber);
    if(frameNumber >= 0){ //TLB hit
        vmm->stats.tlbHitCount++;
        return frameNumber;

    }else{ //TLB miss

        frameNumber = seekPageTable(vmm, pageNumber);
        if(frameNumber >= 0){ //found
            updateTLB(vmm, pageNumber, frameNumber);   //update TLB (and perform entry replacement when it is full - FIFO policy)
            return frameNumber;
        
        }else{ //page fault  
            vmm->stats.pageFaultCount++;
            handlePageFault(vmm, pageNumber); //demand paging technique.
            return seekFrameNumber(vmm, pageNumber);
        }
    }
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

void printPhysicalMemory(VirtualMemoryManager Vmm){
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
    virtualMemoryManager* vmm = (virtualMemoryManager*) Vmm;
    fclose(vmm->backingStore);
    delete vmm;
}