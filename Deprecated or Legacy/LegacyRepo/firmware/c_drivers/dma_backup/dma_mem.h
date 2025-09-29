#ifndef DMA_MEM_H
#define DMA_MEM_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define BUS_TO_PHYS(x)  ((x) & 0x3FFFFFFF)

#define MEM_FLAG_DIRECT (1 << 2)
#define MEM_FLAG_COHERENT (2 << 2)
#define MEM_FLAG_L1_NONALLOCATING (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT)

typedef struct DMAMemHandle
{
    void *virtual_addr; // Virtual base address of the page
    uint32_t bus_addr;  // Bus address of the page
    uint32_t mb_handle; // Used by mailbox property interface
    uint32_t size;
} DMAMemHandle;

DMAMemHandle *dma_malloc(unsigned int size);
void dma_free(DMAMemHandle *mem);


#endif // DMA_MEM_H
