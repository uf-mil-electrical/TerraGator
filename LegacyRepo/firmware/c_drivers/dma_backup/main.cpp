#include "dma_drivers.h"


int main()
{
//uint32_t two_buffer[2];
//check_active_gpio(two_buffer);
//std::cout << *two_buffer;
//std::cout << "\n";
//std::cout << *(two_buffer + 1);
//output_active_gpio(100,200);


//pwm_dma(125,250,0); // pwm_dma(uint8_t on_block,uint8_t num_blocks_f,uint32_t dma_num)//rate should be a number between>


dma_handler dma_object = dma_handler(125,250,0,21);
//dma_handler dma_object2 = dma_handler(125,250,1,20);

//int x = 0;
//while(1)
//{
//if(x > 250)
//{
//x = 0;
//}
//dma_object.modify_blocks(x,250);
//x++;
//sleep(1);


return 0;
}
