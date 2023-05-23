/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Eduard Vintila <eduard.vintila47@gmail.com>
 *
 * Copyright (c) 2022, University of Bucharest. All rights reserved..
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
#ifndef __PLAT_CMN_LOONGARCH64_IRQ_H__
#define __PLAT_CMN_LOONGARCH64_IRQ_H__

#include <loongarch/cpu.h>
#include <loongarch/cpu_defs.h>

#define __set_ie() _csr_set(LOONGARCH_CSR_CRMD, CSR_CRMD_IE)

#define __clear_ie() _csr_clear(LOONGARCH_CSR_CRMD, CSR_CRMD_IE)

#define __save_flags(x)                                                        \
	({                                                                     \
		unsigned long __f;                                             \
		__f = _csr_read(LOONGARCH_CSR_CRMD);                                  \
		x = (__f & CSR_CRMD_IE) ? 1 : 0;                               \
	})

#define __restore_flags(x)                                                     \
	({                                                                     \
		if (x)                                                         \
			__set_ie();                                           \
		else                                                           \
			__clear_ie();                                         \
	})

#define __save_and_clear_ie(x)                                                \
	({                                                                     \
		__save_flags(x);                                               \
		__clear_ie();                                                 \
	})

static inline int irqs_disabled(void)
{
	int flag;

	__save_flags(flag);
	return !flag;
}

#define local_irq_save(x) __save_and_clear_ie(x)
#define local_irq_restore(x) __restore_flags(x)
#define local_save_flags(x) __save_flags(x)
#define local_irq_disable() __clear_ie()
#define local_irq_enable() __set_ie()

#define __MAX_IRQ 1024

#endif /* __PLAT_CMN_LOONGARCH64_IRQ_H__ */
