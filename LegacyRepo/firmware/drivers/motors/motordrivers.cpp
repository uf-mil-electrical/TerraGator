#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "motordrivers.h"
#include <softPwm.h>
#include <iostream>
#include <ostream>

motor_drivers::motor_drivers()
{
	pwm_res_0 = 0;
	pwm_res_1 = 0;
}

int motor_drivers::set_PWM(int pin,int level)
{
	wiringPiSetup();
	if(pin == 26 || pin == 1) //if the pin is in PWM bank 0
	{
		if(pwm_res_0 == 0 || pwm_res_0 == pin) //if the bank is open or reserved for this pin already
		{
  			pinMode(pin, PWM_OUTPUT) ;
  			pwmWrite(pin,level);
  			pwm_res_0 = pin; //handles the case where nothing is reserved yet
		}
		else
		{
			softPwmCreate (pin,0,1024);
			std::cout << "\nPin " << pin << " must use software PWM, and thus may be slightly inaccurate\n";
			softPwmWrite(pin,level);
		}
	}
	else if(pin == 23 || pin == 24) //if the pin is in PWM bank 1
        {
                if(pwm_res_1 == 0 || pwm_res_1 == pin) //if the bank is open, or resereved for this pin already
                {
                        wiringPiSetup();
                        pinMode(pin, PWM_OUTPUT) ;
                        pwmWrite(pin,level);
                        pwm_res_1 = pin; //handles the case where nothing is reserved yet
                }
		else
                {
			softPwmCreate (pin,0,1024);
                        std::cout << "\nPin " << pin << " must use software PWM, and thus may be slightly inaccurate\n";
                        softPwmWrite(pin,level);
                }
        }
	else
        {
	    softPwmCreate (pin,0,1024);
            std::cout << "\nPin " << pin << " must use software PWM (not in PWM bank), and thus may be slightly inaccurate\n";
            softPwmWrite(pin,level);
        }
	return 0;
}

