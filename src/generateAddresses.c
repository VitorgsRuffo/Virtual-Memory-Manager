#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(){

    FILE* addresses = fopen("./addresses.txt", "w");
    if (addresses == NULL)
        exit(1);

    srand(time(0));

    for(int i = 0; i<1000; i++)
        fprintf(addresses, "%d\n", (rand() % 65535));
        //fprintf(addresses, "%d\n", i);

    return 0;
}