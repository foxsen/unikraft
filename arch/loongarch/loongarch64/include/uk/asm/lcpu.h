#ifndef __UKARCH_LCPU_H__
#error Do not include this header directly
#endif

#include <uk/arch/types.h>

#define REGBYTES 8

#define CACHE_LINE_SIZE	64

#define __CALLEE_SAVED_SIZE    (REGBYTES * 16)

#ifndef __ASSEMBLY__
struct __regs {
	/* Temporary registers t0-t8 */
	unsigned long t[9];

	/* Argument/return registers a0-a7 */
	unsigned long a[8];

	/* Saved registers s0-s9 */
	unsigned long s[10];

	/* Return address */
	unsigned long ra;

	/* Thread pointer */
	unsigned long tp;

	/* Stack pointer */
	unsigned long sp;

	/* Program counter */
	unsigned long pc;

	/* Padding for achieving 16-byte structure alignment */
	unsigned long pad;
};

#define __sync()	__asm__ __volatile__("dbar 0" : : : "memory")

#ifndef mb
/* Full memory barrier */
#define mb() __sync()
#endif

#ifndef rmb
/* Read memory barrier */
#define rmb() __sync()
#endif

#ifndef wmb
/* Write memory barrier */
#define wmb() __sync()
#endif

#ifndef nop
#define nop() ({ __asm__ __volatile__("nop" : : : "memory"); })
#endif

#define _csr_read(csr)                                                         \
	({                                                                     \
		register unsigned long __v;                                    \
		__asm__ __volatile__("csrrd %0, " __ASM_STR(csr)               \
				     : "=r"(__v)                               \
				     :                                         \
				     : "memory");                              \
		__v;                                                           \
	})

#define _csr_write(csr, val)                                                   \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrwr %0" __ASM_STR(csr)                 \
				     :                                         \
				     : "rK"(__v)                               \
				     : "memory");                              \
	})

#define _csr_set(csr, val)                                                      \
	({                                                                      \
		unsigned long __v = (unsigned long)(val);                       \
		unsigned long __r;                                              \
		__asm__ __volatile__("csrrd %0, " __ASM_STR(csr) "\n\t"         \
				     "or %0, %0, %1\n\t"                        \
				     "csrwr %0" __ASM_STR(csr)                  \
				     : "=&r"(__r)                               \
				     : "rK"(__v)                                \
				     : "memory");                               \
		__r;                                                            \
	})


//LA similar to csrc inst in RISC-V
#define _csr_clear(csr, val)                                            \
        ({                                                              \
                unsigned long __v = (unsigned long)(val);                         \
                unsigned long __r;                                                \
                __asm__ __volatile__("csrrd %0, " __ASM_STR(csr) "\n\t"           \
                                     "and %0, %0, %1\n\t"                         \
                                     "csrwr %0" __ASM_STR(csr)                    \
                                     : "=&r"(__r)                                 \
                                     : "rK"(-1UL ^ __v)                           \
                                     : "memory");                                 \
    __r;                                                                          \
  })


/* IOCSR */
/* 
 * iocsrrd.d和iocsrwr.d指令只存在于LA64架构，
 * 因此使用到可能需要更改unikraft_loongarch/arch/loongarch/loongarch64中的-march选项
 */

static inline __u8 ioreg_read8(const volatile __u8 *address)
{
	__u8 value;

	asm volatile("iocsrrd.b %0, 0(%1)" : "=r"(value) : "r"(address));
	return value;
}

static inline __u16 ioreg_read16(const volatile __u16 *address)
{
	__u16 value;

	asm volatile("iocsrrd.h %0, 0(%1)" : "=r"(value) : "r"(address));
	return value;
}

static inline __u32 ioreg_read32(const volatile __u32 *address)
{
	__u32 value;

	asm volatile("iocsrrd.w %0, 0(%1)" : "=r"(value) : "r"(address));
	return value;
}

static inline __u64 ioreg_read64(const volatile __u64 *address)
{
	__u64 value;

	asm volatile("iocsrrd.d %0, 0(%1)" : "=r"(value) : "r"(address));
	return value;
}

static inline void ioreg_write8(const volatile __u8 *address, __u8 value)
{
	asm volatile("iocsrwr.b %0, 0(%1)" : : "rZ"(value), "r"(address));
}

static inline void ioreg_write16(const volatile __u16 *address, __u16 value)
{
	asm volatile("iocsrwr.h %0, 0(%1)" : : "rZ"(value), "r"(address));
}

static inline void ioreg_write32(const volatile __u32 *address, __u32 value)
{
	asm volatile("iocsrwr.w %0, 0(%1)" : : "rZ"(value), "r"(address));
}

static inline void ioreg_write64(const volatile __u64 *address, __u64 value)
{
	asm volatile("iocsrwr.d %0, 0(%1)" : : "rZ"(value), "r"(address));
}

static inline unsigned long ukarch_read_sp(void)
{
	unsigned long sp;

	__asm__ __volatile("move %0, sp" : "=&r"(sp));

	return sp;
}

#endif
