#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define GET_OFFSET(binaryNumber) (binaryNumber & 0xff)
#define GET_PAGE(binaryNumber) ((binaryNumber >> 8) & 0xff)

int main(){
    uint32_t addresses[] = {1, 18, 256, 32768, 32769, 128, 65537, 33153, 73456};
    for(int i = 0 ; i<9; i++){

        printf("original value: 0x%x\n", addresses[i]);
        printf("offset: 0x%x, page: 0x%x\n\n", GET_OFFSET(addresses[i]), GET_PAGE(addresses[i]));
    } 

    /*
    
        phy_mem, backingStore, pageTable, TLB, addresses
        totalAddressReferences = 0, pageFaultCount = 0, tblHitCount = 0;
        
        read next address (addresses):
            - extract page number and offset from address().

            - frame = translate(pg):  (Section 8.5)       
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
                                - translate(pg)
            - phy_address = (8 << frame) | offset
            - printf("logical address: %d; physical address: %d; byte value: %d", address, phy_address, phy_mem[phy_address]);
            - totalAddressReferences++

        printStatistics():
            pageFaultRate = (pageFaultCount/totalAddressReferences)*100;
            tblHitRate = (tblHitCount/totalAddressReferences)*100;
            printf("Statistics:\nPage-fault rate: %.2lf\% | TLB hit rate: %.2lf\%\n", pageFaultRate, tblHitRate);
                
    */


    return 0;
}