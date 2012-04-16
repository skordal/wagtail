/*                     Wagtail OS ARM Linker Script                     */
/*   (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>    */
/* Report bugs and issues on <http://github.com/skordal/wagtail/issues> */

SECTIONS
{
	ramstart = 0x80000000;

	.text 0x80008000 :
	{
		text_start = .;
		start.o(.text)
		*(.text*)
		*(.gnu.linkonce.t*)

		/* Constructors: */
		constructors_start = .;
		*(.init_array)
		constructors_end = .;
	}
	text_end = ALIGN(4096);

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
		*(.bss)
		*(.gnu.linkonce.b*)
	}
	bss_end = ALIGN(4);
	data_end = ALIGN(4096);

	/DISCARD/ :
	{
		
		*(.comment)
		*(.ARM.attributes)
	}
}

