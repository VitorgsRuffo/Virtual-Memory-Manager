CC = g++

flags = # -fstack-protector-all -g -Wall -Werror

obj = main.o virtualMemoryManager.o


vmm: $(obj)
	$(CC) -o vmm $(obj)

main.o: main.cpp  
	$(CC) $(flags) -c main.cpp


virtualMemoryManager.o: virtualMemoryManager.cpp 
	$(CC) $(flags) -c virtualMemoryManager.cpp

clean: 
	rm $(obj) vmm