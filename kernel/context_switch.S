@ Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail>
.syntax unified

@ Note that these functions are coded in ARM mode! They must be called using
@ the bx or blx instructions. Also note that they are run in IRQ mode.
.arm

#include "asm/processor.h"

@ Executes a context switch:
.global switch_context
.balign 4
switch_context:
	@ Store the current context:
	b store_context

1:	@ Run the IRQ handler, which takes care of setting up the pointer to the
	@ next application's register block and resetting the timer interrupt:
	ldr sp, =irq_stack
	ldr sp, [sp]
	blx handle_irq

	@ Restores a previously saved context and returns from the IRQ interrupt:
	b restore_context

@ Stores the registers for the current context:
.global store_context
.balign 4
store_context:
	push {r1}

	@ Store the link register, temporarily stored in r0:
	ldr r1, =register_block
	ldr r1, [r1]
	str r0, [r1, #16 * 4]

	pop {r0 - r1}

	ldr sp, =register_block
	ldr sp, [sp]

	@ Store the user registers:
	stmia sp, {r0 - lr}^

	@ Store the stored program status register:
	add sp, #15 * 4
	mrs r0, spsr
	str r0, [sp]

	b 1b

@ Restores the registers for the current context:
.global restore_context
.balign 4
restore_context:
	ldr sp, =register_block
	ldr sp, [sp]

	@ Restore the user mode registers:
	ldmia sp, {r0 - lr}^

	@ Load the program status register:
	ldr lr, [sp, #15 * 4]
	msr spsr, lr

	@ Load the IRQ return address:
	ldr lr, [sp, #16 * 4]

	@ Return from the interrupt:
	subs pc, lr, #4

.section .bss

@ Pointer to a register block to store user mode registers in on context switches.
@ When restoring the registers, they are also read from the memory pointed to by
@ this variable.
.global register_block
.balign 4
register_block:
	.skip 4

