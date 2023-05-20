.section .text

.global _liblinuxuplat_start
_liblinuxuplat_start:
	ld.d $a0, $sp, 0		// argc to r0
	addi.d $a1, $sp, 8		// argv to r1
	bl _liblinuxuplat_entry

	// Protection
_liblinuxuplat_err:
	b _liblinuxuplat_err
