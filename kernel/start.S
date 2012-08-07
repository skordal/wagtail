@ Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail>
.syntax unified
.thumb

#include "asm/processor.h"

@ Global entry point procedure:
.global start
start:
	@ This code is loaded in ARM mode:
	.arm

	@ Ensure we are in supervisor mode with interrupts disabled:
	cpsid aif, #PROCESSOR_MODE_SVC

	@ Set up a temporary stack until the MMU is enabled:
	ldr sp, =0x80008000

	@ Switch to thumb mode:
	blx thumb_start

.thumb
thumb_start: @ Thumb code entry point.

clear_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	eors r2, r2, r2
1:
	cmp r0, r1
	itt ne
	strne r2, [r0], #4
	bne 1b

initialize_interrupts: @ Initializes the interrupt handling code:
	bl interrupts_init

initialize_mmu: @ Initializes the MMU:
	bl mmu_init

enable_mmu: @ Enables the MMU:
	mrc p15, 0, r0, c1, c0, 0
	ldr r1, =SCTLR_I|SCTLR_Z|SCTLR_C|SCTLR_M|SCTLR_TE
	orrs r0, r1
	mcr p15, 0, r0, c1, c0, 0

	isb
	dsb

	@ Set the new stack pointer:
	ldr sp, =0xa0000000

initialize_mm: @ Initializes the kernel memory manager:
	bl mm_init

run_global_constructors:
	ldr r0, =constructors_start
	ldr r1, =constructors_end
1:
	cmp r0, r1
	beq.n 2f

	ldr r2, [r0]
	push {r0 - r1}
	blx r2
	pop {r0 - r1}
	adds r0, #4
	b.n 1b
2:

start_kernel: @ Enable interrupts and enter the C++ kernel:
	cpsie if
	bl kernel_main

run_global_destructors:
	ldr r0, =destructors_start
	ldr r1, =destructors_end
1:
	cmp r0, r1
	beq.n 2f

	ldr r2, [r0]
	push {r0 - r1}
	blx r2
	pop {r0 - r1}
	adds r0, #4
	b.n 1b
2:

loopy: @ TODO: halt or reboot here-abouts.
	cpsid aif
	wfi
	b.n loopy

