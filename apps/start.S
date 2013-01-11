@ Makefile for the Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail/issues>
.syntax unified
.thumb

@ Start code for Wagtail OS applications.
.global start
start:
	@ Zero the .bss section:
	ldr r0, =bss_start
	ldr r1, =application_end
	eors r2, r2

1:	@ Loop through the section:
	cmp r0, r1
	itt ne
	strne r2, [r0], #4
	bne 1b

	@ Run global constructors:
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
	@ Start the application main program:
	@ TODO: Implement command line argument handling.
	movs r0, #0	@ argc
	movs r1, #0	@ argv
	bl main

	@ Run global destructors:
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
	swi #1
	@ As applications cannot quit yet, simply loop forever.
	wfi
	b.n 2b

