@ Makefile for the Wagtail OS
@ (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/wagtail/issues>
.syntax unified
.thumb

.align 4
@ Executable file header for Wagtail OS applications:
header:
	b.w start		@ Jump to the start of the executable code.
	.byte 'W', 'O', 'S', 1	@ Magic bytes and format version.
	.4byte data_start	@ Start of the data section.
	.4byte bss_start	@ Start of the bss section.
	.4byte application_end	@ End of the application.

@ When an application is loaded, all data between the start of the executable
@ and data_start is mapped as code. All data between data_start and bss_start
@ is mapped as data. The bss section is not zeroed by the OS.

