/* #include <uk/arch/limits.h> */
#include <uk/plat/common/common.lds.h>

/* QEMU LA virt platform's ram base address */
#define RAM_BASE_ADDR 0x00000000

/* Reserve first 1MB for BIOS */
#define BIOS_RESERVED_SIZE 0x200000

OUTPUT_ARCH( "loongarch" )
ENTRY( _libkvmplat_entry )

PHDRS
{
    text PT_LOAD FLAGS(PHDRS_PF_RX);
    rodata PT_LOAD FLAGS(PHDRS_PF_R);
    data PT_LOAD;
    tls PT_TLS;
    tls_load PT_LOAD;
    stack PT_GNU_STACK FLAGS(PHDRS_PF_RW);
}

SECTIONS {
	. = RAM_BASE_ADDR;
	_start_ram_addr = .;

	_dtb = . + 0x100000;
	. = . + BIOS_RESERVED_SIZE;

	/* Code */
	_text = .;
	.text :
	{
		*(.text)
		*(.text.*)
	} :text

	. = ALIGN(__PAGE_SIZE);
	_etext = .;

   /* Read-only data */
    . = ALIGN(__PAGE_SIZE);
    _rodata = .;
    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
    } :rodata

	EXCEPTION_SECTIONS

	CTORTAB_SECTION

	INITTAB_SECTION

    EVENTTAB_SECTION

	_erodata = .;

	/* Constructor tables (read-only) */
	. = ALIGN(0x8);
	_ctors = .;
	.preinit_array : {
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	}

	. = ALIGN(0x8);
	.init_array : {
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	}
	_ectors = .;
	. = ALIGN(__PAGE_SIZE);

	TLS_SECTIONS

    DATA_SECTIONS

    _end = .;

    .comment       0 : { *(.comment) }
    DEBUG_SYMBOLS

    /DISCARD/ : { *(.note.gnu.build-id) }
}
