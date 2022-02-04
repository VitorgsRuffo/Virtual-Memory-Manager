CC = gcc

flags = -std=c99 -fstack-protector-all -g -Wall -Werror

obj = main.o virtualMemoryManager.o


vmm: $(obj)
	$(CC) $(obj) -o vmm -lm

main.o: main.c  
	$(CC) $(flags) -c main.c


virtualMemoryManager.o: virtualMemoryManager.c 
	$(CC) $(flags) -c virtualMemoryManager.c

clean: 
	rm $(obj) vmm