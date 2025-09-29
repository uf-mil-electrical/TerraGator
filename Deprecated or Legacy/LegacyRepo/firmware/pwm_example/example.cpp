#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
  int bright;
  printf ("Raspberry Pi blink\n") ;

  wiringPiSetup();
  pinMode(1, PWM_OUTPUT) ;

  while(1)
  {
    	while(bright < 1024)
	{
    		pwmWrite(1,bright);	// Off
    		delay(1);
		bright++;
	}
	while(bright > 0)
	{
		pwmWrite(1,bright);
		delay(1);
		bright--;
	}
  }
  return 0 ;





}
