// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef PROCESSOR_H
#define PROCESSOR_H

// Processor modes:
#define PROCESSOR_MODE_USR	0b10000
#define PROCESSOR_MODE_FIQ	0b10001
#define PROCESSOR_MODE_IRQ	0b10010
#define PROCESSOR_MODE_SVC	0b10011
#define PROCESSOR_MODE_MON	0b10110
#define PROCESSOR_MODE_ABT	0b10111
#define PROCESSOR_MODE_HYP	0b11010
#define PROCESSOR_MODE_UND	0b11011
#define PROCESSOR_MODE_SYS	0b11111

// SCTLR bit names
#define SCTLR_TE	(1 << 30)
#define SCTLR_AFE	(1 << 29)
#define SCTLR_TRE	(1 << 28)
#define SCTLR_NMFI	(1 << 27)
#define SCTLR_EE	(1 << 25)
#define SCTLR_VE	(1 << 24)
#define SCTLR_U		(1 << 22)
#define SCTLR_FI	(1 << 21)
#define SCTLR_UWXN	(1 << 20)
#define SCTLR_WXN	(1 << 19)
#define SCTLR_HA	(1 << 17)
#define SCTLR_RR	(1 << 14)
#define SCTLR_V		(1 << 13)
#define SCTLR_I		(1 << 12)
#define SCTLR_Z		(1 << 11)
#define SCTLR_SW	(1 << 10)
#define SCTLR_B		(1 <<  7)
#define SCTLR_CP15BEN	(1 <<  5)
#define SCTLR_C		(1 <<  2)
#define SCTLR_A		(1 <<  1)
#define SCTLR_M		(1 <<  0)

#endif

