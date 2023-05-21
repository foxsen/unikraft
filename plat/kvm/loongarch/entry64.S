#include <uk/arch/limits.h>
#include <uk/asm.h>
#include <uk/plat/common/sections.h>
#include <uk/config.h>

#define LOONGARCH_CSR_CPUID		    0x20	/* CPU core id */

#define LOONGARCH_CSR_DMWIN0		0x180	/* 64 direct map win0: MEM & IF */
#define LOONGARCH_CSR_DMWIN1		0x181	/* 64 direct map win1: MEM & IF */
#define LOONGARCH_CSR_DMWIN2		0x182	/* 64 direct map win2: MEM */
#define LOONGARCH_CSR_DMWIN3		0x183	/* 64 direct map win3: MEM */

.section .bss
.space 4096
bootstack:
.align __STACK_ALIGN_SIZE

.section .text
ENTRY(_libkvmplat_entry)
	/* Setup the temporary bootstack */
	la $sp, bootstack
	
	/* a2 BootParamsInterface data structure address   */
	move $a1, $a2
	bl _libkvmplat_start
spin:
        b           spin

END(_libkvmplat_entry)

ENTRY(__start_mmu)
	/* Enable paging */
	li.d        $t0, 0x8000000000000000   
        csrwr       $t0, LOONGARCH_CSR_DMWIN0
        li.d        $t0, 0x9000000000000011   # CA, PLV0, 0x9000 xxxx xxxx xxxx
        csrwr       $t0, LOONGARCH_CSR_DMWIN1

	ertn
END(__start_mmu)

ENTRY(_libkvmplat_newstack)
	/* Switch to the new stack */
	move $sp, $a0

	/* Load argument for the next function */
	move $a0, $a2

	/* Jump to the next function */
	b  spin
END(_libkvmplat_newstack)
