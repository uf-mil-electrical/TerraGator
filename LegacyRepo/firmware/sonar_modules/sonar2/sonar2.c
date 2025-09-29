#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/ktime.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/workqueue.h>


#define FILENAME "/home/raspberry/Mars-Rover/firmware/sonar2.txt"

struct task_struct *task;
s64 time_interval = 0;

//gpio 17

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Quintin Lopez-Scarim");
MODULE_DESCRIPTION("A LKM to read echo from sonar");

unsigned int irq_number;

s64 time_first_trigger;
s64 time_second_trigger;
bool first_trigger = 1;

static struct workqueue_struct *my_wq;
static struct work_struct my_work;

void log_time_interval(struct work_struct *work) {
    char msg[100];
    ssize_t ret;
    loff_t pos = 0;

    struct file* file = filp_open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(file)) {
        pr_err("Error opening file: %ld", PTR_ERR(file));
        return;
    }

    snprintf(msg, sizeof(msg), "%lld", time_interval);
    ret = kernel_write(file, msg, strlen(msg), &pos);
    if (ret < 0) {
        pr_err("Error writing to file: %ld\n", ret);
    }

    filp_close(file, NULL);
}

//ISR
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	if(first_trigger)
	{
	time_first_trigger = ktime_to_ns(ktime_get());
	first_trigger = !first_trigger;
	}
	else
	{
	time_second_trigger = ktime_to_ns(ktime_get());
    time_interval = ((time_second_trigger-time_first_trigger) / 1000)/ 58;
	printk("Distance: %lld\n",((time_second_trigger-time_first_trigger) / 1000)/ 58); //us/58 = cm

	queue_work(my_wq, &my_work);

    first_trigger = !first_trigger;
	}
	return IRQ_HANDLED;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	printk("Initiating gpio interrupt...\n");

	my_wq = create_singlethread_workqueue("my_workqueue");
    if (!my_wq) {
        printk("Failed to create workqueue\n");
        return -ENOMEM;
    }
    INIT_WORK(&my_work, log_time_interval);

    //setup GPIO pin
	//cat /sys/kernel/debug/gpio tells you which number linux uses to reference.
	int gpio_stat = gpio_request(539, "rpi-gpio-27");
	if(gpio_stat == -EBUSY)
	{
	printk("ERROR: GPIO 27 is not free. free it or reboot\n");
	return -1;
	}
	if(gpio_stat == -EINVAL)
	{
	printk("ERROR: INVALID GPIO\n");
	return -1;
	}
	if(gpio_stat)
	{
	printk("Flags for errors are wrong: %d\n",gpio_stat);
	return -1;
	}
	if(gpio_direction_input(539))
	{
	printk("ERROR: GPIO 13 cannot be set as input\n");
	gpio_free(539);
	return -1;
	}
	//done setting up pin 13
	//setting up interrupt now
	irq_number = gpio_to_irq(539);
	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"my_gpio_irq",NULL) !=0)
	{
	printk("Error occured when assigning interrupt");
	gpio_free(539);
	return -1;
	}
	
	printk("Done");
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	printk("GPIO_IRQ: unloading\n");
	flush_workqueue(my_wq);
    destroy_workqueue(my_wq);
	free_irq(irq_number,NULL);
	gpio_free(539);
}

module_init(my_init);
module_exit(my_exit);