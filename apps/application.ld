/*               Linker Script for Wagtail OS Applications              */
/*   (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>    */
/* Report bugs and issues on <http://github.com/skordal/wagtail/issues> */

SECTIONS
{
	ENTRY(start)

	.text 0x1000 :
	{
		../header.o(.text)
		*(.text*)
		*(.gnu.linkonce.t*)
		constructors_start = .;
		*(.init_array)
		constructors_end = .;
		destructors_start = .;
		*(.fini_array)
		destructors_end = .;
	}

	.data : ALIGN(4096)
	{
		data_start = .;
		*(.rodata*)
		*(.gnu.linkonce.r*)
		*(.data*)
		*(.gnu.linkonce.d*)
	}

	.bss : ALIGN(4)
	{
		bss_start = .;
		*(.bss*)
	}

	application_end = .;
}

