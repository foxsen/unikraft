/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Eduard Vintila <eduard.vintila47@gmail.com>
 *
 * Copyright (c) 2022, University of Bucharest. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
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

#include <uk/config.h>
#include <uk/arch/limits.h>
#include <uk/arch/types.h>
#include <uk/essentials.h>
#include <uk/assert.h>
#include <uk/print.h>
#include <kvm/config.h>
#include <uk/plat/common/irq.h>
#include <loongarch/cpu.h>
#include <loongarch/cpu_defs.h>

extern void __trap_handler(void);
extern void __tlbr_handler(void);

static void do_unknown_exception(struct __regs *regs, unsigned long scause)
{
	unsigned long estat = _csr_read(LOONGARCH_CSR_ESTAT);

	uk_pr_crit(
	    "Unknown exception, scause: %lu, pc: 0x%lx, estat: 0x%lx, sp: "
	    "0x%lx, fp: 0x%lx\n",
	    scause, regs->pc, estat, regs->sp, regs->s[9]);

	/* TODO: dump regs and mem */
	UK_CRASH("Crashing...\n");
}

static void do_page_fault(struct __regs *regs, unsigned long scause __unused)
{
	unsigned long estat = _csr_read(LOONGARCH_CSR_ESTAT);

	uk_pr_crit("Page fault at address 0x%lx, estat : 0x%lx, sp: 0x%lx, "
		   "fp: 0x%lx\n",
		   regs->pc, estat, regs->sp, regs->s[9]);

	/* TODO: dump regs and mem */
	UK_CRASH("Crashing...\n");
}

void _trap_handler(struct __regs *regs)
{
	unsigned long estat = _csr_read(LOONGARCH_CSR_ESTAT);
    int ecode = (estat >> 16) & 0x3f;
    int esubcode = (estat >> 22) & 0x1ff;

	switch (ecode) {
	case 0:
		//plic_handle_irq(regs);
		break;

	case EXCCODE_TLBL:
	case EXCCODE_TLBS:
	case EXCCODE_TLBI:
	case EXCCODE_TLBM:
	case EXCCODE_TLBNX:
	case EXCCODE_TLBNR:
	case EXCCODE_TLBPE:
		do_page_fault(regs, ecode);
		break;

	default:
		do_unknown_exception(regs, ecode);
	}
}

void _init_traps(void)
{
	uintptr_t handler = (uintptr_t)&__trap_handler;

    /* enable all hardware interrupts and timer, use single entrance */
    uint32_t ecfg = 0x7fc; 
    /* enable timer at period mode, with initial counter = 1^24 */
    //uint64_t tcfg = 0x1000003UL;
    uint64_t tcfg = 0x1000000UL;

    _csr_write(LOONGARCH_CSR_ECFG, ecfg);
    _csr_write(LOONGARCH_CSR_TCFG, tcfg);
    _csr_write(LOONGARCH_CSR_EENTRY, (unsigned long)handler);
    _csr_write(LOONGARCH_CSR_TLBRENTRY, (unsigned long)&__tlbr_handler);

	uk_pr_debug("trap inited\n");
}
