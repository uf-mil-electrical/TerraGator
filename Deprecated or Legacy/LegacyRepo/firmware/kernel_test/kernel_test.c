#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define FILENAME "/home/raspberry/Mars-Rover/firmware/kernel_test/kernel_test.txt"

struct task_struct *task;
s64 time_interval = 0;

static int __init my_module_init(void) {
    time_interval = 1000;

    struct file *file;
    char msg[100];
    ssize_t ret;
    loff_t pos = 0;

    file = filp_open(FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (IS_ERR(file)) {
        pr_err("Error opening file: %ld\n", PTR_ERR(file));
        return -1;
    }

    // Write to the file
    snprintf(msg, sizeof(msg), "Interrupt was triggered and ISR was called Time between two: %lld\n", time_interval);
    ret = kernel_write(file, msg, strlen(msg), &pos);
    if (ret < 0) {
        pr_err("Error writing to file: %ld\n", ret);
    }

    // Close the file
    filp_close(file, NULL);
	
    return 0;
}

static void __exit my_module_exit(void) {
    printk("GPIO_IRQ: unloading\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple example of writing to a user space file from a kernel module");
