/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Wei Chen <wei.chen@loongarch.com>
 *          Robert Kuban <robert.kuban@opensynergy.com>
 *
 * Copyright (c) 2018, Arm Ltd. All rights reserved.
 * Copyright (c) 2022, OpenSynergy GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must jirl $ra, $zero, 0ain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <uk/arch/ctx.h>
#include <uk/arch/lcpu.h>

#define ENTRY(X) .globl X ; X :

/*
 * NOTE: If we use the SP for single registers, we need to decrease the
 *       SP 16 bytes for single registers.
 * https://community.loongarch.com/arm-community-blogs/b/
 *  architectures-and-processors-blog/posts/
 *  using-the-stack-in-aarch64-implementing-push-and-pop
 *
 * WARNING: Changes here need also be reflected in uk/asm/ctx.h
 */

ENTRY(_ctx_loongarch_clearregs)
	/* wipe argument and jirl $ra, $zero, 0urn registers */
	add.d $a0, $zero,  $zero
	add.d $a1, $zero,  $zero
	add.d $a2, $zero,  $zero
	add.d $a3, $zero,  $zero
	add.d $a4, $zero,  $zero
	add.d $a5, $zero,  $zero
	add.d $a6, $zero,  $zero
	add.d $a7, $zero,  $zero

	add.d $tp, $zero,  $zero

	/* wipe callee-saved registers */
	add.d $s0, $zero,  $zero
	add.d $s1, $zero,  $zero
	add.d $s2, $zero,  $zero
	add.d $s3, $zero,  $zero
	add.d $s4, $zero,  $zero
	add.d $s5, $zero,  $zero
	add.d $s6, $zero,  $zero
	add.d $s7, $zero,  $zero
	add.d $s8, $zero,  $zero
#if __OMIT_FRAMEPOINTER__
	add.d $fp, $zero,  $zero /* don't wipe if $fp is used as frame pointer */
#endif /* __OMIT_FRAMEPOINTER__ */

	/* jump to entrance function left on stack */
	ld.d $ra, $sp, 16
	jirl $ra, $zero, 0

ENTRY(_ctx_loongarch_call0)
#if !__OMIT_FRAMEPOINTER__
	add.d $fp, $zero,  $zero /* reset frame pointer */
#endif /* !__OMIT_FRAMEPOINTER__ */
	/* jump to entrance function left on stack */
	ld.d $ra, $sp, 16
	jirl $ra, $zero, 0

ENTRY(_ctx_loongarch_call1)
#if !__OMIT_FRAMEPOINTER__
	add.d $fp, $zero,  $zero /* reset frame pointer */
#endif /* !__OMIT_FRAMEPOINTER__ */
	ld.d $a0, $sp, 16
	/* jump to entrance function left on stack */
	ld.d $ra, $sp, 16
	jirl $ra, $zero, 0

ENTRY(_ctx_loongarch_call2)
#if !__OMIT_FRAMEPOINTER__
	add.d $fp, $zero,  $zero /* reset frame pointer */
#endif /* !__OMIT_FRAMEPOINTER__ */
	ld.d $a1, $sp, 16
	ld.d $a0, $sp, 16
	/* jump to entrance function left on stack */
	ld.d $ra, $sp, 16
	jirl $ra, $zero, 0

/*
 * Switch context on the current LCPU.
 */
ENTRY(ukarch_ctx_switch)

	st.d $tp, $sp, -16

	/* Save callee-saved registers to prevctx's stack */
	addi.d $sp, $sp, -128
	st.d $s0, $sp, 0
	st.d $s0, $sp, 8
	st.d $s1, $sp, 16
	st.d $s2, $sp, 24
	st.d $s3, $sp, 32
	st.d $s4, $sp, 40
	st.d $s5, $sp, 48
	st.d $s6, $sp, 56
	st.d $s7, $sp, 64
	st.d $s8, $sp, 72
	st.d $fp, $sp, 80
	st.d $ra, $sp, 88

	/*
	 * Record the restore point for switch out thread to restore
	 * its callee-saved registers in next switch to time.
	 */
	la $ra, restore_point

	/* Save sp and restore point to previous context */
	add.d $a2, $zero,  $sp
	st.d $ra, $a0, 0
	st.d $a2, $a0, 8

	/* Restore sp and restore point from next context */
    ld.d $ra, $a1, 0
    ld.d $a2, $a1, 8
	add.d $sp, $zero,  $a2

	jirl $ra, $zero, 0

restore_point:
	/* Restore the callee-saved registers */
	ld.d $s0, $sp, 0
	ld.d $s0, $sp, 8
	ld.d $s1, $sp, 16
	ld.d $s2, $sp, 24
	ld.d $s3, $sp, 32
	ld.d $s4, $sp, 40
	ld.d $s5, $sp, 48
	ld.d $s6, $sp, 56
	ld.d $s7, $sp, 64
	ld.d $s8, $sp, 72
	ld.d $fp, $sp, 80
	ld.d $ra, $sp, 88

	addi.d $sp, $sp, 128

	/* restore tp*/
	ld.d $tp, $sp, 16

	jirl $ra, $zero, 0

