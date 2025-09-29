#include "motordrivers.h"

int main(void)
{
	motor_drivers motor_instance = motor_drivers();
	//motor_instance.set_PWM(1,100);
	motor_instance.set_PWM(23,512);
	//motor_instance.set_PWM(26,700);
	motor_instance.set_PWM(40,150);
}
