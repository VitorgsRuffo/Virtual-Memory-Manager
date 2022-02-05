#ifndef VMM_H
#define VMM_H

typedef void* VirtualMemoryManager;

VirtualMemoryManager newVirtualMemoryManager(char* backingStorePath);

uint16_t translateAddress(VirtualMemoryManager Vmm, uint32_t logicalAddress);

char readPhysicalMemory(VirtualMemoryManager Vmm, uint16_t physicalAddress);

void printMemory(VirtualMemoryManager Vmm);

void printStatistics(VirtualMemoryManager Vmm);

void freeVirtualMemoryManager(VirtualMemoryManager Vmm);

#endif