/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Florian Schmidt <florian.schmidt@neclab.eu>
 *          Cyril Soldani <cyril.soldani@uliege.be>
 *          Simon Kuenzer <simon@unikraft.io>
 *          Robert Kuban <robert.kuban@opensynergy.com>
 *
 * Copyright (c) 2019, NEC Europe Ltd., NEC Corporation. All rights reserved.
 * Copyright (c) 2021, University of Liège. All rights reserved.
 * Copyright (c) 2022, NEC Laboratories Europe GmbH, NEC Corporation.
 *                     All rights reserved.
 * Copyright (c) 2022, OpenSynergy GmbH All rights reserved.
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

#ifndef CONFIG_LIBCONTEXT_CLEAR_TBSS
#define CONFIG_LIBCONTEXT_CLEAR_TBSS 1
#endif /* CONFIG_LIBCONTEXT_CLEAR_TBSS */

#include <string.h>
#include <uk/print.h>
#include <uk/hexdump.h>
#include <uk/assert.h>
#include <uk/essentials.h>
#include <uk/arch/types.h>
#include <uk/arch/tls.h>

#if CONFIG_LIBUKDEBUG
#include <uk/assert.h>
#include <uk/print.h>
#else /* !CONFIG_LIBUKDEBUG */
#define UK_ASSERT(..) do {} while (0)
#define uk_pr_debug(..) do {} while (0)
#endif /* !CONFIG_LIBUKDEBUG */

#ifndef __UKARCH_TLS_HAVE_TCB__
#ifndef TCB_SIZE
#define TCB_SIZE 0
#endif /* !TCB_SIZE */
#endif /* !__UKARCH_TLS_HAVE_TCB__ */

extern char _tls_start[], _etdata[], _tls_end[];

/*
 * The TLS area alignment is usually read from the ELF header (p_align of the
 * TLS section).
 * WARNING: It NEEDS to match, as aarch64 uses the alignment at link time to
 * calculate the offset from the tlsp.
 */
#define TLS_AREA_ALIGN 16

static __sz tls_data_offset(void)
{
	return TLS_AREA_ALIGN;
}

static __sz tls_padding_2_size(void)
{
	/*
	 * The tlsp points to the mandatory 16 bytes block, padding 2 fills the
	 * rest of it. The size of this padding is therefore determined by the
	 * ABI.
	 */
	return tls_data_offset();
}

static __sz tls_tdata_size(void)
{
	return (__uptr) _etdata  - (__uptr) _tls_start;
}
static __sz tls_tbss_size(void)
{
	return (__uptr) _tls_end - (__uptr) _etdata;
}

static __sz tls_padding_1_size(void)
{
	/* We do not control:
	 * - TCB_SIZE
	 * - TLS_AREA_ALIGN
	 * - and therefore size of padding 2
	 * Adding tls_data_offset does not break alignment, so we can just align
	 * tlsp. This padding rounds up the size of the TCB before tlsp.
	 */
	const __sz tcb_before_tlsp = TCB_SIZE;

	return ALIGN_UP(tcb_before_tlsp, TLS_AREA_ALIGN) - tcb_before_tlsp;
}

__sz ukarch_tls_area_align(void)
{
	return TLS_AREA_ALIGN;
}

__sz ukarch_tls_area_size(void)
{
	return tls_padding_1_size()
		+ ukarch_tls_tcb_size()
		+ tls_padding_2_size()
		+ tls_tdata_size()
		+ tls_tbss_size();
}

__uptr ukarch_tls_tlsp(void *tls_area)
{
	return ((__uptr) tls_area)
		+ tls_padding_1_size()
		+ ukarch_tls_tcb_size();
}

/* arch pointer ($tp) to area */
void *ukarch_tls_area_get(__uptr tlsp)
{
	return (void *) (tlsp
		- ukarch_tls_tcb_size()
		- tls_padding_1_size());
}

/* arch pointer to tcb pointer */
void *ukarch_tls_tcb_get(__uptr tlsp)
{
	return (void *) (tlsp
		- ukarch_tls_tcb_size());
}

__sz ukarch_tls_tcb_size(void)
{
	return TCB_SIZE;
}

void ukarch_tls_area_init(void *tls_area)
{
	UK_ASSERT(IS_ALIGNED((__uptr) tls_area, ukarch_tls_area_align()));
	uk_pr_debug("tls_area_init: target: %p (%"__PRIsz" bytes)\n",
		    tls_area, ukarch_tls_area_size());

	__u8 *writepos = tls_area;

	/* padding 1 */
	uk_pr_debug("tls_area_init: pad: %"__PRIsz" bytes\n",
		    tls_padding_1_size());
#if CONFIG_LIBCONTEXT_CLEAR_TBSS
	memset(writepos, 0x0, tls_padding_1_size());
#endif /* CONFIG_LIBCONTEXT_CLEAR_TBSS */
	writepos += tls_padding_1_size();

	/* TCB */
	uk_pr_debug("tls_area_init: tcb: %"__PRIsz" bytes\n",
		    ukarch_tls_tcb_size());
	UK_ASSERT(ukarch_tls_tcb_get(ukarch_tls_tlsp(tls_area)) == writepos);
#if CONFIG_UKARCH_TLS_HAVE_TCB
	ukarch_tls_tcb_init(writepos);
#else /* !CONFIG_UKARCH_TLS_HAVE_TCB */
	memset(writepos, 0x0, ukarch_tls_tcb_size());
#endif /*!CONFIG_UKARCH_TLS_HAVE_TCB */
	writepos += ukarch_tls_tcb_size();
	UK_ASSERT(ukarch_tls_tlsp(tls_area) ==
		  ((__uptr) writepos));

	/* padding 2 */
	uk_pr_debug("tls_area_init: pad: %"__PRIsz" bytes\n",
		    tls_padding_2_size());
#if CONFIG_LIBCONTEXT_CLEAR_TBSS
	memset(writepos, 0x0, tls_padding_2_size());
#endif /* CONFIG_LIBCONTEXT_CLEAR_TBSS */
	writepos += tls_padding_2_size();

	/* .tdata */
	uk_pr_debug("tls_area_init: copy (.tdata): %"__PRIsz" bytes\n",
		    tls_tdata_size());
	UK_ASSERT(IS_ALIGNED((__uptr) writepos, ukarch_tls_area_align()));
	memcpy(writepos, _tls_start, tls_tdata_size());
	writepos += tls_tdata_size();

	/* .tbss */
	uk_pr_debug("tls_area_init: uninitialized (.tbss): %"__PRIsz" bytes\n",
		    tls_tdata_size());
#if CONFIG_LIBCONTEXT_CLEAR_TBSS
	memset(writepos, 0x0, tls_tbss_size());
#endif /* CONFIG_LIBCONTEXT_CLEAR_TBSS */
	writepos += tls_tbss_size();

	UK_ASSERT(ukarch_tls_area_size() ==
		  (__sz) (writepos - (__u8 *) tls_area));
	uk_hexdumpCd(tls_area, ukarch_tls_area_size());
}

