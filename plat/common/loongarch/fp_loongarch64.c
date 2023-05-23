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
#include <uk/plat/common/cpu.h>

void fpsimd_save_state(uintptr_t ptr)
{
	__u32 fcsr;

	__asm__ __volatile__(
		"movfcsr2gr %0, $zero\n"
		"fld.d $f0, %1, 0\n"
		"fld.d $f1, %1, 8\n"
		"fld.d $f2, %1, 16\n"
		"fld.d $f3, %1, 24\n"
		"fld.d $f4, %1, 32\n"
		"fld.d $f5, %1, 40\n"
		"fld.d $f6, %1, 48\n"
		"fld.d $f7, %1, 56\n"
		"fld.d $f8, %1, 64\n"
		"fld.d $f9, %1, 72\n"
		"fld.d $f10, %1, 80\n"
		"fld.d $f11, %1, 88\n"
		"fld.d $f12, %1, 96\n"
		"fld.d $f13, %1, 104\n"
		"fld.d $f14, %1, 112\n"
		"fld.d $f15, %1, 120\n"
		"fld.d $f16, %1, 128\n"
		"fld.d $f17, %1, 136\n"
		"fld.d $f18, %1, 144\n"
		"fld.d $f19, %1, 152\n"
		"fld.d $f20, %1, 160\n"
		"fld.d $f21, %1, 168\n"
		"fld.d $f22, %1, 176\n"
		"fld.d $f23, %1, 184\n"
		"fld.d $f24, %1, 192\n"
		"fld.d $f25, %1, 200\n"
		"fld.d $f26, %1, 208\n"
		"fld.d $f27, %1, 216\n"
		"fld.d $f28, %1, 224\n"
		"fld.d $f29, %1, 232\n"
		"fld.d $f30, %1, 240\n"
		"fld.d $f31, %1, 248\n"
		: "=&r"(fcsr) : "r"(ptr));

	((struct fpsimd_state *)ptr)->fcsr = fcsr;
}

void fpsimd_restore_state(uintptr_t ptr)
{
	__u32 fcsr;

	fcsr = ((struct fpsimd_state *)ptr)->fcsr;

	__asm__ __volatile__(
		"movgr2fcsr $zero, %0\n"
		"fld.d $f0, %1, 0\n"
		"fld.d $f1, %1, 8\n"
		"fld.d $f2, %1, 16\n"
		"fld.d $f3, %1, 24\n"
		"fld.d $f4, %1, 32\n"
		"fld.d $f5, %1, 40\n"
		"fld.d $f6, %1, 48\n"
		"fld.d $f7, %1, 56\n"
		"fld.d $f8, %1, 64\n"
		"fld.d $f9, %1, 72\n"
		"fld.d $f10, %1, 80\n"
		"fld.d $f11, %1, 88\n"
		"fld.d $f12, %1, 96\n"
		"fld.d $f13, %1, 104\n"
		"fld.d $f14, %1, 112\n"
		"fld.d $f15, %1, 120\n"
		"fld.d $f16, %1, 128\n"
		"fld.d $f17, %1, 136\n"
		"fld.d $f18, %1, 144\n"
		"fld.d $f19, %1, 152\n"
		"fld.d $f20, %1, 160\n"
		"fld.d $f21, %1, 168\n"
		"fld.d $f22, %1, 176\n"
		"fld.d $f23, %1, 184\n"
		"fld.d $f24, %1, 192\n"
		"fld.d $f25, %1, 200\n"
		"fld.d $f26, %1, 208\n"
		"fld.d $f27, %1, 216\n"
		"fld.d $f28, %1, 224\n"
		"fld.d $f29, %1, 232\n"
		"fld.d $f30, %1, 240\n"
		"fld.d $f31, %1, 248\n"
		: : "r"(fcsr), "r"(ptr));
}
