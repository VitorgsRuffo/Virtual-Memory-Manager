#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(){
    uint32_t addresses[] = {1, 18, 256, 32768, 32769, 128, 65537, 33153};
    for(int i = 0 ; i<7; i++){

        printf("original value: 0x%x\n", addresses[i]);
        printf("offset: 0x%x, page: 0x%x\n\n", addresses[i] & 0xff, (addresses[i] >> 8) & 0xff);
    } 
    
  

    return 0;
}