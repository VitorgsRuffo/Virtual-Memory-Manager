#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "virtualMemoryManager.h"


int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf("error: correct arguments must be provided.\n", stderr);
        fprintf("usage: vmm addresses_file backing_store_file\n", stderr);
        return 1;
    }

    FILE* addresses = fopen(argv[1], "r");
    if(addresses == NULL){
        fprintf("error: it was not possible to open addresses file.\n", stderr);
        return 1;
    } 
    char address[12];

    VirtualMemoryManager vmm = newVirtualMemoryManager();    
    uint32_t logicalAddress;
    uint16_t physicalAddress;
    char byte;

    while(fgets(address, 12, addresses) != NULL){
        logicalAddress = (uint32_t) atoi(address);
        physicalAddress = translateAddress(vmm, logicalAddress);
        byte = readPhysicalMemory(vmm, physicalAddress);
        printf("logical address: %u; physical address: %u; byte value: %d\n", logicalAddress, physicalAddress, byte);

    }
    calculateStatistics(vmm);
    printStatistics(vmm);
    freeVirtualMemoryManager(vmm);
    fclose(addresses);
    return 0;
}

/*
    
        phy_mem, backingStore, pageTable, TLB, addresses
        totalAddressReferences = 0, pageFaultCount = 0, tblHitCount = 0;
        
        log_address = read next address (addresses)
            - phy_address = translateAddress(log_address):  (Section 8.5)  
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
                - phy_address = (8 << frame) | offset
                - totalAddressReferences++
                - return phy_address
            - printf("logical address: %d; physical address: %d; byte value: %d", log_address, phy_address, phy_mem[phy_address]);

        printStatistics():
            pageFaultRate = (pageFaultCount/totalAddressReferences)*100;
            tblHitRate = (tblHitCount/totalAddressReferences)*100;
            printf("Statistics:\nPage-fault rate: %.2lf\% | TLB hit rate: %.2lf\%\n", pageFaultRate, tblHitRate);
                
    */