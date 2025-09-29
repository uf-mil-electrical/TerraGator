#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x122c3a7e, "_printk" },
	{ 0x3069fe29, "sysfs_remove_file_ns" },
	{ 0x1fedd7a9, "kobject_put" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xfe990052, "gpio_free" },
	{ 0xb43f9365, "ktime_get" },
	{ 0x5dc6c34f, "kernel_kobj" },
	{ 0x40731afb, "kobject_create_and_add" },
	{ 0xb17677ff, "sysfs_create_file_ns" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xb0d2a97, "gpio_to_desc" },
	{ 0xcee724e1, "gpiod_direction_input" },
	{ 0x2fb63741, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x67a35d9, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "346BB471256C3030FCAA19D");
