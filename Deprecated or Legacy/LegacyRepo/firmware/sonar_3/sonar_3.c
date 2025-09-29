#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/ktime.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>

//gpio 17

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Quintin Lopez-Scarim");
MODULE_DESCRIPTION("A LKM to read echo from sonar");

unsigned int irq_number;

s64 time_first_trigger;
s64 time_second_trigger;
bool first_trigger = 1;

//definitions for sysfs
static struct kobject *kobj_ref;
static s64 time_diff = 0;

static ssize_t time_diff_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%lld\n", time_diff);
}

static struct kobj_attribute time_diff_attr = __ATTR(time_diff, 0444, time_diff_show, NULL);
//end of definitions for sysfs



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
	//for sysfs
	time_diff = time_second_trigger - time_first_trigger;  // Store the time difference
	//end of for sysfs
	printk("Interrupt was triggered and ISR was called Time between two: %lld\n",time_second_trigger-time_first_trigger);
	first_trigger = !first_trigger;
	}
	return IRQ_HANDLED;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	printk("Initiating gpio interrupt...\n");

	    // Create the sysfs directory /sys/kernel/<module_name>
    	kobj_ref = kobject_create_and_add("sonar_3_time", kernel_kobj);
    	if (!kobj_ref)
	{
        return -ENOMEM;
	}
	int ret = sysfs_create_file(kobj_ref, &time_diff_attr.attr);
	if (ret) 
	{
        printk(KERN_ERR "Failed to create sysfs file.\n");
        kobject_put(kobj_ref);  // Clean up in case of error
        return ret;
    	}


	//setup GPIO pin
	//cat /sys/kernel/debug/gpio tells you which number linux uses to reference.
	int gpio_stat = gpio_request(536, "rpi-gpio-24");
	if(gpio_stat == -EBUSY)
	{
	printk("ERROR: GPIO 24 is not free. free it or reboot\n");
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
	if(gpio_direction_input(536))
	{
	printk("ERROR: GPIO 24 cannot be set as input\n");
	gpio_free(529);
	return -1;
	}
	//done setting up pin 13
	//setting up interrupt now
	irq_number = gpio_to_irq(536);
	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"my_gpio_irq",NULL) !=0)
	{
	printk("Error occured when assigning interrupt");
	gpio_free(536);
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

	// Remove the sysfs entry
    	sysfs_remove_file(kobj_ref, &time_diff_attr.attr);
    	kobject_put(kobj_ref);
	//done removing sysfs

	free_irq(irq_number,NULL);
	gpio_free(536);
	printk("GPIO_IRQ: Done unloading\n");
}

module_init(my_init);
module_exit(my_exit);
