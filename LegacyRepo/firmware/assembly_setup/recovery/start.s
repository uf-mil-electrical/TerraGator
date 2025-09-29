//.equ GPIO_BASE, 	0xfe200000

//Directory for svc calls:/usr/src/linux-headers-6.6.20+rpt-common-rpi/arch/arm64/include/asm
//Half decent doc https://man7.org/linux/man-pages/man2/open.2.html
//open is 274?
.equ GPFSEL2,		0x08
.equ GPIO_21_OUTPUT, 	0x8
.equ GPIOSET0, 		0x1c
.equ TOSET,	 	0x200000//0x200000
.equ CHANNEL_SEVEN_CS,	0x700
.equ CHANNEL_SEVEN_AD,	0x704
.equ DMA_OFFSET,	0x7e007000//0x7e00_7000
.equ ACTIVATE_DMA,	0x01 //literally just 1
.data
gpiomem: .asciz "/dev/gpiomem"
gen_mem: .asciz "/dev/mem"

source_buffer:
	.word 0xFFFFFFFF

control_block:
	.word 0x00000000 //0 TI
	.word 0x00000000 //1 SOURCE AD This is dynamically allocated, let the compiler handle it, will assign to buffer address
	.word 0x7e20001c //2 DEST AD  This is a constant as it is legacy addressed - should be GPIOSET0
        .word 0x00000004 //3 TXFR_LEM Only ever want to transfer 32 bits
	.word 0x00000000 //4 N/A
        .word 0x00000000 //5 NEXTCONBK
	.word 0x00000000 //6
        .word 0x00000000 //7

.text

.global _start
_start:
	//grabbing GPIO memory
	//x0 is dirfd and ignored with absolute path
	ldr	x1, =gpiomem
	ldr	x2, =0x101002	/* O_RDWR | O_SYNC */
	//x3 is umode, and is set only when deciding to create or not
	mov	x8, #56		/* open */
	svc	#0
	mov	x4, x0		/* file descriptor */

	mov	x0, #0		/* kernel chooses address */
	mov	x1, #4096	/* map size */
	mov	x2, #3		/* PROT_READ | PROT_WRITE */
	mov	x3, #1		/* MAP_SHARED */
	//x4 already has file descriptor 
	mov	x5, #0		/* offset */
	mov	x8, #222	/* mmap2 */
	svc	#0


	mov 	x12, x0		//WILL KEEP GPIO MEMORY POINTER IN R12

	//grabbing DMA memory
	//-ldr     x0, =gen_mem
        //-ldr     x1, =0x101002   /* O_RDWR | O_SYNC */
        //-mov     x7, #5          /* open */
        //-svc     #0
        //-mov     x4, x0          /* file descriptor */

	//-mov     x0, #0          /* kernel chooses address */
        //-mov     x1, #4096       /* map size */
        //-mov     x2, #3          /* PROT_READ | PROT_WRITE */
        //-mov     x3, #1          /* MAP_SHARED */
        //-ldr     x5, =DMA_OFFSET          /* offset */
        //-mov     x7, #192        /* mmap2 */
        //-svc     #0

	//-mov	x13, x0 //WILL KEEP DMA MEMORY POINTER IN R13

	//setting up control block source ad and dest ad
        //-ldr x0, =source_buffer //load address of source buffer into r0
        //-ldr x1, =control_block //load address of control block into r1
        //-str x0, [x1,#4]        //store source buffer address to source ad in control_block
//        mov r0, r12          -------LEGACY ADDRESS NOT VIRTUAL ADDRESS NEEDED SO IGNORE THIS ----------
//	str r0, [r1,#8]        //store GPIO as destination address, may need to make this the legacy access

	ldr x8, =GPIO_21_OUTPUT
	str x8, [x12,#GPFSEL2]
	ldr w8, =TOSET
	add x10,x12,#GPIOSET0 
	str w8, [x12,#GPIOSET0]


//	ldr r8, =control_block
//        str r8,[r13,#CHANNEL_SEVEN_AD]
//        ldr r8, =ACTIVATE_DMA
//        str r8,[r13,#CHANNEL_SEVEN_CS]

	b loop
loop:
	b loop
