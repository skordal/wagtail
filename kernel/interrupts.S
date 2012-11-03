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
	movs pc, lr @ Unused interrupt
	b.w int_irq
	b.w int_fiq

@ Undefined instruction handler:
int_undef:
	movs pc, lr

@ Supervisor call handler:
int_svc:
	movs pc, lr

@ Prefetch abort handler:
int_prefetch_abort:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	push {r0 - r3, ip, lr}
	bl prefetch_abort
	pop {r0 - r3, ip, lr}

	rfeia sp!

@ Data abort handler:
int_data_abort:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	push {r0 - r3, ip, lr}
	bl data_abort
	pop {r0 - r3, ip, lr}

	rfeia sp!

@ IRQ handler:
int_irq:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	push {r0 - r3, ip, lr}
	bl handle_irq
	pop {r0 - r3, ip, lr}

	rfeia sp!

@ FIQ handler:
int_fiq:
	sub lr, #4
	srsdb sp!, #PROCESSOR_MODE_SVC
	cps #PROCESSOR_MODE_SVC

	rfeia sp!

