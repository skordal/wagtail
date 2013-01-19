@ Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail>
.syntax unified
.thumb

#include "asm/processor.h"
#include "asm/syscalls.h"

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

	@ Store the context, the fork syscall needs this:
	push {lr}
	blx store_context @ TODO: Do this only if neccessary.
	pop {lr}

	@ Push the syscall arguments:
	push {r0 - r2}

	@ Test if we were called from thumb code:
	tst lr, #1
	ite eq

	@ Get the syscall number from a Thumb instruction:
	ldrheq r0, [lr, #-2]

	@ Get the syscall number from an ARM instruction:
	ldrne r0, [lr, #-4]

	@ Only 0 - 255 are valid system call numbers:
	and r0, #0x000000ff

	@ Pop the syscall arguments:
	pop {r1 - r3}

	@ Call the syscall handler:
	push {ip, lr}
	bl handle_syscall
	pop {ip, lr}

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

	@ Store the contents of r0 so r0 can be used as a scratch register below:
	push {r0}

	@ Read the IRQ number to check if there should be a context switch:
	ldr r0, =intc_base
	ldr r0, [r0]
	ldr r0, [r0, #0x40]
	and r0, #0xff

	@ Check for the timer0 IRQ, which is used for context switching:
	cmp r0, #37
	pop {r0}

	@ If we are handling IRQ 37, execute a context switch:
	it eq
	ldreq pc, =switch_context

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

