@ Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail>
.syntax unified
.thumb

#include "asm/processor.h"

@ Sets up interrupt handling by setting the VBAR register to the
@ address of the interrupt handler table.
.global interrupts_init
interrupts_init:
	ldr r0, =interrupt_handlers
	mcr p15, 0, r0, c12, c0, 0
	bx lr
.ltorg

.balign 32
interrupt_handlers:
	b.w start
	b.w int_undef
	b.w int_svc
	b.w int_prefetch_abort
	b.w int_data_abort
	movs.w pc, lr @ Unused interrupt
	b.w int_irq
	b.w int_fiq

@ Undefined instruction handler:
.balign 4
int_undef:
	movs pc, lr

@ Supervisor call handler:
.balign 4
int_svc:
	@ Reset the SVC stack on SVC entry; this is to accomodate the exit syscall, which
	@ does not return, and therefore does not cause all pushed values in this function
	@ to be popped off the stack again:
	ldr sp, =0xa0000000

	push {r0 - r3, ip, lr}

	@ Push the syscall arguments:
	push {r0 - r2}

	@ Test if we were called from thumb code:
	tst lr, #1

	ittee eq

	@ Get the syscall number from a Thumb instruction:
	ldrheq r0, [lr, #-2]
	andeq r0, #0xff

	@ Get the syscall number from an ARM instruction:
	ldrne r0, [lr, #-4]
	andne r0, #0x00ffffff

	@ Pop the syscall arguments:
	pop {r1 - r3}

	@ Call the syscall handler:
	bl handle_syscall

	pop {r0 - r3, ip, lr}
	movs pc, lr

@ Prefetch abort handler:
.balign 4
int_prefetch_abort:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	push {r0 - r3, ip, lr}
	bl prefetch_abort
	pop {r0 - r3, ip, lr}

	rfeia sp!

@ Data abort handler:
.balign 4
int_data_abort:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	push {r0 - r3, ip, lr}
	bl data_abort
	pop {r0 - r3, ip, lr}

	rfeia sp!

@ IRQ handler:
.balign 4
int_irq:
	@ Set up the IRQ stack; sp is used as a scratch register throughout the
	@ IRQ handler and store_context, so to actually use the stack, it must be
	@ reset:
	ldr sp, =irq_stack
	ldr sp, [sp]

	@ Store the user-mode contents of r0 so r0 can be used as a scratch register below:
	push {r0}

	@ Read the IRQ number to check if there should be a context switch:
	ldr r0, =intc_base
	ldr r0, [r0]
	ldr r0, [r0, #0x40]
	and r0, #0xff

	@ Check for the timer0 IRQ, which is used for context switching:
	cmp r0, #37
	itt eq

	@ Use r0 to convey the contents of the lr register to store_context; the
	@ user-mode contents of r0 is pushed onto the stack earlier in this function.
	moveq r0, lr

	@ Execute a context switch; note that this call does not return back here,
	@ it has its own exception return code:
	blxeq switch_context

	@ Restore r0 if not doing a context switch:
	pop {r0}

	@ Call the IRQ handler function:
	push {r0 - r3, ip, lr}
	bl handle_irq
	pop {r0 - r3, ip, lr}

	@ Return from the interrupt:
	subs pc, lr, #4

@ FIQ handler, not used:
.balign 4
int_fiq:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	rfeia sp!

.section .bss

@ IRQ stack:
.global irq_stack
.balign 4
irq_stack:
	.skip 4

@ Virtual base address of the interrupt controller:
.global intc_base
.balign 4
intc_base:
	.skip 4

