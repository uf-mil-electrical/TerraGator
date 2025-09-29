#include <unistd.h>
#include <cstdint>  // Required for uint8_t#include <cstdint>  // Required for uint8_t
#include "dma_mem.h"


class dma_handler
{
private:
DMAMemHandle *dma_cbs;// = dma_malloc(sizeof(block_one)*num_control_blocks);
DMAMemHandle *source_info;// = dma_malloc(sizeof(all_on));
DMAMemHandle *source_info2;// = dma_malloc(sizeof(all_off));
uint32_t num_control_blocks;
//uint32_t bus_address_on;
uint32_t bus_address_off;
//uint32_t reserved[20] = {};

typedef struct {
    bool reserved_me;
    uint32_t pin;
    uint32_t start;
    uint32_t end;
} pin_container;


uint32_t block_one[8] =
{
0x03E00008, //0 TI - flags  0x03E00000
0x00000000, //1 SOURCE AD This is dynamically allocated, let the compiler handle it, will assign to buff>
0x7e20001c, //2 DEST AD  This is a constant as it is legacy addressed - should be GPIOSET0
0x00000004, //3 TXFR_LEM Only ever want to transfer 32 bits
0x00000000, //4 N/A
0x00000000, //5 NEXTCONBK
0x00000000, //6 reserved
0x00000000 //7 reserved
};

public:
uint32_t num_reserved = 0;
uint32_t iteration = 0;
pin_container reserved[20];
void init_data();
void turn_off();
dma_handler(uint8_t on_block, uint8_t num_blocks_f,uint32_t dma_num,uint32_t pin_number);
void print_all_cbs(uint32_t num_control_blocks, uint32_t* cb_start);
void pwm_dma(uint8_t on_block, uint8_t num_blocks_f, uint32_t dma_num,uint32_t pin_number);
void initiate_dma_transfer(uint32_t* dma_mem, uint32_t first_cb_address, uint32_t dma_num);
void reset_dma(uint32_t* dma_mem, uint32_t dma_num);
void modify_blocks(uint8_t on_block, uint8_t num_blocks_f,uint32_t pin_number);
void clear_block(uint8_t pin_numer);
uint32_t find_index();
~dma_handler();
};



/*template <typename T>
struct aligned_allocator
{
  using value_type = T;
  T* allocate(std::size_t num)
  {
    void* ptr = nullptr;
    if (posix_memalign(&ptr,256,num*sizeof(T)))
      throw std::bad_alloc();
    return reinterpret_cast<T*>(ptr);
  }
  void deallocate(T* p, std::size_t num)
  {
    free(p);
  }
};*/