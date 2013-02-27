@ Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail>
.syntax unified

@ Note that these functions are ARM mode encoded!
.arm

#include "asm/processor.h"

@ Executes a context switch:
.global switch_context
.balign 4
switch_context:
	@ Store the current context:
	push {lr}
	bl store_context
	pop {lr}

	@ Run the IRQ handler, which takes care of setting up the pointer to the
	@ next application's register block and resetting the timer interrupt:
	blx handle_irq

	@ Restore a previously saved context and return from the interrupt:
	bl restore_context

	@ Return from the interrupt:
	subs pc, lr, #4

@ Stores the current process context. The exception return address is passed
@ on the stack.
.global store_context
.balign 4
store_context:
	push {r0, lr}

	ldr lr, =register_block
	ldr lr, [lr]
	pld [lr]

	@ Return if no register block is available:
	cmp lr, #0
	popeq {r0, pc}

	@ Store user-mode registers:
	stmia lr, {r0 - lr}^

	@ Store the SPSR:
	mrs r0, spsr
	str r0, [lr, #15 * 4]

	@ Store the exception return address:
	mrs r0, cpsr

	@ Check which mode the processor is currently in:
	bfc r0, #6, #26
	cmp r0, #PROCESSOR_MODE_SVC

	ldr r0, [sp, #8]
	addeq r0, r0, #4	@ If in SVC mode, add 4 to the return address.
	str r0, [lr, #16 * 4]

	pop {r0, pc}

@ Restores the registers for the current context. The exception return
@ address is passed through the lr register.
.global restore_context
.balign 4
restore_context:
	push {lr}

	ldr lr, =register_block
	ldr lr, [lr]

	@ If no register block is available, loop until the next interrupt:
	cmp lr, #0
	beq restore_context_loop

	@ Load the SPSR:
	ldr r0, [lr, #15 * 4]
	msr spsr, r0

	@ Load the user-mode registers:
	ldmia lr, {r0 - lr}^

	@ Load the return address:
	ldr lr, [lr, #16 * 4]

	pop {pc}

restore_context_loop:
1:
	cpsie if
	wfi
	b 1b

.section .bss

@ Pointer to a register block to store user mode registers in on context switches.
@ When restoring the registers, they are also read from the memory pointed to by
@ this variable.
.global register_block
.balign 4
register_block:
	.skip 4

