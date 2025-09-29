#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
class motor_drivers{
public:
	int pwm_res_0;
	int pwm_res_1;
	motor_drivers();
	int set_PWM(int pin, int level);

};
#endif
