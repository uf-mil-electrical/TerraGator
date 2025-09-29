#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>

#include "mailbox.h"
#include "dma_mem.h"
/*#define PAGE_SIZE 4096
#define BUS_TO_PHYS(x) ((x) & ~0xC0000000 | 0xFE000000)

#define MEM_FLAG_DIRECT (1 << 2)
#define MEM_FLAG_COHERENT (2 << 2)
#define MEM_FLAG_L1_NONALLOCATING (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT)

typedef struct DMAMemHandle
{
    void *virtual_addr; // Virutal base address of the page
    uint32_t bus_addr;  // Bus adress of the page, this is not a pointer because it does not point to valid virtual address
    uint32_t mb_handle; // Used by mailbox property interface
    uint32_t size;
} DMAMemHandle;
*/


int mailbox_fd = -1;
//DMAMemHandle *dma_cbs;

DMAMemHandle *dma_malloc(unsigned int size)
{
    if (mailbox_fd < 0)
    {
        mailbox_fd = mbox_open();
        assert(mailbox_fd >= 0);
    }

    // Make `size` a multiple of PAGE_SIZE
    size = ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

    DMAMemHandle *mem = (DMAMemHandle *)malloc(sizeof(DMAMemHandle));
    // Documentation: https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
    mem->mb_handle = mem_alloc(mailbox_fd, size, PAGE_SIZE, MEM_FLAG_L1_NONALLOCATING);
    mem->bus_addr = mem_lock(mailbox_fd, mem->mb_handle);
    mem->virtual_addr = mapmem(BUS_TO_PHYS(mem->bus_addr), size);
    mem->size = size;

    assert(mem->bus_addr != 0);

    fprintf(stderr, "MBox alloc: %d bytes, bus: %08X, virt: %08X\n", mem->size, mem->bus_addr, (uint64_t)mem->virtual_addr);

    return mem;
}

void dma_free(DMAMemHandle *mem)
{
    if (mem->virtual_addr == NULL)
        return;

    unmapmem(mem->virtual_addr, PAGE_SIZE);
    mem_unlock(mailbox_fd, mem->mb_handle);
    mem_free(mailbox_fd, mem->mb_handle);
    mem->virtual_addr = NULL;
}
