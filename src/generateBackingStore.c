#include <stdio.h>
#include <stdlib.h>

void GerarArquivo();
void GerarArquivoValoresSequenciais();
void ConsultarArquivo();

void main()
{
    //GerarArquivo(); 
    GerarArquivoValoresSequenciais();
	//ConsultarArquivo();
}

void GerarArquivo()
{   FILE * arq;

    
    if ((arq = fopen("backing_store.bin","wb")) == NULL){
       printf("Error! opening file");
       exit(1);
   }
    int i;
    unsigned char num;
    int numint;

    for (i=0;i<65536;i++)
    {   
        numint=rand();

        num=(char)(numint%256);

        fwrite(&num,sizeof(num),1,arq);
    }
    fclose(arq);
}


void GerarArquivoValoresSequenciais()
{   FILE * arq;

    
    if ((arq = fopen("backing_store.bin","wb")) == NULL){
       printf("Error! opening file");
       exit(1);
   }
    int i;
    unsigned char c;
    for (i=0;i<65536;i++){
        c = (unsigned char) (i%256);
        fwrite(&c,sizeof(c),1,arq);
    }
    fclose(arq);
}

void ConsultarArquivo()
{   FILE * arq;
    int i,j;
    unsigned char num;
    int numint;
    long offset=0;
    if ((arq = fopen("backing_store.bin","rb")) == NULL){
       printf("Error! opening file");
       // Program exits if the file pointer returns NULL.
       exit(1);
   }
   

    for (i=0;i<65536;i++)
    {
        fread(&num, sizeof(num), 1, arq); 
        printf(">>>Endereço: %d >>> conteúdo: %d\n",i,num);
       
    }

    fclose(arq);
    
}

