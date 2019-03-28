// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2014 Pieter Wuille
// Copyright (c) 2018 FA Enterprise system
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//This is an all-in-one version of secp256k1.
//Rationale for this version:
//An attempt to expose internal functions of secp256k1 using the
//fabcoind build system resulted in 6 hours of failed attempts.
//I have therefore decided to use about 3 hours to collapse all secp256k1 headers into one file
//and all implementations into another.
//It turned out to be easier to collapse secp256k1 into a single


#include "secp256k1_all_in_one.h"


#if defined(USE_FIELD_10X26)
#include "field_10x26_impl.h"
#elif defined(USE_FIELD_5X52)
///From secp256k1/src/field_5x52_impl.h
#if defined(USE_ASM_X86_64)
///From secp256k1/src/field_5x52_asm_impl.h
/**********************************************************************
 * Copyright (c) 2013-2014 Diederik Huys, Pieter Wuille               *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

/**
 * Changelog:
 * - March 2013, Diederik Huys:    original version
 * - November 2014, Pieter Wuille: updated to use Peter Dettman's parallel multiplication algorithm
 * - December 2014, Pieter Wuille: converted from YASM to GCC inline assembly
 */
SECP256K1_INLINE void secp256k1_fe_mul_inner(uint64_t *r, const uint64_t *a, const uint64_t * SECP256K1_RESTRICT b) {
/**
 * Registers: rdx:rax = multiplication accumulator
 *            r9:r8   = c
 *            r15:rcx = d
 *            r10-r14 = a0-a4
 *            rbx     = b
 *            rdi     = r
 *            rsi     = a / t?
 */
  uint64_t tmp1, tmp2, tmp3;
__asm__ __volatile__(
    "movq 0(%%rsi),%%r10\n"
    "movq 8(%%rsi),%%r11\n"
    "movq 16(%%rsi),%%r12\n"
    "movq 24(%%rsi),%%r13\n"
    "movq 32(%%rsi),%%r14\n"

    /* d += a3 * b0 */
    "movq 0(%%rbx),%%rax\n"
    "mulq %%r13\n"
    "movq %%rax,%%rcx\n"
    "movq %%rdx,%%r15\n"
    /* d += a2 * b1 */
    "movq 8(%%rbx),%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a1 * b2 */
    "movq 16(%%rbx),%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d = a0 * b3 */
    "movq 24(%%rbx),%%rax\n"
    "mulq %%r10\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* c = a4 * b4 */
    "movq 32(%%rbx),%%rax\n"
    "mulq %%r14\n"
    "movq %%rax,%%r8\n"
    "movq %%rdx,%%r9\n"
    /* d += (c & M) * R */
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* c >>= 52 (%%r8 only) */
    "shrdq $52,%%r9,%%r8\n"
    /* t3 (tmp1) = d & M */
    "movq %%rcx,%%rsi\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rsi\n"
    "movq %%rsi,%q1\n"
    /* d >>= 52 */
    "shrdq $52,%%r15,%%rcx\n"
    "xorq %%r15,%%r15\n"
    /* d += a4 * b0 */
    "movq 0(%%rbx),%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a3 * b1 */
    "movq 8(%%rbx),%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a2 * b2 */
    "movq 16(%%rbx),%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a1 * b3 */
    "movq 24(%%rbx),%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a0 * b4 */
    "movq 32(%%rbx),%%rax\n"
    "mulq %%r10\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += c * R */
    "movq %%r8,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* t4 = d & M (%%rsi) */
    "movq %%rcx,%%rsi\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rsi\n"
    /* d >>= 52 */
    "shrdq $52,%%r15,%%rcx\n"
    "xorq %%r15,%%r15\n"
    /* tx = t4 >> 48 (tmp3) */
    "movq %%rsi,%%rax\n"
    "shrq $48,%%rax\n"
    "movq %%rax,%q3\n"
    /* t4 &= (M >> 4) (tmp2) */
    "movq $0xffffffffffff,%%rax\n"
    "andq %%rax,%%rsi\n"
    "movq %%rsi,%q2\n"
    /* c = a0 * b0 */
    "movq 0(%%rbx),%%rax\n"
    "mulq %%r10\n"
    "movq %%rax,%%r8\n"
    "movq %%rdx,%%r9\n"
    /* d += a4 * b1 */
    "movq 8(%%rbx),%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a3 * b2 */
    "movq 16(%%rbx),%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a2 * b3 */
    "movq 24(%%rbx),%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a1 * b4 */
    "movq 32(%%rbx),%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* u0 = d & M (%%rsi) */
    "movq %%rcx,%%rsi\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rsi\n"
    /* d >>= 52 */
    "shrdq $52,%%r15,%%rcx\n"
    "xorq %%r15,%%r15\n"
    /* u0 = (u0 << 4) | tx (%%rsi) */
    "shlq $4,%%rsi\n"
    "movq %q3,%%rax\n"
    "orq %%rax,%%rsi\n"
    /* c += u0 * (R >> 4) */
    "movq $0x1000003d1,%%rax\n"
    "mulq %%rsi\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* r[0] = c & M */
    "movq %%r8,%%rax\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq %%rax,0(%%rdi)\n"
    /* c >>= 52 */
    "shrdq $52,%%r9,%%r8\n"
    "xorq %%r9,%%r9\n"
    /* c += a1 * b0 */
    "movq 0(%%rbx),%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* c += a0 * b1 */
    "movq 8(%%rbx),%%rax\n"
    "mulq %%r10\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d += a4 * b2 */
    "movq 16(%%rbx),%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a3 * b3 */
    "movq 24(%%rbx),%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a2 * b4 */
    "movq 32(%%rbx),%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* c += (d & M) * R */
    "movq %%rcx,%%rax\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d >>= 52 */
    "shrdq $52,%%r15,%%rcx\n"
    "xorq %%r15,%%r15\n"
    /* r[1] = c & M */
    "movq %%r8,%%rax\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq %%rax,8(%%rdi)\n"
    /* c >>= 52 */
    "shrdq $52,%%r9,%%r8\n"
    "xorq %%r9,%%r9\n"
    /* c += a2 * b0 */
    "movq 0(%%rbx),%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* c += a1 * b1 */
    "movq 8(%%rbx),%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* c += a0 * b2 (last use of %%r10 = a0) */
    "movq 16(%%rbx),%%rax\n"
    "mulq %%r10\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* fetch t3 (%%r10, overwrites a0), t4 (%%rsi) */
    "movq %q2,%%rsi\n"
    "movq %q1,%%r10\n"
    /* d += a4 * b3 */
    "movq 24(%%rbx),%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* d += a3 * b4 */
    "movq 32(%%rbx),%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rcx\n"
    "adcq %%rdx,%%r15\n"
    /* c += (d & M) * R */
    "movq %%rcx,%%rax\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d >>= 52 (%%rcx only) */
    "shrdq $52,%%r15,%%rcx\n"
    /* r[2] = c & M */
    "movq %%r8,%%rax\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq %%rax,16(%%rdi)\n"
    /* c >>= 52 */
    "shrdq $52,%%r9,%%r8\n"
    "xorq %%r9,%%r9\n"
    /* c += t3 */
    "addq %%r10,%%r8\n"
    /* c += d * R */
    "movq %%rcx,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* r[3] = c & M */
    "movq %%r8,%%rax\n"
    "movq $0xfffffffffffff,%%rdx\n"
    "andq %%rdx,%%rax\n"
    "movq %%rax,24(%%rdi)\n"
    /* c >>= 52 (%%r8 only) */
    "shrdq $52,%%r9,%%r8\n"
    /* c += t4 (%%r8 only) */
    "addq %%rsi,%%r8\n"
    /* r[4] = c */
    "movq %%r8,32(%%rdi)\n"
: "+S"(a), "=m"(tmp1), "=m"(tmp2), "=m"(tmp3)
: "b"(b), "D"(r)
: "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "cc", "memory"
);
}

SECP256K1_INLINE void secp256k1_fe_sqr_inner(uint64_t *r, const uint64_t *a) {
/**
 * Registers: rdx:rax = multiplication accumulator
 *            r9:r8   = c
 *            rcx:rbx = d
 *            r10-r14 = a0-a4
 *            r15     = M (0xfffffffffffff)
 *            rdi     = r
 *            rsi     = a / t?
 */
  uint64_t tmp1, tmp2, tmp3;
__asm__ __volatile__(
    "movq 0(%%rsi),%%r10\n"
    "movq 8(%%rsi),%%r11\n"
    "movq 16(%%rsi),%%r12\n"
    "movq 24(%%rsi),%%r13\n"
    "movq 32(%%rsi),%%r14\n"
    "movq $0xfffffffffffff,%%r15\n"

    /* d = (a0*2) * a3 */
    "leaq (%%r10,%%r10,1),%%rax\n"
    "mulq %%r13\n"
    "movq %%rax,%%rbx\n"
    "movq %%rdx,%%rcx\n"
    /* d += (a1*2) * a2 */
    "leaq (%%r11,%%r11,1),%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* c = a4 * a4 */
    "movq %%r14,%%rax\n"
    "mulq %%r14\n"
    "movq %%rax,%%r8\n"
    "movq %%rdx,%%r9\n"
    /* d += (c & M) * R */
    "andq %%r15,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* c >>= 52 (%%r8 only) */
    "shrdq $52,%%r9,%%r8\n"
    /* t3 (tmp1) = d & M */
    "movq %%rbx,%%rsi\n"
    "andq %%r15,%%rsi\n"
    "movq %%rsi,%q1\n"
    /* d >>= 52 */
    "shrdq $52,%%rcx,%%rbx\n"
    "xorq %%rcx,%%rcx\n"
    /* a4 *= 2 */
    "addq %%r14,%%r14\n"
    /* d += a0 * a4 */
    "movq %%r10,%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* d+= (a1*2) * a3 */
    "leaq (%%r11,%%r11,1),%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* d += a2 * a2 */
    "movq %%r12,%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* d += c * R */
    "movq %%r8,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* t4 = d & M (%%rsi) */
    "movq %%rbx,%%rsi\n"
    "andq %%r15,%%rsi\n"
    /* d >>= 52 */
    "shrdq $52,%%rcx,%%rbx\n"
    "xorq %%rcx,%%rcx\n"
    /* tx = t4 >> 48 (tmp3) */
    "movq %%rsi,%%rax\n"
    "shrq $48,%%rax\n"
    "movq %%rax,%q3\n"
    /* t4 &= (M >> 4) (tmp2) */
    "movq $0xffffffffffff,%%rax\n"
    "andq %%rax,%%rsi\n"
    "movq %%rsi,%q2\n"
    /* c = a0 * a0 */
    "movq %%r10,%%rax\n"
    "mulq %%r10\n"
    "movq %%rax,%%r8\n"
    "movq %%rdx,%%r9\n"
    /* d += a1 * a4 */
    "movq %%r11,%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* d += (a2*2) * a3 */
    "leaq (%%r12,%%r12,1),%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* u0 = d & M (%%rsi) */
    "movq %%rbx,%%rsi\n"
    "andq %%r15,%%rsi\n"
    /* d >>= 52 */
    "shrdq $52,%%rcx,%%rbx\n"
    "xorq %%rcx,%%rcx\n"
    /* u0 = (u0 << 4) | tx (%%rsi) */
    "shlq $4,%%rsi\n"
    "movq %q3,%%rax\n"
    "orq %%rax,%%rsi\n"
    /* c += u0 * (R >> 4) */
    "movq $0x1000003d1,%%rax\n"
    "mulq %%rsi\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* r[0] = c & M */
    "movq %%r8,%%rax\n"
    "andq %%r15,%%rax\n"
    "movq %%rax,0(%%rdi)\n"
    /* c >>= 52 */
    "shrdq $52,%%r9,%%r8\n"
    "xorq %%r9,%%r9\n"
    /* a0 *= 2 */
    "addq %%r10,%%r10\n"
    /* c += a0 * a1 */
    "movq %%r10,%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d += a2 * a4 */
    "movq %%r12,%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* d += a3 * a3 */
    "movq %%r13,%%rax\n"
    "mulq %%r13\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* c += (d & M) * R */
    "movq %%rbx,%%rax\n"
    "andq %%r15,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d >>= 52 */
    "shrdq $52,%%rcx,%%rbx\n"
    "xorq %%rcx,%%rcx\n"
    /* r[1] = c & M */
    "movq %%r8,%%rax\n"
    "andq %%r15,%%rax\n"
    "movq %%rax,8(%%rdi)\n"
    /* c >>= 52 */
    "shrdq $52,%%r9,%%r8\n"
    "xorq %%r9,%%r9\n"
    /* c += a0 * a2 (last use of %%r10) */
    "movq %%r10,%%rax\n"
    "mulq %%r12\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* fetch t3 (%%r10, overwrites a0),t4 (%%rsi) */
    "movq %q2,%%rsi\n"
    "movq %q1,%%r10\n"
    /* c += a1 * a1 */
    "movq %%r11,%%rax\n"
    "mulq %%r11\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d += a3 * a4 */
    "movq %%r13,%%rax\n"
    "mulq %%r14\n"
    "addq %%rax,%%rbx\n"
    "adcq %%rdx,%%rcx\n"
    /* c += (d & M) * R */
    "movq %%rbx,%%rax\n"
    "andq %%r15,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* d >>= 52 (%%rbx only) */
    "shrdq $52,%%rcx,%%rbx\n"
    /* r[2] = c & M */
    "movq %%r8,%%rax\n"
    "andq %%r15,%%rax\n"
    "movq %%rax,16(%%rdi)\n"
    /* c >>= 52 */
    "shrdq $52,%%r9,%%r8\n"
    "xorq %%r9,%%r9\n"
    /* c += t3 */
    "addq %%r10,%%r8\n"
    /* c += d * R */
    "movq %%rbx,%%rax\n"
    "movq $0x1000003d10,%%rdx\n"
    "mulq %%rdx\n"
    "addq %%rax,%%r8\n"
    "adcq %%rdx,%%r9\n"
    /* r[3] = c & M */
    "movq %%r8,%%rax\n"
    "andq %%r15,%%rax\n"
    "movq %%rax,24(%%rdi)\n"
    /* c >>= 52 (%%r8 only) */
    "shrdq $52,%%r9,%%r8\n"
    /* c += t4 (%%r8 only) */
    "addq %%rsi,%%r8\n"
    /* r[4] = c */
    "movq %%r8,32(%%rdi)\n"
: "+S"(a), "=m"(tmp1), "=m"(tmp2), "=m"(tmp3)
: "D"(r)
: "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "cc", "memory"
);
}
///End of secp256k1/src/field_5x52_asm_impl.h
#else
#include "field_5x52_int128_impl.h"
#endif

/** Implements arithmetic modulo FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F,
 *  represented as 5 uint64_t's in base 2^52. The values are allowed to contain >52 each. In particular,
 *  each FieldElem has a 'magnitude' associated with it. Internally, a magnitude M means each element
 *  is at most M*(2^53-1), except the most significant one, which is limited to M*(2^49-1). All operations
 *  accept any input with magnitude at most M, and have different rules for propagating magnitude to their
 *  output.
 */

#ifdef VERIFY
void secp256k1_fe_verify(const secp256k1_fe *a) {
    const uint64_t *d = a->n;
    int m = a->normalized ? 1 : 2 * a->magnitude, r = 1;
   /* secp256k1 'p' value defined in "Standards for Efficient Cryptography" (SEC2) 2.7.1. */
    r &= (d[0] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[1] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[2] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[3] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[4] <= 0x0FFFFFFFFFFFFULL * m);
    r &= (a->magnitude >= 0);
    r &= (a->magnitude <= 2048);
    if (a->normalized) {
        r &= (a->magnitude <= 1);
        if (r && (d[4] == 0x0FFFFFFFFFFFFULL) && ((d[3] & d[2] & d[1]) == 0xFFFFFFFFFFFFFULL)) {
            r &= (d[0] < 0xFFFFEFFFFFC2FULL);
        }
    }
    VERIFY_CHECK(r == 1);
}
#endif

void secp256k1_fe_normalize(secp256k1_fe *r) {
    uint64_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4];

    /* Reduce t4 at the start so there will be at most a single carry from the first pass */
    uint64_t m;
    uint64_t x = t4 >> 48; t4 &= 0x0FFFFFFFFFFFFULL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x1000003D1ULL;
    t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL;
    t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL; m = t1;
    t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL; m &= t2;
    t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL; m &= t3;

    /* ... except for a possible carry at bit 48 of t4 (i.e. bit 256 of the field element) */
    VERIFY_CHECK(t4 >> 49 == 0);

    /* At most a single final reduction is needed; check if the value is >= the field characteristic */
    x = (t4 >> 48) | ((t4 == 0x0FFFFFFFFFFFFULL) & (m == 0xFFFFFFFFFFFFFULL)
        & (t0 >= 0xFFFFEFFFFFC2FULL));

    /* Apply the final reduction (for constant-time behaviour, we do it always) */
    t0 += x * 0x1000003D1ULL;
    t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL;
    t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL;
    t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL;
    t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL;

    /* If t4 didn't carry to bit 48 already, then it should have after any final reduction */
    VERIFY_CHECK(t4 >> 48 == x);

    /* Mask off the possible multiple of 2^256 from the final reduction */
    t4 &= 0x0FFFFFFFFFFFFULL;

    r->n[0] = t0; r->n[1] = t1; r->n[2] = t2; r->n[3] = t3; r->n[4] = t4;

#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 1;
    secp256k1_fe_verify(r);
#endif
}

void secp256k1_fe_normalize_weak(secp256k1_fe *r) {
    uint64_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4];

    /* Reduce t4 at the start so there will be at most a single carry from the first pass */
    uint64_t x = t4 >> 48; t4 &= 0x0FFFFFFFFFFFFULL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x1000003D1ULL;
    t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL;
    t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL;
    t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL;
    t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL;

    /* ... except for a possible carry at bit 48 of t4 (i.e. bit 256 of the field element) */
    VERIFY_CHECK(t4 >> 49 == 0);

    r->n[0] = t0; r->n[1] = t1; r->n[2] = t2; r->n[3] = t3; r->n[4] = t4;

#ifdef VERIFY
    r->magnitude = 1;
    secp256k1_fe_verify(r);
#endif
}

void secp256k1_fe_normalize_var(secp256k1_fe *r) {
    uint64_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4];

    /* Reduce t4 at the start so there will be at most a single carry from the first pass */
    uint64_t m;
    uint64_t x = t4 >> 48; t4 &= 0x0FFFFFFFFFFFFULL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x1000003D1ULL;
    t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL;
    t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL; m = t1;
    t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL; m &= t2;
    t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL; m &= t3;

    /* ... except for a possible carry at bit 48 of t4 (i.e. bit 256 of the field element) */
    VERIFY_CHECK(t4 >> 49 == 0);

    /* At most a single final reduction is needed; check if the value is >= the field characteristic */
    x = (t4 >> 48) | ((t4 == 0x0FFFFFFFFFFFFULL) & (m == 0xFFFFFFFFFFFFFULL)
        & (t0 >= 0xFFFFEFFFFFC2FULL));

    if (x) {
        t0 += 0x1000003D1ULL;
        t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL;
        t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL;
        t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL;
        t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL;

        /* If t4 didn't carry to bit 48 already, then it should have after any final reduction */
        VERIFY_CHECK(t4 >> 48 == x);

        /* Mask off the possible multiple of 2^256 from the final reduction */
        t4 &= 0x0FFFFFFFFFFFFULL;
    }

    r->n[0] = t0; r->n[1] = t1; r->n[2] = t2; r->n[3] = t3; r->n[4] = t4;

#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 1;
    secp256k1_fe_verify(r);
#endif
}

int secp256k1_fe_normalizes_to_zero(secp256k1_fe *r) {
    uint64_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4];

    /* z0 tracks a possible raw value of 0, z1 tracks a possible raw value of P */
    uint64_t z0, z1;

    /* Reduce t4 at the start so there will be at most a single carry from the first pass */
    uint64_t x = t4 >> 48; t4 &= 0x0FFFFFFFFFFFFULL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x1000003D1ULL;
    t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL; z0  = t0; z1  = t0 ^ 0x1000003D0ULL;
    t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL; z0 |= t1; z1 &= t1;
    t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL; z0 |= t2; z1 &= t2;
    t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL; z0 |= t3; z1 &= t3;
                                                z0 |= t4; z1 &= t4 ^ 0xF000000000000ULL;

    /* ... except for a possible carry at bit 48 of t4 (i.e. bit 256 of the field element) */
    VERIFY_CHECK(t4 >> 49 == 0);

    return (z0 == 0) | (z1 == 0xFFFFFFFFFFFFFULL);
}

int secp256k1_fe_normalizes_to_zero_var(secp256k1_fe *r) {
    uint64_t t0, t1, t2, t3, t4;
    uint64_t z0, z1;
    uint64_t x;

    t0 = r->n[0];
    t4 = r->n[4];

    /* Reduce t4 at the start so there will be at most a single carry from the first pass */
    x = t4 >> 48;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x1000003D1ULL;

    /* z0 tracks a possible raw value of 0, z1 tracks a possible raw value of P */
    z0 = t0 & 0xFFFFFFFFFFFFFULL;
    z1 = z0 ^ 0x1000003D0ULL;

    /* Fast return path should catch the majority of cases */
    if ((z0 != 0ULL) & (z1 != 0xFFFFFFFFFFFFFULL)) {
        return 0;
    }

    t1 = r->n[1];
    t2 = r->n[2];
    t3 = r->n[3];

    t4 &= 0x0FFFFFFFFFFFFULL;

    t1 += (t0 >> 52);
    t2 += (t1 >> 52); t1 &= 0xFFFFFFFFFFFFFULL; z0 |= t1; z1 &= t1;
    t3 += (t2 >> 52); t2 &= 0xFFFFFFFFFFFFFULL; z0 |= t2; z1 &= t2;
    t4 += (t3 >> 52); t3 &= 0xFFFFFFFFFFFFFULL; z0 |= t3; z1 &= t3;
                                                z0 |= t4; z1 &= t4 ^ 0xF000000000000ULL;

    /* ... except for a possible carry at bit 48 of t4 (i.e. bit 256 of the field element) */
    VERIFY_CHECK(t4 >> 49 == 0);

    return (z0 == 0) | (z1 == 0xFFFFFFFFFFFFFULL);
}

SECP256K1_INLINE void secp256k1_fe_set_int(secp256k1_fe *r, int a) {
    r->n[0] = a;
    r->n[1] = r->n[2] = r->n[3] = r->n[4] = 0;
#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 1;
    secp256k1_fe_verify(r);
#endif
}

SECP256K1_INLINE int secp256k1_fe_is_zero(const secp256k1_fe *a) {
    const uint64_t *t = a->n;
#ifdef VERIFY
    VERIFY_CHECK(a->normalized);
    secp256k1_fe_verify(a);
#endif
    return (t[0] | t[1] | t[2] | t[3] | t[4]) == 0;
}

SECP256K1_INLINE int secp256k1_fe_is_odd(const secp256k1_fe *a) {
#ifdef VERIFY
    VERIFY_CHECK(a->normalized);
    secp256k1_fe_verify(a);
#endif
    return a->n[0] & 1;
}

SECP256K1_INLINE void secp256k1_fe_clear(secp256k1_fe *a) {
    int i;
#ifdef VERIFY
    a->magnitude = 0;
    a->normalized = 1;
#endif
    for (i=0; i<5; i++) {
        a->n[i] = 0;
    }
}

int secp256k1_fe_cmp_var(const secp256k1_fe *a, const secp256k1_fe *b) {
    int i;
#ifdef VERIFY
    VERIFY_CHECK(a->normalized);
    VERIFY_CHECK(b->normalized);
    secp256k1_fe_verify(a);
    secp256k1_fe_verify(b);
#endif
    for (i = 4; i >= 0; i--) {
        if (a->n[i] > b->n[i]) {
            return 1;
        }
        if (a->n[i] < b->n[i]) {
            return -1;
        }
    }
    return 0;
}

int secp256k1_fe_set_b32(secp256k1_fe *r, const unsigned char *a) {
    r->n[0] = (uint64_t)a[31]
            | ((uint64_t)a[30] << 8)
            | ((uint64_t)a[29] << 16)
            | ((uint64_t)a[28] << 24)
            | ((uint64_t)a[27] << 32)
            | ((uint64_t)a[26] << 40)
            | ((uint64_t)(a[25] & 0xF)  << 48);
    r->n[1] = (uint64_t)((a[25] >> 4) & 0xF)
            | ((uint64_t)a[24] << 4)
            | ((uint64_t)a[23] << 12)
            | ((uint64_t)a[22] << 20)
            | ((uint64_t)a[21] << 28)
            | ((uint64_t)a[20] << 36)
            | ((uint64_t)a[19] << 44);
    r->n[2] = (uint64_t)a[18]
            | ((uint64_t)a[17] << 8)
            | ((uint64_t)a[16] << 16)
            | ((uint64_t)a[15] << 24)
            | ((uint64_t)a[14] << 32)
            | ((uint64_t)a[13] << 40)
            | ((uint64_t)(a[12] & 0xF) << 48);
    r->n[3] = (uint64_t)((a[12] >> 4) & 0xF)
            | ((uint64_t)a[11] << 4)
            | ((uint64_t)a[10] << 12)
            | ((uint64_t)a[9]  << 20)
            | ((uint64_t)a[8]  << 28)
            | ((uint64_t)a[7]  << 36)
            | ((uint64_t)a[6]  << 44);
    r->n[4] = (uint64_t)a[5]
            | ((uint64_t)a[4] << 8)
            | ((uint64_t)a[3] << 16)
            | ((uint64_t)a[2] << 24)
            | ((uint64_t)a[1] << 32)
            | ((uint64_t)a[0] << 40);
    if (r->n[4] == 0x0FFFFFFFFFFFFULL && (r->n[3] & r->n[2] & r->n[1]) == 0xFFFFFFFFFFFFFULL && r->n[0] >= 0xFFFFEFFFFFC2FULL) {
        return 0;
    }
#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 1;
    secp256k1_fe_verify(r);
#endif
    return 1;
}

/** Convert a field element to a 32-byte big endian value. Requires the input to be normalized */
void secp256k1_fe_get_b32(unsigned char *r, const secp256k1_fe *a) {
#ifdef VERIFY
    VERIFY_CHECK(a->normalized);
    secp256k1_fe_verify(a);
#endif
    r[0] = (a->n[4] >> 40) & 0xFF;
    r[1] = (a->n[4] >> 32) & 0xFF;
    r[2] = (a->n[4] >> 24) & 0xFF;
    r[3] = (a->n[4] >> 16) & 0xFF;
    r[4] = (a->n[4] >> 8) & 0xFF;
    r[5] = a->n[4] & 0xFF;
    r[6] = (a->n[3] >> 44) & 0xFF;
    r[7] = (a->n[3] >> 36) & 0xFF;
    r[8] = (a->n[3] >> 28) & 0xFF;
    r[9] = (a->n[3] >> 20) & 0xFF;
    r[10] = (a->n[3] >> 12) & 0xFF;
    r[11] = (a->n[3] >> 4) & 0xFF;
    r[12] = ((a->n[2] >> 48) & 0xF) | ((a->n[3] & 0xF) << 4);
    r[13] = (a->n[2] >> 40) & 0xFF;
    r[14] = (a->n[2] >> 32) & 0xFF;
    r[15] = (a->n[2] >> 24) & 0xFF;
    r[16] = (a->n[2] >> 16) & 0xFF;
    r[17] = (a->n[2] >> 8) & 0xFF;
    r[18] = a->n[2] & 0xFF;
    r[19] = (a->n[1] >> 44) & 0xFF;
    r[20] = (a->n[1] >> 36) & 0xFF;
    r[21] = (a->n[1] >> 28) & 0xFF;
    r[22] = (a->n[1] >> 20) & 0xFF;
    r[23] = (a->n[1] >> 12) & 0xFF;
    r[24] = (a->n[1] >> 4) & 0xFF;
    r[25] = ((a->n[0] >> 48) & 0xF) | ((a->n[1] & 0xF) << 4);
    r[26] = (a->n[0] >> 40) & 0xFF;
    r[27] = (a->n[0] >> 32) & 0xFF;
    r[28] = (a->n[0] >> 24) & 0xFF;
    r[29] = (a->n[0] >> 16) & 0xFF;
    r[30] = (a->n[0] >> 8) & 0xFF;
    r[31] = a->n[0] & 0xFF;
}

SECP256K1_INLINE void secp256k1_fe_negate(secp256k1_fe *r, const secp256k1_fe *a, int m) {
#ifdef VERIFY
    VERIFY_CHECK(a->magnitude <= m);
    secp256k1_fe_verify(a);
#endif
    r->n[0] = 0xFFFFEFFFFFC2FULL * 2 * (m + 1) - a->n[0];
    r->n[1] = 0xFFFFFFFFFFFFFULL * 2 * (m + 1) - a->n[1];
    r->n[2] = 0xFFFFFFFFFFFFFULL * 2 * (m + 1) - a->n[2];
    r->n[3] = 0xFFFFFFFFFFFFFULL * 2 * (m + 1) - a->n[3];
    r->n[4] = 0x0FFFFFFFFFFFFULL * 2 * (m + 1) - a->n[4];
#ifdef VERIFY
    r->magnitude = m + 1;
    r->normalized = 0;
    secp256k1_fe_verify(r);
#endif
}

SECP256K1_INLINE void secp256k1_fe_mul_int(secp256k1_fe *r, int a) {
    r->n[0] *= a;
    r->n[1] *= a;
    r->n[2] *= a;
    r->n[3] *= a;
    r->n[4] *= a;
#ifdef VERIFY
    r->magnitude *= a;
    r->normalized = 0;
    secp256k1_fe_verify(r);
#endif
}

SECP256K1_INLINE void secp256k1_fe_add(secp256k1_fe *r, const secp256k1_fe *a) {
#ifdef VERIFY
    secp256k1_fe_verify(a);
#endif
    r->n[0] += a->n[0];
    r->n[1] += a->n[1];
    r->n[2] += a->n[2];
    r->n[3] += a->n[3];
    r->n[4] += a->n[4];
#ifdef VERIFY
    r->magnitude += a->magnitude;
    r->normalized = 0;
    secp256k1_fe_verify(r);
#endif
}

void secp256k1_fe_mul(secp256k1_fe *r, const secp256k1_fe *a, const secp256k1_fe * SECP256K1_RESTRICT b) {
#ifdef VERIFY
    VERIFY_CHECK(a->magnitude <= 8);
    VERIFY_CHECK(b->magnitude <= 8);
    secp256k1_fe_verify(a);
    secp256k1_fe_verify(b);
    VERIFY_CHECK(r != b);
#endif
    secp256k1_fe_mul_inner(r->n, a->n, b->n);
#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 0;
    secp256k1_fe_verify(r);
#endif
}

void secp256k1_fe_sqr(secp256k1_fe *r, const secp256k1_fe *a) {
#ifdef VERIFY
    VERIFY_CHECK(a->magnitude <= 8);
    secp256k1_fe_verify(a);
#endif
    secp256k1_fe_sqr_inner(r->n, a->n);
#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 0;
    secp256k1_fe_verify(r);
#endif
}

SECP256K1_INLINE void secp256k1_fe_cmov(secp256k1_fe *r, const secp256k1_fe *a, int flag) {
    uint64_t mask0, mask1;
    mask0 = flag + ~((uint64_t)0);
    mask1 = ~mask0;
    r->n[0] = (r->n[0] & mask0) | (a->n[0] & mask1);
    r->n[1] = (r->n[1] & mask0) | (a->n[1] & mask1);
    r->n[2] = (r->n[2] & mask0) | (a->n[2] & mask1);
    r->n[3] = (r->n[3] & mask0) | (a->n[3] & mask1);
    r->n[4] = (r->n[4] & mask0) | (a->n[4] & mask1);
#ifdef VERIFY
    if (a->magnitude > r->magnitude) {
        r->magnitude = a->magnitude;
    }
    r->normalized &= a->normalized;
#endif
}

SECP256K1_INLINE void secp256k1_fe_storage_cmov(secp256k1_fe_storage *r, const secp256k1_fe_storage *a, int flag) {
    uint64_t mask0, mask1;
    mask0 = flag + ~((uint64_t)0);
    mask1 = ~mask0;
    r->n[0] = (r->n[0] & mask0) | (a->n[0] & mask1);
    r->n[1] = (r->n[1] & mask0) | (a->n[1] & mask1);
    r->n[2] = (r->n[2] & mask0) | (a->n[2] & mask1);
    r->n[3] = (r->n[3] & mask0) | (a->n[3] & mask1);
}

void secp256k1_fe_to_storage(secp256k1_fe_storage *r, const secp256k1_fe *a) {
#ifdef VERIFY
    VERIFY_CHECK(a->normalized);
#endif
    r->n[0] = a->n[0] | a->n[1] << 52;
    r->n[1] = a->n[1] >> 12 | a->n[2] << 40;
    r->n[2] = a->n[2] >> 24 | a->n[3] << 28;
    r->n[3] = a->n[3] >> 36 | a->n[4] << 16;
}

SECP256K1_INLINE void secp256k1_fe_from_storage(secp256k1_fe *r, const secp256k1_fe_storage *a) {
    r->n[0] = a->n[0] & 0xFFFFFFFFFFFFFULL;
    r->n[1] = a->n[0] >> 52 | ((a->n[1] << 12) & 0xFFFFFFFFFFFFFULL);
    r->n[2] = a->n[1] >> 40 | ((a->n[2] << 24) & 0xFFFFFFFFFFFFFULL);
    r->n[3] = a->n[2] >> 28 | ((a->n[3] << 36) & 0xFFFFFFFFFFFFFULL);
    r->n[4] = a->n[3] >> 16;
#ifdef VERIFY
    r->magnitude = 1;
    r->normalized = 1;
#endif
}
///End of secp256k1/src/field_5x52_impl.h














#else
#error "Please select field implementation"
#endif

SECP256K1_INLINE int secp256k1_fe_equal(const secp256k1_fe *a, const secp256k1_fe *b) {
    secp256k1_fe na;
    secp256k1_fe_negate(&na, a, 1);
    secp256k1_fe_add(&na, b);
    return secp256k1_fe_normalizes_to_zero(&na);
}

int secp256k1_fe_equal_var(const secp256k1_fe *a, const secp256k1_fe *b) {
    secp256k1_fe na;
    secp256k1_fe_negate(&na, a, 1);
    secp256k1_fe_add(&na, b);
    return secp256k1_fe_normalizes_to_zero_var(&na);
}

int secp256k1_fe_sqrt(secp256k1_fe *r, const secp256k1_fe *a) {
    /** Given that p is congruent to 3 mod 4, we can compute the square root of
     *  a mod p as the (p+1)/4'th power of a.
     *
     *  As (p+1)/4 is an even number, it will have the same result for a and for
     *  (-a). Only one of these two numbers actually has a square root however,
     *  so we test at the end by squaring and comparing to the input.
     *  Also because (p+1)/4 is an even number, the computed square root is
     *  itself always a square (a ** ((p+1)/4) is the square of a ** ((p+1)/8)).
     */
    secp256k1_fe x2, x3, x6, x9, x11, x22, x44, x88, x176, x220, x223, t1;
    int j;

    /** The binary representation of (p + 1)/4 has 3 blocks of 1s, with lengths in
     *  { 2, 22, 223 }. Use an addition chain to calculate 2^n - 1 for each block:
     *  1, [2], 3, 6, 9, 11, [22], 44, 88, 176, 220, [223]
     */

    secp256k1_fe_sqr(&x2, a);
    secp256k1_fe_mul(&x2, &x2, a);

    secp256k1_fe_sqr(&x3, &x2);
    secp256k1_fe_mul(&x3, &x3, a);

    x6 = x3;
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&x6, &x6);
    }
    secp256k1_fe_mul(&x6, &x6, &x3);

    x9 = x6;
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&x9, &x9);
    }
    secp256k1_fe_mul(&x9, &x9, &x3);

    x11 = x9;
    for (j=0; j<2; j++) {
        secp256k1_fe_sqr(&x11, &x11);
    }
    secp256k1_fe_mul(&x11, &x11, &x2);

    x22 = x11;
    for (j=0; j<11; j++) {
        secp256k1_fe_sqr(&x22, &x22);
    }
    secp256k1_fe_mul(&x22, &x22, &x11);

    x44 = x22;
    for (j=0; j<22; j++) {
        secp256k1_fe_sqr(&x44, &x44);
    }
    secp256k1_fe_mul(&x44, &x44, &x22);

    x88 = x44;
    for (j=0; j<44; j++) {
        secp256k1_fe_sqr(&x88, &x88);
    }
    secp256k1_fe_mul(&x88, &x88, &x44);

    x176 = x88;
    for (j=0; j<88; j++) {
        secp256k1_fe_sqr(&x176, &x176);
    }
    secp256k1_fe_mul(&x176, &x176, &x88);

    x220 = x176;
    for (j=0; j<44; j++) {
        secp256k1_fe_sqr(&x220, &x220);
    }
    secp256k1_fe_mul(&x220, &x220, &x44);

    x223 = x220;
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&x223, &x223);
    }
    secp256k1_fe_mul(&x223, &x223, &x3);

    /* The final result is then assembled using a sliding window over the blocks. */

    t1 = x223;
    for (j=0; j<23; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x22);
    for (j=0; j<6; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x2);
    secp256k1_fe_sqr(&t1, &t1);
    secp256k1_fe_sqr(r, &t1);

    /* Check that a square root was actually calculated */

    secp256k1_fe_sqr(&t1, r);
    return secp256k1_fe_equal(&t1, a);
}

void secp256k1_fe_inv(secp256k1_fe *r, const secp256k1_fe *a) {
    secp256k1_fe x2, x3, x6, x9, x11, x22, x44, x88, x176, x220, x223, t1;
    int j;

    /** The binary representation of (p - 2) has 5 blocks of 1s, with lengths in
     *  { 1, 2, 22, 223 }. Use an addition chain to calculate 2^n - 1 for each block:
     *  [1], [2], 3, 6, 9, 11, [22], 44, 88, 176, 220, [223]
     */

    secp256k1_fe_sqr(&x2, a);
    secp256k1_fe_mul(&x2, &x2, a);

    secp256k1_fe_sqr(&x3, &x2);
    secp256k1_fe_mul(&x3, &x3, a);

    x6 = x3;
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&x6, &x6);
    }
    secp256k1_fe_mul(&x6, &x6, &x3);

    x9 = x6;
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&x9, &x9);
    }
    secp256k1_fe_mul(&x9, &x9, &x3);

    x11 = x9;
    for (j=0; j<2; j++) {
        secp256k1_fe_sqr(&x11, &x11);
    }
    secp256k1_fe_mul(&x11, &x11, &x2);

    x22 = x11;
    for (j=0; j<11; j++) {
        secp256k1_fe_sqr(&x22, &x22);
    }
    secp256k1_fe_mul(&x22, &x22, &x11);

    x44 = x22;
    for (j=0; j<22; j++) {
        secp256k1_fe_sqr(&x44, &x44);
    }
    secp256k1_fe_mul(&x44, &x44, &x22);

    x88 = x44;
    for (j=0; j<44; j++) {
        secp256k1_fe_sqr(&x88, &x88);
    }
    secp256k1_fe_mul(&x88, &x88, &x44);

    x176 = x88;
    for (j=0; j<88; j++) {
        secp256k1_fe_sqr(&x176, &x176);
    }
    secp256k1_fe_mul(&x176, &x176, &x88);

    x220 = x176;
    for (j=0; j<44; j++) {
        secp256k1_fe_sqr(&x220, &x220);
    }
    secp256k1_fe_mul(&x220, &x220, &x44);

    x223 = x220;
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&x223, &x223);
    }
    secp256k1_fe_mul(&x223, &x223, &x3);

    /* The final result is then assembled using a sliding window over the blocks. */

    t1 = x223;
    for (j=0; j<23; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x22);
    for (j=0; j<5; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, a);
    for (j=0; j<3; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(&t1, &t1, &x2);
    for (j=0; j<2; j++) {
        secp256k1_fe_sqr(&t1, &t1);
    }
    secp256k1_fe_mul(r, a, &t1);
}

void secp256k1_fe_inv_var(secp256k1_fe *r, const secp256k1_fe *a) {
#if defined(USE_FIELD_INV_BUILTIN)
    secp256k1_fe_inv(r, a);
#elif defined(USE_FIELD_INV_NUM)
    secp256k1_num n, m;
    static const secp256k1_fe negone = SECP256K1_FE_CONST(
        0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL,
        0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFEUL, 0xFFFFFC2EUL
    );
    /* secp256k1 field prime, value p defined in "Standards for Efficient Cryptography" (SEC2) 2.7.1. */
    static const unsigned char prime[32] = {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFC,0x2F
    };
    unsigned char b[32];
    int res;
    secp256k1_fe c = *a;
    secp256k1_fe_normalize_var(&c);
    secp256k1_fe_get_b32(b, &c);
    secp256k1_num_set_bin(&n, b, 32);
    secp256k1_num_set_bin(&m, prime, 32);
    secp256k1_num_mod_inverse(&n, &n, &m);
    secp256k1_num_get_bin(b, 32, &n);
    res = secp256k1_fe_set_b32(r, b);
    (void)res;
    VERIFY_CHECK(res);
    /* Verify the result is the (unique) valid inverse using non-GMP code. */
    secp256k1_fe_mul(&c, &c, r);
    secp256k1_fe_add(&c, &negone);
    CHECK(secp256k1_fe_normalizes_to_zero_var(&c));
#else
#error "Please select field inverse implementation"
#endif
}

void secp256k1_fe_inv_all_var(secp256k1_fe *r, const secp256k1_fe *a, size_t len) {
    secp256k1_fe u;
    size_t i;
    if (len < 1) {
        return;
    }

    VERIFY_CHECK((r + len <= a) || (a + len <= r));

    r[0] = a[0];

    i = 0;
    while (++i < len) {
        secp256k1_fe_mul(&r[i], &r[i - 1], &a[i]);
    }

    secp256k1_fe_inv_var(&u, &r[--i]);

    while (i > 0) {
        size_t j = i--;
        secp256k1_fe_mul(&r[j], &r[i], &u);
        secp256k1_fe_mul(&u, &u, &a[j]);
    }

    r[0] = u;
}

int secp256k1_fe_is_quad_var(const secp256k1_fe *a) {
#ifndef USE_NUM_NONE
    unsigned char b[32];
    secp256k1_num n;
    secp256k1_num m;
    /* secp256k1 field prime, value p defined in "Standards for Efficient Cryptography" (SEC2) 2.7.1. */
    static const unsigned char prime[32] = {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFC,0x2F
    };

    secp256k1_fe c = *a;
    secp256k1_fe_normalize_var(&c);
    secp256k1_fe_get_b32(b, &c);
    secp256k1_num_set_bin(&n, b, 32);
    secp256k1_num_set_bin(&m, prime, 32);
    return secp256k1_num_jacobi(&n, &m) >= 0;
#else
    secp256k1_fe r;
    return secp256k1_fe_sqrt(&r, a);
#endif
}






///From secp256k1/src/field_impl.h
#if defined(USE_FIELD_10X26)
#include "field_10x26_impl.h"
#elif defined(USE_FIELD_5X52)
///From secp256k1/src/field_5x52_impl.h
#if defined(USE_ASM_X86_64)
//already included above:
//#include "field_5x52_asm_impl.h"
#else
#include "field_5x52_int128_impl.h"
#endif

/** Implements arithmetic modulo FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F,
 *  represented as 5 uint64_t's in base 2^52. The values are allowed to contain >52 each. In particular,
 *  each FieldElem has a 'magnitude' associated with it. Internally, a magnitude M means each element
 *  is at most M*(2^53-1), except the most significant one, which is limited to M*(2^49-1). All operations
 *  accept any input with magnitude at most M, and have different rules for propagating magnitude to their
 *  output.
 */

#ifdef VERIFY
void secp256k1_fe_verify(const secp256k1_fe *a) {
    const uint64_t *d = a->n;
    int m = a->normalized ? 1 : 2 * a->magnitude, r = 1;
   /* secp256k1 'p' value defined in "Standards for Efficient Cryptography" (SEC2) 2.7.1. */
    r &= (d[0] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[1] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[2] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[3] <= 0xFFFFFFFFFFFFFULL * m);
    r &= (d[4] <= 0x0FFFFFFFFFFFFULL * m);
    r &= (a->magnitude >= 0);
    r &= (a->magnitude <= 2048);
    if (a->normalized) {
        r &= (a->magnitude <= 1);
        if (r && (d[4] == 0x0FFFFFFFFFFFFULL) && ((d[3] & d[2] & d[1]) == 0xFFFFFFFFFFFFFULL)) {
            r &= (d[0] < 0xFFFFEFFFFFC2FULL);
        }
    }
    VERIFY_CHECK(r == 1);
}
#endif
///End of secp256k1/src/field_5x52_impl.h
#else
#error "Please select field implementation"
#endif
///End of secp256k1/src/field_impl.h


///From secp256k1/src/scalar_impl.h
/**********************************************************************
 * Copyright (c) 2014 Pieter Wuille                                   *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#if defined(EXHAUSTIVE_TEST_ORDER)
#include "scalar_low_impl.h"
#elif defined(USE_SCALAR_4X64)
///From secp256k1/src/scalar_4x64_impl.h
/**********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                             *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/
/* Limbs of the secp256k1 order. */
#define SECP256K1_N_0 ((uint64_t)0xBFD25E8CD0364141ULL)
#define SECP256K1_N_1 ((uint64_t)0xBAAEDCE6AF48A03BULL)
#define SECP256K1_N_2 ((uint64_t)0xFFFFFFFFFFFFFFFEULL)
#define SECP256K1_N_3 ((uint64_t)0xFFFFFFFFFFFFFFFFULL)

/* Limbs of 2^256 minus the secp256k1 order. */
#define SECP256K1_N_C_0 (~SECP256K1_N_0 + 1)
#define SECP256K1_N_C_1 (~SECP256K1_N_1)
#define SECP256K1_N_C_2 (1)

/* Limbs of half the secp256k1 order. */
#define SECP256K1_N_H_0 ((uint64_t)0xDFE92F46681B20A0ULL)
#define SECP256K1_N_H_1 ((uint64_t)0x5D576E7357A4501DULL)
#define SECP256K1_N_H_2 ((uint64_t)0xFFFFFFFFFFFFFFFFULL)
#define SECP256K1_N_H_3 ((uint64_t)0x7FFFFFFFFFFFFFFFULL)

void secp256k1_scalar_clear(secp256k1_scalar *r) {
    r->d[0] = 0;
    r->d[1] = 0;
    r->d[2] = 0;
    r->d[3] = 0;
}

void secp256k1_scalar_set_int(secp256k1_scalar *r, unsigned int v) {
    r->d[0] = v;
    r->d[1] = 0;
    r->d[2] = 0;
    r->d[3] = 0;
}

unsigned int secp256k1_scalar_get_bits(const secp256k1_scalar *a, unsigned int offset, unsigned int count) {
    VERIFY_CHECK((offset + count - 1) >> 6 == offset >> 6);
    return (a->d[offset >> 6] >> (offset & 0x3F)) & ((((uint64_t)1) << count) - 1);
}

unsigned int secp256k1_scalar_get_bits_var(const secp256k1_scalar *a, unsigned int offset, unsigned int count) {
    VERIFY_CHECK(count < 32);
    VERIFY_CHECK(offset + count <= 256);
    if ((offset + count - 1) >> 6 == offset >> 6) {
        return secp256k1_scalar_get_bits(a, offset, count);
    } else {
        VERIFY_CHECK((offset >> 6) + 1 < 4);
        return ((a->d[offset >> 6] >> (offset & 0x3F)) | (a->d[(offset >> 6) + 1] << (64 - (offset & 0x3F)))) & ((((uint64_t)1) << count) - 1);
    }
}

SECP256K1_INLINE int secp256k1_scalar_check_overflow(const secp256k1_scalar *a) {
    int yes = 0;
    int no = 0;
    no |= (a->d[3] < SECP256K1_N_3); /* No need for a > check. */
    no |= (a->d[2] < SECP256K1_N_2);
    yes |= (a->d[2] > SECP256K1_N_2) & ~no;
    no |= (a->d[1] < SECP256K1_N_1);
    yes |= (a->d[1] > SECP256K1_N_1) & ~no;
    yes |= (a->d[0] >= SECP256K1_N_0) & ~no;
    return yes;
}

SECP256K1_INLINE int secp256k1_scalar_reduce(secp256k1_scalar *r, unsigned int overflow) {
    uint128_t t;
    VERIFY_CHECK(overflow <= 1);
    t = (uint128_t)r->d[0] + overflow * SECP256K1_N_C_0;
    r->d[0] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)r->d[1] + overflow * SECP256K1_N_C_1;
    r->d[1] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)r->d[2] + overflow * SECP256K1_N_C_2;
    r->d[2] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint64_t)r->d[3];
    r->d[3] = t & 0xFFFFFFFFFFFFFFFFULL;
    return overflow;
}

//safe when input and output are the same
int secp256k1_scalar_add(secp256k1_scalar *r, const secp256k1_scalar *a, const secp256k1_scalar *b) {
    int overflow;
    uint128_t t = (uint128_t)a->d[0] + b->d[0];
    r->d[0] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)a->d[1] + b->d[1];
    r->d[1] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)a->d[2] + b->d[2];
    r->d[2] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)a->d[3] + b->d[3];
    r->d[3] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    overflow = t + secp256k1_scalar_check_overflow(r);
    VERIFY_CHECK(overflow == 0 || overflow == 1);
    secp256k1_scalar_reduce(r, overflow);
    return overflow;
}

void secp256k1_scalar_cadd_bit(secp256k1_scalar *r, unsigned int bit, int flag) {
    uint128_t t;
    VERIFY_CHECK(bit < 256);
    bit += ((uint32_t) flag - 1) & 0x100;  /* forcing (bit >> 6) > 3 makes this a noop */
    t = (uint128_t)r->d[0] + (((uint64_t)((bit >> 6) == 0)) << (bit & 0x3F));
    r->d[0] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)r->d[1] + (((uint64_t)((bit >> 6) == 1)) << (bit & 0x3F));
    r->d[1] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)r->d[2] + (((uint64_t)((bit >> 6) == 2)) << (bit & 0x3F));
    r->d[2] = t & 0xFFFFFFFFFFFFFFFFULL; t >>= 64;
    t += (uint128_t)r->d[3] + (((uint64_t)((bit >> 6) == 3)) << (bit & 0x3F));
    r->d[3] = t & 0xFFFFFFFFFFFFFFFFULL;
#ifdef VERIFY
    VERIFY_CHECK((t >> 64) == 0);
    VERIFY_CHECK(secp256k1_scalar_check_overflow(r) == 0);
#endif
}

void secp256k1_scalar_set_b32(secp256k1_scalar *r, const unsigned char *b32, int *overflow) {
    int over;
    r->d[0] = (uint64_t)b32[31] | (uint64_t)b32[30] << 8 | (uint64_t)b32[29] << 16 | (uint64_t)b32[28] << 24 | (uint64_t)b32[27] << 32 | (uint64_t)b32[26] << 40 | (uint64_t)b32[25] << 48 | (uint64_t)b32[24] << 56;
    r->d[1] = (uint64_t)b32[23] | (uint64_t)b32[22] << 8 | (uint64_t)b32[21] << 16 | (uint64_t)b32[20] << 24 | (uint64_t)b32[19] << 32 | (uint64_t)b32[18] << 40 | (uint64_t)b32[17] << 48 | (uint64_t)b32[16] << 56;
    r->d[2] = (uint64_t)b32[15] | (uint64_t)b32[14] << 8 | (uint64_t)b32[13] << 16 | (uint64_t)b32[12] << 24 | (uint64_t)b32[11] << 32 | (uint64_t)b32[10] << 40 | (uint64_t)b32[9] << 48 | (uint64_t)b32[8] << 56;
    r->d[3] = (uint64_t)b32[7] | (uint64_t)b32[6] << 8 | (uint64_t)b32[5] << 16 | (uint64_t)b32[4] << 24 | (uint64_t)b32[3] << 32 | (uint64_t)b32[2] << 40 | (uint64_t)b32[1] << 48 | (uint64_t)b32[0] << 56;
    over = secp256k1_scalar_reduce(r, secp256k1_scalar_check_overflow(r));
    if (overflow) {
        *overflow = over;
    }
}

void secp256k1_scalar_get_b32(unsigned char *bin, const secp256k1_scalar* a) {
    bin[0] = a->d[3] >> 56; bin[1] = a->d[3] >> 48; bin[2] = a->d[3] >> 40; bin[3] = a->d[3] >> 32; bin[4] = a->d[3] >> 24; bin[5] = a->d[3] >> 16; bin[6] = a->d[3] >> 8; bin[7] = a->d[3];
    bin[8] = a->d[2] >> 56; bin[9] = a->d[2] >> 48; bin[10] = a->d[2] >> 40; bin[11] = a->d[2] >> 32; bin[12] = a->d[2] >> 24; bin[13] = a->d[2] >> 16; bin[14] = a->d[2] >> 8; bin[15] = a->d[2];
    bin[16] = a->d[1] >> 56; bin[17] = a->d[1] >> 48; bin[18] = a->d[1] >> 40; bin[19] = a->d[1] >> 32; bin[20] = a->d[1] >> 24; bin[21] = a->d[1] >> 16; bin[22] = a->d[1] >> 8; bin[23] = a->d[1];
    bin[24] = a->d[0] >> 56; bin[25] = a->d[0] >> 48; bin[26] = a->d[0] >> 40; bin[27] = a->d[0] >> 32; bin[28] = a->d[0] >> 24; bin[29] = a->d[0] >> 16; bin[30] = a->d[0] >> 8; bin[31] = a->d[0];
}

int secp256k1_scalar_is_zero(const secp256k1_scalar *a) {
    return (a->d[0] | a->d[1] | a->d[2] | a->d[3]) == 0;
}

void secp256k1_scalar_negate(secp256k1_scalar *r, const secp256k1_scalar *a) {
    uint64_t nonzero = 0xFFFFFFFFFFFFFFFFULL * (secp256k1_scalar_is_zero(a) == 0);
    uint128_t t = (uint128_t)(~a->d[0]) + SECP256K1_N_0 + 1;
    r->d[0] = t & nonzero; t >>= 64;
    t += (uint128_t)(~a->d[1]) + SECP256K1_N_1;
    r->d[1] = t & nonzero; t >>= 64;
    t += (uint128_t)(~a->d[2]) + SECP256K1_N_2;
    r->d[2] = t & nonzero; t >>= 64;
    t += (uint128_t)(~a->d[3]) + SECP256K1_N_3;
    r->d[3] = t & nonzero;
}

int secp256k1_scalar_is_one(const secp256k1_scalar *a) {
    return ((a->d[0] ^ 1) | a->d[1] | a->d[2] | a->d[3]) == 0;
}

int secp256k1_scalar_is_high(const secp256k1_scalar *a) {
    int yes = 0;
    int no = 0;
    no |= (a->d[3] < SECP256K1_N_H_3);
    yes |= (a->d[3] > SECP256K1_N_H_3) & ~no;
    no |= (a->d[2] < SECP256K1_N_H_2) & ~yes; /* No need for a > check. */
    no |= (a->d[1] < SECP256K1_N_H_1) & ~yes;
    yes |= (a->d[1] > SECP256K1_N_H_1) & ~no;
    yes |= (a->d[0] > SECP256K1_N_H_0) & ~no;
    return yes;
}

int secp256k1_scalar_cond_negate(secp256k1_scalar *r, int flag) {
    /* If we are flag = 0, mask = 00...00 and this is a no-op;
     * if we are flag = 1, mask = 11...11 and this is identical to secp256k1_scalar_negate */
    uint64_t mask = !flag - 1;
    uint64_t nonzero = (secp256k1_scalar_is_zero(r) != 0) - 1;
    uint128_t t = (uint128_t)(r->d[0] ^ mask) + ((SECP256K1_N_0 + 1) & mask);
    r->d[0] = t & nonzero; t >>= 64;
    t += (uint128_t)(r->d[1] ^ mask) + (SECP256K1_N_1 & mask);
    r->d[1] = t & nonzero; t >>= 64;
    t += (uint128_t)(r->d[2] ^ mask) + (SECP256K1_N_2 & mask);
    r->d[2] = t & nonzero; t >>= 64;
    t += (uint128_t)(r->d[3] ^ mask) + (SECP256K1_N_3 & mask);
    r->d[3] = t & nonzero;
    return 2 * (mask == 0) - 1;
}

/* Inspired by the macros in OpenSSL's crypto/bn/asm/x86_64-gcc.c. */

/** Add a*b to the number defined by (c0,c1,c2). c2 must never overflow. */
#define muladd(a,b) { \
    uint64_t tl, th; \
    { \
        uint128_t t = (uint128_t)a * b; \
        th = t >> 64;         /* at most 0xFFFFFFFFFFFFFFFE */ \
        tl = t; \
    } \
    c0 += tl;                 /* overflow is handled on the next line */ \
    th += (c0 < tl) ? 1 : 0;  /* at most 0xFFFFFFFFFFFFFFFF */ \
    c1 += th;                 /* overflow is handled on the next line */ \
    c2 += (c1 < th) ? 1 : 0;  /* never overflows by contract (verified in the next line) */ \
    VERIFY_CHECK((c1 >= th) || (c2 != 0)); \
}

/** Add a*b to the number defined by (c0,c1). c1 must never overflow. */
#define muladd_fast(a,b) { \
    uint64_t tl, th; \
    { \
        uint128_t t = (uint128_t)a * b; \
        th = t >> 64;         /* at most 0xFFFFFFFFFFFFFFFE */ \
        tl = t; \
    } \
    c0 += tl;                 /* overflow is handled on the next line */ \
    th += (c0 < tl) ? 1 : 0;  /* at most 0xFFFFFFFFFFFFFFFF */ \
    c1 += th;                 /* never overflows by contract (verified in the next line) */ \
    VERIFY_CHECK(c1 >= th); \
}

/** Add 2*a*b to the number defined by (c0,c1,c2). c2 must never overflow. */
#define muladd2(a,b) { \
    uint64_t tl, th, th2, tl2; \
    { \
        uint128_t t = (uint128_t)a * b; \
        th = t >> 64;               /* at most 0xFFFFFFFFFFFFFFFE */ \
        tl = t; \
    } \
    th2 = th + th;                  /* at most 0xFFFFFFFFFFFFFFFE (in case th was 0x7FFFFFFFFFFFFFFF) */ \
    c2 += (th2 < th) ? 1 : 0;       /* never overflows by contract (verified the next line) */ \
    VERIFY_CHECK((th2 >= th) || (c2 != 0)); \
    tl2 = tl + tl;                  /* at most 0xFFFFFFFFFFFFFFFE (in case the lowest 63 bits of tl were 0x7FFFFFFFFFFFFFFF) */ \
    th2 += (tl2 < tl) ? 1 : 0;      /* at most 0xFFFFFFFFFFFFFFFF */ \
    c0 += tl2;                      /* overflow is handled on the next line */ \
    th2 += (c0 < tl2) ? 1 : 0;      /* second overflow is handled on the next line */ \
    c2 += (c0 < tl2) & (th2 == 0);  /* never overflows by contract (verified the next line) */ \
    VERIFY_CHECK((c0 >= tl2) || (th2 != 0) || (c2 != 0)); \
    c1 += th2;                      /* overflow is handled on the next line */ \
    c2 += (c1 < th2) ? 1 : 0;       /* never overflows by contract (verified the next line) */ \
    VERIFY_CHECK((c1 >= th2) || (c2 != 0)); \
}

/** Add a to the number defined by (c0,c1,c2). c2 must never overflow. */
#define sumadd(a) { \
    unsigned int over; \
    c0 += (a);                  /* overflow is handled on the next line */ \
    over = (c0 < (a)) ? 1 : 0; \
    c1 += over;                 /* overflow is handled on the next line */ \
    c2 += (c1 < over) ? 1 : 0;  /* never overflows by contract */ \
}

/** Add a to the number defined by (c0,c1). c1 must never overflow, c2 must be zero. */
#define sumadd_fast(a) { \
    c0 += (a);                 /* overflow is handled on the next line */ \
    c1 += (c0 < (a)) ? 1 : 0;  /* never overflows by contract (verified the next line) */ \
    VERIFY_CHECK((c1 != 0) | (c0 >= (a))); \
    VERIFY_CHECK(c2 == 0); \
}

/** Extract the lowest 64 bits of (c0,c1,c2) into n, and left shift the number 64 bits. */
#define extract(n) { \
    (n) = c0; \
    c0 = c1; \
    c1 = c2; \
    c2 = 0; \
}

/** Extract the lowest 64 bits of (c0,c1,c2) into n, and left shift the number 64 bits. c2 is required to be zero. */
#define extract_fast(n) { \
    (n) = c0; \
    c0 = c1; \
    c1 = 0; \
    VERIFY_CHECK(c2 == 0); \
}

void secp256k1_scalar_reduce_512(secp256k1_scalar *r, const uint64_t *l) {
#ifdef USE_ASM_X86_64
    /* Reduce 512 bits into 385. */
    uint64_t m0, m1, m2, m3, m4, m5, m6;
    uint64_t p0, p1, p2, p3, p4;
    uint64_t c;

    __asm__ __volatile__(
    /* Preload. */
    "movq 32(%%rsi), %%r11\n"
    "movq 40(%%rsi), %%r12\n"
    "movq 48(%%rsi), %%r13\n"
    "movq 56(%%rsi), %%r14\n"
    /* Initialize r8,r9,r10 */
    "movq 0(%%rsi), %%r8\n"
    "xorq %%r9, %%r9\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9) += n0 * c0 */
    "movq %8, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    /* extract m0 */
    "movq %%r8, %q0\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10) += l1 */
    "addq 8(%%rsi), %%r9\n"
    "adcq $0, %%r10\n"
    /* (r9,r10,r8) += n1 * c0 */
    "movq %8, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += n0 * c1 */
    "movq %9, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* extract m1 */
    "movq %%r9, %q1\n"
    "xorq %%r9, %%r9\n"
    /* (r10,r8,r9) += l2 */
    "addq 16(%%rsi), %%r10\n"
    "adcq $0, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += n2 * c0 */
    "movq %8, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += n1 * c1 */
    "movq %9, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += n0 */
    "addq %%r11, %%r10\n"
    "adcq $0, %%r8\n"
    "adcq $0, %%r9\n"
    /* extract m2 */
    "movq %%r10, %q2\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9,r10) += l3 */
    "addq 24(%%rsi), %%r8\n"
    "adcq $0, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += n3 * c0 */
    "movq %8, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += n2 * c1 */
    "movq %9, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += n1 */
    "addq %%r12, %%r8\n"
    "adcq $0, %%r9\n"
    "adcq $0, %%r10\n"
    /* extract m3 */
    "movq %%r8, %q3\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10,r8) += n3 * c1 */
    "movq %9, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += n2 */
    "addq %%r13, %%r9\n"
    "adcq $0, %%r10\n"
    "adcq $0, %%r8\n"
    /* extract m4 */
    "movq %%r9, %q4\n"
    /* (r10,r8) += n3 */
    "addq %%r14, %%r10\n"
    "adcq $0, %%r8\n"
    /* extract m5 */
    "movq %%r10, %q5\n"
    /* extract m6 */
    "movq %%r8, %q6\n"
    : "=g"(m0), "=g"(m1), "=g"(m2), "=g"(m3), "=g"(m4), "=g"(m5), "=g"(m6)
    : "S"(l), "n"(SECP256K1_N_C_0), "n"(SECP256K1_N_C_1)
    : "rax", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "cc");

    /* Reduce 385 bits into 258. */
    __asm__ __volatile__(
    /* Preload */
    "movq %q9, %%r11\n"
    "movq %q10, %%r12\n"
    "movq %q11, %%r13\n"
    /* Initialize (r8,r9,r10) */
    "movq %q5, %%r8\n"
    "xorq %%r9, %%r9\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9) += m4 * c0 */
    "movq %12, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    /* extract p0 */
    "movq %%r8, %q0\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10) += m1 */
    "addq %q6, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r9,r10,r8) += m5 * c0 */
    "movq %12, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += m4 * c1 */
    "movq %13, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* extract p1 */
    "movq %%r9, %q1\n"
    "xorq %%r9, %%r9\n"
    /* (r10,r8,r9) += m2 */
    "addq %q7, %%r10\n"
    "adcq $0, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += m6 * c0 */
    "movq %12, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += m5 * c1 */
    "movq %13, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += m4 */
    "addq %%r11, %%r10\n"
    "adcq $0, %%r8\n"
    "adcq $0, %%r9\n"
    /* extract p2 */
    "movq %%r10, %q2\n"
    /* (r8,r9) += m3 */
    "addq %q8, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r8,r9) += m6 * c1 */
    "movq %13, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    /* (r8,r9) += m5 */
    "addq %%r12, %%r8\n"
    "adcq $0, %%r9\n"
    /* extract p3 */
    "movq %%r8, %q3\n"
    /* (r9) += m6 */
    "addq %%r13, %%r9\n"
    /* extract p4 */
    "movq %%r9, %q4\n"
    : "=&g"(p0), "=&g"(p1), "=&g"(p2), "=g"(p3), "=g"(p4)
    : "g"(m0), "g"(m1), "g"(m2), "g"(m3), "g"(m4), "g"(m5), "g"(m6), "n"(SECP256K1_N_C_0), "n"(SECP256K1_N_C_1)
    : "rax", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "cc");

    /* Reduce 258 bits into 256. */
    __asm__ __volatile__(
    /* Preload */
    "movq %q5, %%r10\n"
    /* (rax,rdx) = p4 * c0 */
    "movq %7, %%rax\n"
    "mulq %%r10\n"
    /* (rax,rdx) += p0 */
    "addq %q1, %%rax\n"
    "adcq $0, %%rdx\n"
    /* extract r0 */
    "movq %%rax, 0(%q6)\n"
    /* Move to (r8,r9) */
    "movq %%rdx, %%r8\n"
    "xorq %%r9, %%r9\n"
    /* (r8,r9) += p1 */
    "addq %q2, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r8,r9) += p4 * c1 */
    "movq %8, %%rax\n"
    "mulq %%r10\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    /* Extract r1 */
    "movq %%r8, 8(%q6)\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r8) += p4 */
    "addq %%r10, %%r9\n"
    "adcq $0, %%r8\n"
    /* (r9,r8) += p2 */
    "addq %q3, %%r9\n"
    "adcq $0, %%r8\n"
    /* Extract r2 */
    "movq %%r9, 16(%q6)\n"
    "xorq %%r9, %%r9\n"
    /* (r8,r9) += p3 */
    "addq %q4, %%r8\n"
    "adcq $0, %%r9\n"
    /* Extract r3 */
    "movq %%r8, 24(%q6)\n"
    /* Extract c */
    "movq %%r9, %q0\n"
    : "=g"(c)
    : "g"(p0), "g"(p1), "g"(p2), "g"(p3), "g"(p4), "D"(r), "n"(SECP256K1_N_C_0), "n"(SECP256K1_N_C_1)
    : "rax", "rdx", "r8", "r9", "r10", "cc", "memory");
#else
    uint128_t c;
    uint64_t c0, c1, c2;
    uint64_t n0 = l[4], n1 = l[5], n2 = l[6], n3 = l[7];
    uint64_t m0, m1, m2, m3, m4, m5;
    uint32_t m6;
    uint64_t p0, p1, p2, p3;
    uint32_t p4;

    /* Reduce 512 bits into 385. */
    /* m[0..6] = l[0..3] + n[0..3] * SECP256K1_N_C. */
    c0 = l[0]; c1 = 0; c2 = 0;
    muladd_fast(n0, SECP256K1_N_C_0);
    extract_fast(m0);
    sumadd_fast(l[1]);
    muladd(n1, SECP256K1_N_C_0);
    muladd(n0, SECP256K1_N_C_1);
    extract(m1);
    sumadd(l[2]);
    muladd(n2, SECP256K1_N_C_0);
    muladd(n1, SECP256K1_N_C_1);
    sumadd(n0);
    extract(m2);
    sumadd(l[3]);
    muladd(n3, SECP256K1_N_C_0);
    muladd(n2, SECP256K1_N_C_1);
    sumadd(n1);
    extract(m3);
    muladd(n3, SECP256K1_N_C_1);
    sumadd(n2);
    extract(m4);
    sumadd_fast(n3);
    extract_fast(m5);
    VERIFY_CHECK(c0 <= 1);
    m6 = c0;

    /* Reduce 385 bits into 258. */
    /* p[0..4] = m[0..3] + m[4..6] * SECP256K1_N_C. */
    c0 = m0; c1 = 0; c2 = 0;
    muladd_fast(m4, SECP256K1_N_C_0);
    extract_fast(p0);
    sumadd_fast(m1);
    muladd(m5, SECP256K1_N_C_0);
    muladd(m4, SECP256K1_N_C_1);
    extract(p1);
    sumadd(m2);
    muladd(m6, SECP256K1_N_C_0);
    muladd(m5, SECP256K1_N_C_1);
    sumadd(m4);
    extract(p2);
    sumadd_fast(m3);
    muladd_fast(m6, SECP256K1_N_C_1);
    sumadd_fast(m5);
    extract_fast(p3);
    p4 = c0 + m6;
    VERIFY_CHECK(p4 <= 2);

    /* Reduce 258 bits into 256. */
    /* r[0..3] = p[0..3] + p[4] * SECP256K1_N_C. */
    c = p0 + (uint128_t)SECP256K1_N_C_0 * p4;
    r->d[0] = c & 0xFFFFFFFFFFFFFFFFULL; c >>= 64;
    c += p1 + (uint128_t)SECP256K1_N_C_1 * p4;
    r->d[1] = c & 0xFFFFFFFFFFFFFFFFULL; c >>= 64;
    c += p2 + (uint128_t)p4;
    r->d[2] = c & 0xFFFFFFFFFFFFFFFFULL; c >>= 64;
    c += p3;
    r->d[3] = c & 0xFFFFFFFFFFFFFFFFULL; c >>= 64;
#endif

    /* Final reduction of r. */
    secp256k1_scalar_reduce(r, c + secp256k1_scalar_check_overflow(r));
}

void secp256k1_scalar_mul_512(uint64_t l[8], const secp256k1_scalar *a, const secp256k1_scalar *b) {
#ifdef USE_ASM_X86_64
    const uint64_t *pb = b->d;
    __asm__ __volatile__(
    /* Preload */
    "movq 0(%%rdi), %%r15\n"
    "movq 8(%%rdi), %%rbx\n"
    "movq 16(%%rdi), %%rcx\n"
    "movq 0(%%rdx), %%r11\n"
    "movq 8(%%rdx), %%r12\n"
    "movq 16(%%rdx), %%r13\n"
    "movq 24(%%rdx), %%r14\n"
    /* (rax,rdx) = a0 * b0 */
    "movq %%r15, %%rax\n"
    "mulq %%r11\n"
    /* Extract l0 */
    "movq %%rax, 0(%%rsi)\n"
    /* (r8,r9,r10) = (rdx) */
    "movq %%rdx, %%r8\n"
    "xorq %%r9, %%r9\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9,r10) += a0 * b1 */
    "movq %%r15, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += a1 * b0 */
    "movq %%rbx, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* Extract l1 */
    "movq %%r8, 8(%%rsi)\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10,r8) += a0 * b2 */
    "movq %%r15, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += a1 * b1 */
    "movq %%rbx, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += a2 * b0 */
    "movq %%rcx, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* Extract l2 */
    "movq %%r9, 16(%%rsi)\n"
    "xorq %%r9, %%r9\n"
    /* (r10,r8,r9) += a0 * b3 */
    "movq %%r15, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* Preload a3 */
    "movq 24(%%rdi), %%r15\n"
    /* (r10,r8,r9) += a1 * b2 */
    "movq %%rbx, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += a2 * b1 */
    "movq %%rcx, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += a3 * b0 */
    "movq %%r15, %%rax\n"
    "mulq %%r11\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* Extract l3 */
    "movq %%r10, 24(%%rsi)\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9,r10) += a1 * b3 */
    "movq %%rbx, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += a2 * b2 */
    "movq %%rcx, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += a3 * b1 */
    "movq %%r15, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* Extract l4 */
    "movq %%r8, 32(%%rsi)\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10,r8) += a2 * b3 */
    "movq %%rcx, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += a3 * b2 */
    "movq %%r15, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* Extract l5 */
    "movq %%r9, 40(%%rsi)\n"
    /* (r10,r8) += a3 * b3 */
    "movq %%r15, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    /* Extract l6 */
    "movq %%r10, 48(%%rsi)\n"
    /* Extract l7 */
    "movq %%r8, 56(%%rsi)\n"
    : "+d"(pb)
    : "S"(l), "D"(a->d)
    : "rax", "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "cc", "memory");
#else
    /* 160 bit accumulator. */
    uint64_t c0 = 0, c1 = 0;
    uint32_t c2 = 0;

    /* l[0..7] = a[0..3] * b[0..3]. */
    muladd_fast(a->d[0], b->d[0]);
    extract_fast(l[0]);
    muladd(a->d[0], b->d[1]);
    muladd(a->d[1], b->d[0]);
    extract(l[1]);
    muladd(a->d[0], b->d[2]);
    muladd(a->d[1], b->d[1]);
    muladd(a->d[2], b->d[0]);
    extract(l[2]);
    muladd(a->d[0], b->d[3]);
    muladd(a->d[1], b->d[2]);
    muladd(a->d[2], b->d[1]);
    muladd(a->d[3], b->d[0]);
    extract(l[3]);
    muladd(a->d[1], b->d[3]);
    muladd(a->d[2], b->d[2]);
    muladd(a->d[3], b->d[1]);
    extract(l[4]);
    muladd(a->d[2], b->d[3]);
    muladd(a->d[3], b->d[2]);
    extract(l[5]);
    muladd_fast(a->d[3], b->d[3]);
    extract_fast(l[6]);
    VERIFY_CHECK(c1 == 0);
    l[7] = c0;
#endif
}

void secp256k1_scalar_sqr_512(uint64_t l[8], const secp256k1_scalar *a) {
#ifdef USE_ASM_X86_64
    __asm__ __volatile__(
    /* Preload */
    "movq 0(%%rdi), %%r11\n"
    "movq 8(%%rdi), %%r12\n"
    "movq 16(%%rdi), %%r13\n"
    "movq 24(%%rdi), %%r14\n"
    /* (rax,rdx) = a0 * a0 */
    "movq %%r11, %%rax\n"
    "mulq %%r11\n"
    /* Extract l0 */
    "movq %%rax, 0(%%rsi)\n"
    /* (r8,r9,r10) = (rdx,0) */
    "movq %%rdx, %%r8\n"
    "xorq %%r9, %%r9\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9,r10) += 2 * a0 * a1 */
    "movq %%r11, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* Extract l1 */
    "movq %%r8, 8(%%rsi)\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10,r8) += 2 * a0 * a2 */
    "movq %%r11, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* (r9,r10,r8) += a1 * a1 */
    "movq %%r12, %%rax\n"
    "mulq %%r12\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* Extract l2 */
    "movq %%r9, 16(%%rsi)\n"
    "xorq %%r9, %%r9\n"
    /* (r10,r8,r9) += 2 * a0 * a3 */
    "movq %%r11, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* (r10,r8,r9) += 2 * a1 * a2 */
    "movq %%r12, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    "adcq $0, %%r9\n"
    /* Extract l3 */
    "movq %%r10, 24(%%rsi)\n"
    "xorq %%r10, %%r10\n"
    /* (r8,r9,r10) += 2 * a1 * a3 */
    "movq %%r12, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* (r8,r9,r10) += a2 * a2 */
    "movq %%r13, %%rax\n"
    "mulq %%r13\n"
    "addq %%rax, %%r8\n"
    "adcq %%rdx, %%r9\n"
    "adcq $0, %%r10\n"
    /* Extract l4 */
    "movq %%r8, 32(%%rsi)\n"
    "xorq %%r8, %%r8\n"
    /* (r9,r10,r8) += 2 * a2 * a3 */
    "movq %%r13, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    "addq %%rax, %%r9\n"
    "adcq %%rdx, %%r10\n"
    "adcq $0, %%r8\n"
    /* Extract l5 */
    "movq %%r9, 40(%%rsi)\n"
    /* (r10,r8) += a3 * a3 */
    "movq %%r14, %%rax\n"
    "mulq %%r14\n"
    "addq %%rax, %%r10\n"
    "adcq %%rdx, %%r8\n"
    /* Extract l6 */
    "movq %%r10, 48(%%rsi)\n"
    /* Extract l7 */
    "movq %%r8, 56(%%rsi)\n"
    :
    : "S"(l), "D"(a->d)
    : "rax", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "cc", "memory");
#else
    /* 160 bit accumulator. */
    uint64_t c0 = 0, c1 = 0;
    uint32_t c2 = 0;

    /* l[0..7] = a[0..3] * b[0..3]. */
    muladd_fast(a->d[0], a->d[0]);
    extract_fast(l[0]);
    muladd2(a->d[0], a->d[1]);
    extract(l[1]);
    muladd2(a->d[0], a->d[2]);
    muladd(a->d[1], a->d[1]);
    extract(l[2]);
    muladd2(a->d[0], a->d[3]);
    muladd2(a->d[1], a->d[2]);
    extract(l[3]);
    muladd2(a->d[1], a->d[3]);
    muladd(a->d[2], a->d[2]);
    extract(l[4]);
    muladd2(a->d[2], a->d[3]);
    extract(l[5]);
    muladd_fast(a->d[3], a->d[3]);
    extract_fast(l[6]);
    VERIFY_CHECK(c1 == 0);
    l[7] = c0;
#endif
}

#undef sumadd
#undef sumadd_fast
#undef muladd
#undef muladd_fast
#undef muladd2
#undef extract
#undef extract_fast

void secp256k1_scalar_mul(secp256k1_scalar *r, const secp256k1_scalar *a, const secp256k1_scalar *b) {
    uint64_t l[8];
    secp256k1_scalar_mul_512(l, a, b);
    secp256k1_scalar_reduce_512(r, l);
}

int secp256k1_scalar_shr_int(secp256k1_scalar *r, int n) {
    int ret;
    VERIFY_CHECK(n > 0);
    VERIFY_CHECK(n < 16);
    ret = r->d[0] & ((1 << n) - 1);
    r->d[0] = (r->d[0] >> n) + (r->d[1] << (64 - n));
    r->d[1] = (r->d[1] >> n) + (r->d[2] << (64 - n));
    r->d[2] = (r->d[2] >> n) + (r->d[3] << (64 - n));
    r->d[3] = (r->d[3] >> n);
    return ret;
}

void secp256k1_scalar_sqr(secp256k1_scalar *r, const secp256k1_scalar *a) {
    uint64_t l[8];
    secp256k1_scalar_sqr_512(l, a);
    secp256k1_scalar_reduce_512(r, l);
}

#ifdef USE_ENDOMORPHISM
void secp256k1_scalar_split_128(secp256k1_scalar *r1, secp256k1_scalar *r2, const secp256k1_scalar *a) {
    r1->d[0] = a->d[0];
    r1->d[1] = a->d[1];
    r1->d[2] = 0;
    r1->d[3] = 0;
    r2->d[0] = a->d[2];
    r2->d[1] = a->d[3];
    r2->d[2] = 0;
    r2->d[3] = 0;
}
#endif

int secp256k1_scalar_eq(const secp256k1_scalar *a, const secp256k1_scalar *b) {
    return ((a->d[0] ^ b->d[0]) | (a->d[1] ^ b->d[1]) | (a->d[2] ^ b->d[2]) | (a->d[3] ^ b->d[3])) == 0;
}

void secp256k1_scalar_mul_shift_var(secp256k1_scalar *r, const secp256k1_scalar *a, const secp256k1_scalar *b, unsigned int shift) {
    uint64_t l[8];
    unsigned int shiftlimbs;
    unsigned int shiftlow;
    unsigned int shifthigh;
    VERIFY_CHECK(shift >= 256);
    secp256k1_scalar_mul_512(l, a, b);
    shiftlimbs = shift >> 6;
    shiftlow = shift & 0x3F;
    shifthigh = 64 - shiftlow;
    r->d[0] = shift < 512 ? (l[0 + shiftlimbs] >> shiftlow | (shift < 448 && shiftlow ? (l[1 + shiftlimbs] << shifthigh) : 0)) : 0;
    r->d[1] = shift < 448 ? (l[1 + shiftlimbs] >> shiftlow | (shift < 384 && shiftlow ? (l[2 + shiftlimbs] << shifthigh) : 0)) : 0;
    r->d[2] = shift < 384 ? (l[2 + shiftlimbs] >> shiftlow | (shift < 320 && shiftlow ? (l[3 + shiftlimbs] << shifthigh) : 0)) : 0;
    r->d[3] = shift < 320 ? (l[3 + shiftlimbs] >> shiftlow) : 0;
    secp256k1_scalar_cadd_bit(r, 0, (l[(shift - 1) >> 6] >> ((shift - 1) & 0x3f)) & 1);
}
///end of secp256k1/src/scalar_4x64_impl.h
#elif defined(USE_SCALAR_8X32)
#include "scalar_8x32_impl.h"
#else
#error "Please select scalar implementation"
#endif

#ifndef USE_NUM_NONE
void secp256k1_scalar_get_num(secp256k1_num *r, const secp256k1_scalar *a) {
    unsigned char c[32];
    secp256k1_scalar_get_b32(c, a);
    secp256k1_num_set_bin(r, c, 32);
}

/** secp256k1 curve order, see secp256k1_ecdsa_const_order_as_fe in ecdsa_impl.h */
void secp256k1_scalar_order_get_num(secp256k1_num *r) {
#if defined(EXHAUSTIVE_TEST_ORDER)
    static const unsigned char order[32] = {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,EXHAUSTIVE_TEST_ORDER
    };
#else
    static const unsigned char order[32] = {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
        0xBA,0xAE,0xDC,0xE6,0xAF,0x48,0xA0,0x3B,
        0xBF,0xD2,0x5E,0x8C,0xD0,0x36,0x41,0x41
    };
#endif
    secp256k1_num_set_bin(r, order, 32);
}
#endif

void secp256k1_scalar_inverse(secp256k1_scalar *r, const secp256k1_scalar *x) {
#if defined(EXHAUSTIVE_TEST_ORDER)
    int i;
    *r = 0;
    for (i = 0; i < EXHAUSTIVE_TEST_ORDER; i++)
        if ((i * *x) % EXHAUSTIVE_TEST_ORDER == 1)
            *r = i;
    /* If this VERIFY_CHECK triggers we were given a noninvertible scalar (and thus
     * have a composite group order; fix it in exhaustive_tests.c). */
    VERIFY_CHECK(*r != 0);
}
#else
    secp256k1_scalar *t;
    int i;
    /* First compute xN as x ^ (2^N - 1) for some values of N,
     * and uM as x ^ M for some values of M. */
    secp256k1_scalar x2, x3, x6, x8, x14, x28, x56, x112, x126;
    secp256k1_scalar u2, u5, u9, u11, u13;

    secp256k1_scalar_sqr(&u2, x);
    secp256k1_scalar_mul(&x2, &u2,  x);
    secp256k1_scalar_mul(&u5, &u2, &x2);
    secp256k1_scalar_mul(&x3, &u5,  &u2);
    secp256k1_scalar_mul(&u9, &x3, &u2);
    secp256k1_scalar_mul(&u11, &u9, &u2);
    secp256k1_scalar_mul(&u13, &u11, &u2);

    secp256k1_scalar_sqr(&x6, &u13);
    secp256k1_scalar_sqr(&x6, &x6);
    secp256k1_scalar_mul(&x6, &x6, &u11);

    secp256k1_scalar_sqr(&x8, &x6);
    secp256k1_scalar_sqr(&x8, &x8);
    secp256k1_scalar_mul(&x8, &x8,  &x2);

    secp256k1_scalar_sqr(&x14, &x8);
    for (i = 0; i < 5; i++) {
        secp256k1_scalar_sqr(&x14, &x14);
    }
    secp256k1_scalar_mul(&x14, &x14, &x6);

    secp256k1_scalar_sqr(&x28, &x14);
    for (i = 0; i < 13; i++) {
        secp256k1_scalar_sqr(&x28, &x28);
    }
    secp256k1_scalar_mul(&x28, &x28, &x14);

    secp256k1_scalar_sqr(&x56, &x28);
    for (i = 0; i < 27; i++) {
        secp256k1_scalar_sqr(&x56, &x56);
    }
    secp256k1_scalar_mul(&x56, &x56, &x28);

    secp256k1_scalar_sqr(&x112, &x56);
    for (i = 0; i < 55; i++) {
        secp256k1_scalar_sqr(&x112, &x112);
    }
    secp256k1_scalar_mul(&x112, &x112, &x56);

    secp256k1_scalar_sqr(&x126, &x112);
    for (i = 0; i < 13; i++) {
        secp256k1_scalar_sqr(&x126, &x126);
    }
    secp256k1_scalar_mul(&x126, &x126, &x14);

    /* Then accumulate the final result (t starts at x126). */
    t = &x126;
    for (i = 0; i < 3; i++) {
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u5); /* 101 */
    for (i = 0; i < 4; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x3); /* 111 */
    for (i = 0; i < 4; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u5); /* 101 */
    for (i = 0; i < 5; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u11); /* 1011 */
    for (i = 0; i < 4; i++) {
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u11); /* 1011 */
    for (i = 0; i < 4; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x3); /* 111 */
    for (i = 0; i < 5; i++) { /* 00 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x3); /* 111 */
    for (i = 0; i < 6; i++) { /* 00 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u13); /* 1101 */
    for (i = 0; i < 4; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u5); /* 101 */
    for (i = 0; i < 3; i++) {
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x3); /* 111 */
    for (i = 0; i < 5; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u9); /* 1001 */
    for (i = 0; i < 6; i++) { /* 000 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u5); /* 101 */
    for (i = 0; i < 10; i++) { /* 0000000 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x3); /* 111 */
    for (i = 0; i < 4; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x3); /* 111 */
    for (i = 0; i < 9; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x8); /* 11111111 */
    for (i = 0; i < 5; i++) { /* 0 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u9); /* 1001 */
    for (i = 0; i < 6; i++) { /* 00 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u11); /* 1011 */
    for (i = 0; i < 4; i++) {
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u13); /* 1101 */
    for (i = 0; i < 5; i++) {
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &x2); /* 11 */
    for (i = 0; i < 6; i++) { /* 00 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u13); /* 1101 */
    for (i = 0; i < 10; i++) { /* 000000 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u13); /* 1101 */
    for (i = 0; i < 4; i++) {
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, &u9); /* 1001 */
    for (i = 0; i < 6; i++) { /* 00000 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(t, t, x); /* 1 */
    for (i = 0; i < 8; i++) { /* 00 */
        secp256k1_scalar_sqr(t, t);
    }
    secp256k1_scalar_mul(r, t, &x6); /* 111111 */
}

int secp256k1_scalar_is_even(const secp256k1_scalar *a) {
    return !(a->d[0] & 1);
}
#endif

void secp256k1_scalar_inverse_var(secp256k1_scalar *r, const secp256k1_scalar *x) {
#if defined(USE_SCALAR_INV_BUILTIN)
    secp256k1_scalar_inverse(r, x);
#elif defined(USE_SCALAR_INV_NUM)
    unsigned char b[32];
    secp256k1_num n, m;
    secp256k1_scalar t = *x;
    secp256k1_scalar_get_b32(b, &t);
    secp256k1_num_set_bin(&n, b, 32);
    secp256k1_scalar_order_get_num(&m);
    secp256k1_num_mod_inverse(&n, &n, &m);
    secp256k1_num_get_bin(b, 32, &n);
    secp256k1_scalar_set_b32(r, b, NULL);
    /* Verify that the inverse was computed correctly, without GMP code. */
    secp256k1_scalar_mul(&t, &t, r);
    CHECK(secp256k1_scalar_is_one(&t));
#else
#error "Please select scalar inverse implementation"
#endif
}

#ifdef USE_ENDOMORPHISM
#if defined(EXHAUSTIVE_TEST_ORDER)
/**
 * Find k1 and k2 given k, such that k1 + k2 * lambda == k mod n; unlike in the
 * full case we don't bother making k1 and k2 be small, we just want them to be
 * nontrivial to get full test coverage for the exhaustive tests. We therefore
 * (arbitrarily) set k2 = k + 5 and k1 = k - k2 * lambda.
 */
void secp256k1_scalar_split_lambda(secp256k1_scalar *r1, secp256k1_scalar *r2, const secp256k1_scalar *a) {
    *r2 = (*a + 5) % EXHAUSTIVE_TEST_ORDER;
    *r1 = (*a + (EXHAUSTIVE_TEST_ORDER - *r2) * EXHAUSTIVE_TEST_LAMBDA) % EXHAUSTIVE_TEST_ORDER;
}
#else
/**
 * The Secp256k1 curve has an endomorphism, where lambda * (x, y) = (beta * x, y), where
 * lambda is {0x53,0x63,0xad,0x4c,0xc0,0x5c,0x30,0xe0,0xa5,0x26,0x1c,0x02,0x88,0x12,0x64,0x5a,
 *            0x12,0x2e,0x22,0xea,0x20,0x81,0x66,0x78,0xdf,0x02,0x96,0x7c,0x1b,0x23,0xbd,0x72}
 *
 * "Guide to Elliptic Curve Cryptography" (Hankerson, Menezes, Vanstone) gives an algorithm
 * (algorithm 3.74) to find k1 and k2 given k, such that k1 + k2 * lambda == k mod n, and k1
 * and k2 have a small size.
 * It relies on constants a1, b1, a2, b2. These constants for the value of lambda above are:
 *
 * - a1 =      {0x30,0x86,0xd2,0x21,0xa7,0xd4,0x6b,0xcd,0xe8,0x6c,0x90,0xe4,0x92,0x84,0xeb,0x15}
 * - b1 =     -{0xe4,0x43,0x7e,0xd6,0x01,0x0e,0x88,0x28,0x6f,0x54,0x7f,0xa9,0x0a,0xbf,0xe4,0xc3}
 * - a2 = {0x01,0x14,0xca,0x50,0xf7,0xa8,0xe2,0xf3,0xf6,0x57,0xc1,0x10,0x8d,0x9d,0x44,0xcf,0xd8}
 * - b2 =      {0x30,0x86,0xd2,0x21,0xa7,0xd4,0x6b,0xcd,0xe8,0x6c,0x90,0xe4,0x92,0x84,0xeb,0x15}
 *
 * The algorithm then computes c1 = round(b1 * k / n) and c2 = round(b2 * k / n), and gives
 * k1 = k - (c1*a1 + c2*a2) and k2 = -(c1*b1 + c2*b2). Instead, we use modular arithmetic, and
 * compute k1 as k - k2 * lambda, avoiding the need for constants a1 and a2.
 *
 * g1, g2 are precomputed constants used to replace division with a rounded multiplication
 * when decomposing the scalar for an endomorphism-based point multiplication.
 *
 * The possibility of using precomputed estimates is mentioned in "Guide to Elliptic Curve
 * Cryptography" (Hankerson, Menezes, Vanstone) in section 3.5.
 *
 * The derivation is described in the paper "Efficient Software Implementation of Public-Key
 * Cryptography on Sensor Networks Using the MSP430X Microcontroller" (Gouvea, Oliveira, Lopez),
 * Section 4.3 (here we use a somewhat higher-precision estimate):
 * d = a1*b2 - b1*a2
 * g1 = round((2^272)*b2/d)
 * g2 = round((2^272)*b1/d)
 *
 * (Note that 'd' is also equal to the curve order here because [a1,b1] and [a2,b2] are found
 * as outputs of the Extended Euclidean Algorithm on inputs 'order' and 'lambda').
 *
 * The function below splits a in r1 and r2, such that r1 + lambda * r2 == a (mod order).
 */

void secp256k1_scalar_split_lambda(secp256k1_scalar *r1, secp256k1_scalar *r2, const secp256k1_scalar *a) {
    secp256k1_scalar c1, c2;
    static const secp256k1_scalar minus_lambda = SECP256K1_SCALAR_CONST(
        0xAC9C52B3UL, 0x3FA3CF1FUL, 0x5AD9E3FDUL, 0x77ED9BA4UL,
        0xA880B9FCUL, 0x8EC739C2UL, 0xE0CFC810UL, 0xB51283CFUL
    );
    static const secp256k1_scalar minus_b1 = SECP256K1_SCALAR_CONST(
        0x00000000UL, 0x00000000UL, 0x00000000UL, 0x00000000UL,
        0xE4437ED6UL, 0x010E8828UL, 0x6F547FA9UL, 0x0ABFE4C3UL
    );
    static const secp256k1_scalar minus_b2 = SECP256K1_SCALAR_CONST(
        0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFEUL,
        0x8A280AC5UL, 0x0774346DUL, 0xD765CDA8UL, 0x3DB1562CUL
    );
    static const secp256k1_scalar g1 = SECP256K1_SCALAR_CONST(
        0x00000000UL, 0x00000000UL, 0x00000000UL, 0x00003086UL,
        0xD221A7D4UL, 0x6BCDE86CUL, 0x90E49284UL, 0xEB153DABUL
    );
    static const secp256k1_scalar g2 = SECP256K1_SCALAR_CONST(
        0x00000000UL, 0x00000000UL, 0x00000000UL, 0x0000E443UL,
        0x7ED6010EUL, 0x88286F54UL, 0x7FA90ABFUL, 0xE4C42212UL
    );
    VERIFY_CHECK(r1 != a);
    VERIFY_CHECK(r2 != a);
    /* these _var calls are constant time since the shift amount is constant */
    secp256k1_scalar_mul_shift_var(&c1, a, &g1, 272);
    secp256k1_scalar_mul_shift_var(&c2, a, &g2, 272);
    secp256k1_scalar_mul(&c1, &c1, &minus_b1);
    secp256k1_scalar_mul(&c2, &c2, &minus_b2);
    secp256k1_scalar_add(r2, &c1, &c2);
    secp256k1_scalar_mul(r1, r2, &minus_lambda);
    secp256k1_scalar_add(r1, r1, a);
}
#endif
#endif
///end of secp256k1/src/scalar_impl.h


///From secp256k1/src/group_impl.h
/* These points can be generated in sage as follows:
 *
 * 0. Setup a worksheet with the following parameters.
 *   b = 4  # whatever CURVE_B will be set to
 *   F = FiniteField (0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F)
 *   C = EllipticCurve ([F (0), F (b)])
 *
 * 1. Determine all the small orders available to you. (If there are
 *    no satisfactory ones, go back and change b.)
 *   print C.order().factor(limit=1000)
 *
 * 2. Choose an order as one of the prime factors listed in the above step.
 *    (You can also multiply some to get a composite order, though the
 *    tests will crash trying to invert scalars during signing.) We take a
 *    random point and scale it to drop its order to the desired value.
 *    There is some probability this won't work; just try again.
 *   order = 199
 *   P = C.random_point()
 *   P = (int(P.order()) / int(order)) * P
 *   assert(P.order() == order)
 *
 * 3. Print the values. You'll need to use a vim macro or something to
 *    split the hex output into 4-byte chunks.
 *   print "%x %x" % P.xy()
 */
#if defined(EXHAUSTIVE_TEST_ORDER)
#  if EXHAUSTIVE_TEST_ORDER == 199
const secp256k1_ge secp256k1_ge_const_g = SECP256K1_GE_CONST(
    0xFA7CC9A7, 0x0737F2DB, 0xA749DD39, 0x2B4FB069,
    0x3B017A7D, 0xA808C2F1, 0xFB12940C, 0x9EA66C18,
    0x78AC123A, 0x5ED8AEF3, 0x8732BC91, 0x1F3A2868,
    0x48DF246C, 0x808DAE72, 0xCFE52572, 0x7F0501ED
);

const int CURVE_B = 4;
#  elif EXHAUSTIVE_TEST_ORDER == 13
const secp256k1_ge secp256k1_ge_const_g = SECP256K1_GE_CONST(
    0xedc60018, 0xa51a786b, 0x2ea91f4d, 0x4c9416c0,
    0x9de54c3b, 0xa1316554, 0x6cf4345c, 0x7277ef15,
    0x54cb1b6b, 0xdc8c1273, 0x087844ea, 0x43f4603e,
    0x0eaf9a43, 0xf6effe55, 0x939f806d, 0x37adf8ac
);
const int CURVE_B = 2;
#  else
#    error No known generator for the specified exhaustive test group order.
#  endif
#else
/** Generator for secp256k1, value 'g' defined in
 *  "Standards for Efficient Cryptography" (SEC2) 2.7.1.
 */
static const secp256k1_ge secp256k1_ge_const_g = SECP256K1_GE_CONST(
    0x79BE667EUL, 0xF9DCBBACUL, 0x55A06295UL, 0xCE870B07UL,
    0x029BFCDBUL, 0x2DCE28D9UL, 0x59F2815BUL, 0x16F81798UL,
    0x483ADA77UL, 0x26A3C465UL, 0x5DA4FBFCUL, 0x0E1108A8UL,
    0xFD17B448UL, 0xA6855419UL, 0x9C47D08FUL, 0xFB10D4B8UL
);

const int CURVE_B = 7;
#endif

void secp256k1_ge_set_gej_zinv(secp256k1_ge *r, const secp256k1_gej *a, const secp256k1_fe *zi) {
    secp256k1_fe zi2;
    secp256k1_fe zi3;
    secp256k1_fe_sqr(&zi2, zi);
    secp256k1_fe_mul(&zi3, &zi2, zi);
    secp256k1_fe_mul(&r->x, &a->x, &zi2);
    secp256k1_fe_mul(&r->y, &a->y, &zi3);
    r->infinity = a->infinity;
}

void secp256k1_ge_set_xy(secp256k1_ge *r, const secp256k1_fe *x, const secp256k1_fe *y) {
    r->infinity = 0;
    r->x = *x;
    r->y = *y;
}

int secp256k1_ge_is_infinity(const secp256k1_ge *a) {
    return a->infinity;
}

void secp256k1_ge_neg(secp256k1_ge *r, const secp256k1_ge *a) {
    *r = *a;
    secp256k1_fe_normalize_weak(&r->y);
    secp256k1_fe_negate(&r->y, &r->y, 1);
}

void secp256k1_ge_set_gej(secp256k1_ge *r, secp256k1_gej *a) {
    secp256k1_fe z2, z3;
    r->infinity = a->infinity;
    secp256k1_fe_inv(&a->z, &a->z);
    secp256k1_fe_sqr(&z2, &a->z);
    secp256k1_fe_mul(&z3, &a->z, &z2);
    secp256k1_fe_mul(&a->x, &a->x, &z2);
    secp256k1_fe_mul(&a->y, &a->y, &z3);
    secp256k1_fe_set_int(&a->z, 1);
    r->x = a->x;
    r->y = a->y;
}

void secp256k1_ge_set_gej_var(secp256k1_ge *r, secp256k1_gej *a) {
    secp256k1_fe z2, z3;
    r->infinity = a->infinity;
    if (a->infinity) {
        return;
    }
    secp256k1_fe_inv_var(&a->z, &a->z);
    secp256k1_fe_sqr(&z2, &a->z);
    secp256k1_fe_mul(&z3, &a->z, &z2);
    secp256k1_fe_mul(&a->x, &a->x, &z2);
    secp256k1_fe_mul(&a->y, &a->y, &z3);
    secp256k1_fe_set_int(&a->z, 1);
    r->x = a->x;
    r->y = a->y;
}

void secp256k1_ge_set_all_gej_var(secp256k1_ge *r, const secp256k1_gej *a, size_t len, const secp256k1_callback *cb) {
    secp256k1_fe *az;
    secp256k1_fe *azi;
    size_t i;
    size_t count = 0;
    az = (secp256k1_fe *)checked_malloc(cb, sizeof(secp256k1_fe) * len);
    for (i = 0; i < len; i++) {
        if (!a[i].infinity) {
            az[count++] = a[i].z;
        }
    }

    azi = (secp256k1_fe *)checked_malloc(cb, sizeof(secp256k1_fe) * count);
    secp256k1_fe_inv_all_var(azi, az, count);
    free(az);

    count = 0;
    for (i = 0; i < len; i++) {
        r[i].infinity = a[i].infinity;
        if (!a[i].infinity) {
            secp256k1_ge_set_gej_zinv(&r[i], &a[i], &azi[count++]);
        }
    }
    free(azi);
}

void secp256k1_ge_set_table_gej_var(secp256k1_ge *r, const secp256k1_gej *a, const secp256k1_fe *zr, size_t len) {
    size_t i = len - 1;
    secp256k1_fe zi;

    if (len > 0) {
        /* Compute the inverse of the last z coordinate, and use it to compute the last affine output. */
        secp256k1_fe_inv(&zi, &a[i].z);
        secp256k1_ge_set_gej_zinv(&r[i], &a[i], &zi);

        /* Work out way backwards, using the z-ratios to scale the x/y values. */
        while (i > 0) {
            secp256k1_fe_mul(&zi, &zi, &zr[i]);
            i--;
            secp256k1_ge_set_gej_zinv(&r[i], &a[i], &zi);
        }
    }
}

void secp256k1_ge_globalz_set_table_gej(size_t len, secp256k1_ge *r, secp256k1_fe *globalz, const secp256k1_gej *a, const secp256k1_fe *zr) {
    size_t i = len - 1;
    secp256k1_fe zs;

    if (len > 0) {
        /* The z of the final point gives us the "global Z" for the table. */
        r[i].x = a[i].x;
        r[i].y = a[i].y;
        *globalz = a[i].z;
        r[i].infinity = 0;
        zs = zr[i];

        /* Work our way backwards, using the z-ratios to scale the x/y values. */
        while (i > 0) {
            if (i != len - 1) {
                secp256k1_fe_mul(&zs, &zs, &zr[i]);
            }
            i--;
            secp256k1_ge_set_gej_zinv(&r[i], &a[i], &zs);
        }
    }
}

void secp256k1_gej_set_infinity(secp256k1_gej *r) {
    r->infinity = 1;
    secp256k1_fe_clear(&r->x);
    secp256k1_fe_clear(&r->y);
    secp256k1_fe_clear(&r->z);
}

void secp256k1_gej_clear(secp256k1_gej *r) {
    r->infinity = 0;
    secp256k1_fe_clear(&r->x);
    secp256k1_fe_clear(&r->y);
    secp256k1_fe_clear(&r->z);
}

void secp256k1_ge_clear(secp256k1_ge *r) {
    r->infinity = 0;
    secp256k1_fe_clear(&r->x);
    secp256k1_fe_clear(&r->y);
}

int secp256k1_ge_set_xquad(secp256k1_ge *r, const secp256k1_fe *x) {
    secp256k1_fe x2, x3, c;
    r->x = *x;
    secp256k1_fe_sqr(&x2, x);
    secp256k1_fe_mul(&x3, x, &x2);
    r->infinity = 0;
    secp256k1_fe_set_int(&c, CURVE_B);
    secp256k1_fe_add(&c, &x3);
    return secp256k1_fe_sqrt(&r->y, &c);
}

int secp256k1_ge_set_xo_var(secp256k1_ge *r, const secp256k1_fe *x, int odd) {
    if (!secp256k1_ge_set_xquad(r, x)) {
        return 0;
    }
    secp256k1_fe_normalize_var(&r->y);
    if (secp256k1_fe_is_odd(&r->y) != odd) {
        secp256k1_fe_negate(&r->y, &r->y, 1);
    }
    return 1;

}

void secp256k1_gej_set_ge(secp256k1_gej *r, const secp256k1_ge *a) {
   r->infinity = a->infinity;
   r->x = a->x;
   r->y = a->y;
   secp256k1_fe_set_int(&r->z, 1);
}

int secp256k1_gej_eq_x_var(const secp256k1_fe *x, const secp256k1_gej *a) {
    secp256k1_fe r, r2;
    VERIFY_CHECK(!a->infinity);
    secp256k1_fe_sqr(&r, &a->z); secp256k1_fe_mul(&r, &r, x);
    r2 = a->x; secp256k1_fe_normalize_weak(&r2);
    return secp256k1_fe_equal_var(&r, &r2);
}

void secp256k1_gej_neg(secp256k1_gej *r, const secp256k1_gej *a) {
    r->infinity = a->infinity;
    r->x = a->x;
    r->y = a->y;
    r->z = a->z;
    secp256k1_fe_normalize_weak(&r->y);
    secp256k1_fe_negate(&r->y, &r->y, 1);
}

int secp256k1_gej_is_infinity(const secp256k1_gej *a) {
    return a->infinity;
}

int secp256k1_gej_is_valid_var(const secp256k1_gej *a) {
    secp256k1_fe y2, x3, z2, z6;
    if (a->infinity) {
        return 0;
    }
    /** y^2 = x^3 + 7
     *  (Y/Z^3)^2 = (X/Z^2)^3 + 7
     *  Y^2 / Z^6 = X^3 / Z^6 + 7
     *  Y^2 = X^3 + 7*Z^6
     */
    secp256k1_fe_sqr(&y2, &a->y);
    secp256k1_fe_sqr(&x3, &a->x); secp256k1_fe_mul(&x3, &x3, &a->x);
    secp256k1_fe_sqr(&z2, &a->z);
    secp256k1_fe_sqr(&z6, &z2); secp256k1_fe_mul(&z6, &z6, &z2);
    secp256k1_fe_mul_int(&z6, CURVE_B);
    secp256k1_fe_add(&x3, &z6);
    secp256k1_fe_normalize_weak(&x3);
    return secp256k1_fe_equal_var(&y2, &x3);
}

int secp256k1_ge_is_valid_var(const secp256k1_ge *a) {
    secp256k1_fe y2, x3, c;
    if (a->infinity) {
        return 0;
    }
    /* y^2 = x^3 + 7 */
    secp256k1_fe_sqr(&y2, &a->y);
    secp256k1_fe_sqr(&x3, &a->x); secp256k1_fe_mul(&x3, &x3, &a->x);
    secp256k1_fe_set_int(&c, CURVE_B);
    secp256k1_fe_add(&x3, &c);
    secp256k1_fe_normalize_weak(&x3);
    return secp256k1_fe_equal_var(&y2, &x3);
}

void secp256k1_gej_double_var(secp256k1_gej *r, const secp256k1_gej *a, secp256k1_fe *rzr) {
    /* Operations: 3 mul, 4 sqr, 0 normalize, 12 mul_int/add/negate.
     *
     * Note that there is an implementation described at
     *     https://hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-0.html#doubling-dbl-2009-l
     * which trades a multiply for a square, but in practice this is actually slower,
     * mainly because it requires more normalizations.
     */
    secp256k1_fe t1,t2,t3,t4;
    /** For secp256k1, 2Q is infinity if and only if Q is infinity. This is because if 2Q = infinity,
     *  Q must equal -Q, or that Q.y == -(Q.y), or Q.y is 0. For a point on y^2 = x^3 + 7 to have
     *  y=0, x^3 must be -7 mod p. However, -7 has no cube root mod p.
     *
     *  Having said this, if this function receives a point on a sextic twist, e.g. by
     *  a fault attack, it is possible for y to be 0. This happens for y^2 = x^3 + 6,
     *  since -6 does have a cube root mod p. For this point, this function will not set
     *  the infinity flag even though the point doubles to infinity, and the result
     *  point will be gibberish (z = 0 but infinity = 0).
     */
    r->infinity = a->infinity;
    if (r->infinity) {
        if (rzr != NULL) {
            secp256k1_fe_set_int(rzr, 1);
        }
        return;
    }

    if (rzr != NULL) {
        *rzr = a->y;
        secp256k1_fe_normalize_weak(rzr);
        secp256k1_fe_mul_int(rzr, 2);
    }

    secp256k1_fe_mul(&r->z, &a->z, &a->y);
    secp256k1_fe_mul_int(&r->z, 2);       /* Z' = 2*Y*Z (2) */
    secp256k1_fe_sqr(&t1, &a->x);
    secp256k1_fe_mul_int(&t1, 3);         /* T1 = 3*X^2 (3) */
    secp256k1_fe_sqr(&t2, &t1);           /* T2 = 9*X^4 (1) */
    secp256k1_fe_sqr(&t3, &a->y);
    secp256k1_fe_mul_int(&t3, 2);         /* T3 = 2*Y^2 (2) */
    secp256k1_fe_sqr(&t4, &t3);
    secp256k1_fe_mul_int(&t4, 2);         /* T4 = 8*Y^4 (2) */
    secp256k1_fe_mul(&t3, &t3, &a->x);    /* T3 = 2*X*Y^2 (1) */
    r->x = t3;
    secp256k1_fe_mul_int(&r->x, 4);       /* X' = 8*X*Y^2 (4) */
    secp256k1_fe_negate(&r->x, &r->x, 4); /* X' = -8*X*Y^2 (5) */
    secp256k1_fe_add(&r->x, &t2);         /* X' = 9*X^4 - 8*X*Y^2 (6) */
    secp256k1_fe_negate(&t2, &t2, 1);     /* T2 = -9*X^4 (2) */
    secp256k1_fe_mul_int(&t3, 6);         /* T3 = 12*X*Y^2 (6) */
    secp256k1_fe_add(&t3, &t2);           /* T3 = 12*X*Y^2 - 9*X^4 (8) */
    secp256k1_fe_mul(&r->y, &t1, &t3);    /* Y' = 36*X^3*Y^2 - 27*X^6 (1) */
    secp256k1_fe_negate(&t2, &t4, 2);     /* T2 = -8*Y^4 (3) */
    secp256k1_fe_add(&r->y, &t2);         /* Y' = 36*X^3*Y^2 - 27*X^6 - 8*Y^4 (4) */
}

SECP256K1_INLINE void secp256k1_gej_double_nonzero(secp256k1_gej *r, const secp256k1_gej *a, secp256k1_fe *rzr) {
    VERIFY_CHECK(!secp256k1_gej_is_infinity(a));
    secp256k1_gej_double_var(r, a, rzr);
}

void secp256k1_gej_add_var(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_gej *b, secp256k1_fe *rzr) {
    /* Operations: 12 mul, 4 sqr, 2 normalize, 12 mul_int/add/negate */
    secp256k1_fe z22, z12, u1, u2, s1, s2, h, i, i2, h2, h3, t;

    if (a->infinity) {
        VERIFY_CHECK(rzr == NULL);
        *r = *b;
        return;
    }

    if (b->infinity) {
        if (rzr != NULL) {
            secp256k1_fe_set_int(rzr, 1);
        }
        *r = *a;
        return;
    }

    r->infinity = 0;
    secp256k1_fe_sqr(&z22, &b->z);
    secp256k1_fe_sqr(&z12, &a->z);
    secp256k1_fe_mul(&u1, &a->x, &z22);
    secp256k1_fe_mul(&u2, &b->x, &z12);
    secp256k1_fe_mul(&s1, &a->y, &z22); secp256k1_fe_mul(&s1, &s1, &b->z);
    secp256k1_fe_mul(&s2, &b->y, &z12); secp256k1_fe_mul(&s2, &s2, &a->z);
    secp256k1_fe_negate(&h, &u1, 1); secp256k1_fe_add(&h, &u2);
    secp256k1_fe_negate(&i, &s1, 1); secp256k1_fe_add(&i, &s2);
    if (secp256k1_fe_normalizes_to_zero_var(&h)) {
        if (secp256k1_fe_normalizes_to_zero_var(&i)) {
            secp256k1_gej_double_var(r, a, rzr);
        } else {
            if (rzr != NULL) {
                secp256k1_fe_set_int(rzr, 0);
            }
            r->infinity = 1;
        }
        return;
    }
    secp256k1_fe_sqr(&i2, &i);
    secp256k1_fe_sqr(&h2, &h);
    secp256k1_fe_mul(&h3, &h, &h2);
    secp256k1_fe_mul(&h, &h, &b->z);
    if (rzr != NULL) {
        *rzr = h;
    }
    secp256k1_fe_mul(&r->z, &a->z, &h);
    secp256k1_fe_mul(&t, &u1, &h2);
    r->x = t; secp256k1_fe_mul_int(&r->x, 2); secp256k1_fe_add(&r->x, &h3); secp256k1_fe_negate(&r->x, &r->x, 3); secp256k1_fe_add(&r->x, &i2);
    secp256k1_fe_negate(&r->y, &r->x, 5); secp256k1_fe_add(&r->y, &t); secp256k1_fe_mul(&r->y, &r->y, &i);
    secp256k1_fe_mul(&h3, &h3, &s1); secp256k1_fe_negate(&h3, &h3, 1);
    secp256k1_fe_add(&r->y, &h3);
}

void secp256k1_gej_add_ge_var(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b, secp256k1_fe *rzr) {
    /* 8 mul, 3 sqr, 4 normalize, 12 mul_int/add/negate */
    secp256k1_fe z12, u1, u2, s1, s2, h, i, i2, h2, h3, t;
    if (a->infinity) {
        VERIFY_CHECK(rzr == NULL);
        secp256k1_gej_set_ge(r, b);
        return;
    }
    if (b->infinity) {
        if (rzr != NULL) {
            secp256k1_fe_set_int(rzr, 1);
        }
        *r = *a;
        return;
    }
    r->infinity = 0;

    secp256k1_fe_sqr(&z12, &a->z);
    u1 = a->x; secp256k1_fe_normalize_weak(&u1);
    secp256k1_fe_mul(&u2, &b->x, &z12);
    s1 = a->y; secp256k1_fe_normalize_weak(&s1);
    secp256k1_fe_mul(&s2, &b->y, &z12); secp256k1_fe_mul(&s2, &s2, &a->z);
    secp256k1_fe_negate(&h, &u1, 1); secp256k1_fe_add(&h, &u2);
    secp256k1_fe_negate(&i, &s1, 1); secp256k1_fe_add(&i, &s2);
    if (secp256k1_fe_normalizes_to_zero_var(&h)) {
        if (secp256k1_fe_normalizes_to_zero_var(&i)) {
            secp256k1_gej_double_var(r, a, rzr);
        } else {
            if (rzr != NULL) {
                secp256k1_fe_set_int(rzr, 0);
            }
            r->infinity = 1;
        }
        return;
    }
    secp256k1_fe_sqr(&i2, &i);
    secp256k1_fe_sqr(&h2, &h);
    secp256k1_fe_mul(&h3, &h, &h2);
    if (rzr != NULL) {
        *rzr = h;
    }
    secp256k1_fe_mul(&r->z, &a->z, &h);
    secp256k1_fe_mul(&t, &u1, &h2);
    r->x = t; secp256k1_fe_mul_int(&r->x, 2); secp256k1_fe_add(&r->x, &h3); secp256k1_fe_negate(&r->x, &r->x, 3); secp256k1_fe_add(&r->x, &i2);
    secp256k1_fe_negate(&r->y, &r->x, 5); secp256k1_fe_add(&r->y, &t); secp256k1_fe_mul(&r->y, &r->y, &i);
    secp256k1_fe_mul(&h3, &h3, &s1); secp256k1_fe_negate(&h3, &h3, 1);
    secp256k1_fe_add(&r->y, &h3);
}

void secp256k1_gej_add_zinv_var(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b, const secp256k1_fe *bzinv) {
    /* 9 mul, 3 sqr, 4 normalize, 12 mul_int/add/negate */
    secp256k1_fe az, z12, u1, u2, s1, s2, h, i, i2, h2, h3, t;

    if (b->infinity) {
        *r = *a;
        return;
    }
    if (a->infinity) {
        secp256k1_fe bzinv2, bzinv3;
        r->infinity = b->infinity;
        secp256k1_fe_sqr(&bzinv2, bzinv);
        secp256k1_fe_mul(&bzinv3, &bzinv2, bzinv);
        secp256k1_fe_mul(&r->x, &b->x, &bzinv2);
        secp256k1_fe_mul(&r->y, &b->y, &bzinv3);
        secp256k1_fe_set_int(&r->z, 1);
        return;
    }
    r->infinity = 0;

    /** We need to calculate (rx,ry,rz) = (ax,ay,az) + (bx,by,1/bzinv). Due to
     *  secp256k1's isomorphism we can multiply the Z coordinates on both sides
     *  by bzinv, and get: (rx,ry,rz*bzinv) = (ax,ay,az*bzinv) + (bx,by,1).
     *  This means that (rx,ry,rz) can be calculated as
     *  (ax,ay,az*bzinv) + (bx,by,1), when not applying the bzinv factor to rz.
     *  The variable az below holds the modified Z coordinate for a, which is used
     *  for the computation of rx and ry, but not for rz.
     */
    secp256k1_fe_mul(&az, &a->z, bzinv);

    secp256k1_fe_sqr(&z12, &az);
    u1 = a->x; secp256k1_fe_normalize_weak(&u1);
    secp256k1_fe_mul(&u2, &b->x, &z12);
    s1 = a->y; secp256k1_fe_normalize_weak(&s1);
    secp256k1_fe_mul(&s2, &b->y, &z12); secp256k1_fe_mul(&s2, &s2, &az);
    secp256k1_fe_negate(&h, &u1, 1); secp256k1_fe_add(&h, &u2);
    secp256k1_fe_negate(&i, &s1, 1); secp256k1_fe_add(&i, &s2);
    if (secp256k1_fe_normalizes_to_zero_var(&h)) {
        if (secp256k1_fe_normalizes_to_zero_var(&i)) {
            secp256k1_gej_double_var(r, a, NULL);
        } else {
            r->infinity = 1;
        }
        return;
    }
    secp256k1_fe_sqr(&i2, &i);
    secp256k1_fe_sqr(&h2, &h);
    secp256k1_fe_mul(&h3, &h, &h2);
    r->z = a->z; secp256k1_fe_mul(&r->z, &r->z, &h);
    secp256k1_fe_mul(&t, &u1, &h2);
    r->x = t; secp256k1_fe_mul_int(&r->x, 2); secp256k1_fe_add(&r->x, &h3); secp256k1_fe_negate(&r->x, &r->x, 3); secp256k1_fe_add(&r->x, &i2);
    secp256k1_fe_negate(&r->y, &r->x, 5); secp256k1_fe_add(&r->y, &t); secp256k1_fe_mul(&r->y, &r->y, &i);
    secp256k1_fe_mul(&h3, &h3, &s1); secp256k1_fe_negate(&h3, &h3, 1);
    secp256k1_fe_add(&r->y, &h3);
}


void secp256k1_gej_add_ge(secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_ge *b) {
    /* Operations: 7 mul, 5 sqr, 4 normalize, 21 mul_int/add/negate/cmov */
    static const secp256k1_fe fe_1 = SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 1);
    secp256k1_fe zz, u1, u2, s1, s2, t, tt, m, n, q, rr;
    secp256k1_fe m_alt, rr_alt;
    int infinity, degenerate;
    VERIFY_CHECK(!b->infinity);
    VERIFY_CHECK(a->infinity == 0 || a->infinity == 1);

    /** In:
     *    Eric Brier and Marc Joye, Weierstrass Elliptic Curves and Side-Channel Attacks.
     *    In D. Naccache and P. Paillier, Eds., Public Key Cryptography, vol. 2274 of Lecture Notes in Computer Science, pages 335-345. Springer-Verlag, 2002.
     *  we find as solution for a unified addition/doubling formula:
     *    lambda = ((x1 + x2)^2 - x1 * x2 + a) / (y1 + y2), with a = 0 for secp256k1's curve equation.
     *    x3 = lambda^2 - (x1 + x2)
     *    2*y3 = lambda * (x1 + x2 - 2 * x3) - (y1 + y2).
     *
     *  Substituting x_i = Xi / Zi^2 and yi = Yi / Zi^3, for i=1,2,3, gives:
     *    U1 = X1*Z2^2, U2 = X2*Z1^2
     *    S1 = Y1*Z2^3, S2 = Y2*Z1^3
     *    Z = Z1*Z2
     *    T = U1+U2
     *    M = S1+S2
     *    Q = T*M^2
     *    R = T^2-U1*U2
     *    X3 = 4*(R^2-Q)
     *    Y3 = 4*(R*(3*Q-2*R^2)-M^4)
     *    Z3 = 2*M*Z
     *  (Note that the paper uses xi = Xi / Zi and yi = Yi / Zi instead.)
     *
     *  This formula has the benefit of being the same for both addition
     *  of distinct points and doubling. However, it breaks down in the
     *  case that either point is infinity, or that y1 = -y2. We handle
     *  these cases in the following ways:
     *
     *    - If b is infinity we simply bail by means of a VERIFY_CHECK.
     *
     *    - If a is infinity, we detect this, and at the end of the
     *      computation replace the result (which will be meaningless,
     *      but we compute to be constant-time) with b.x : b.y : 1.
     *
     *    - If a = -b, we have y1 = -y2, which is a degenerate case.
     *      But here the answer is infinity, so we simply set the
     *      infinity flag of the result, overriding the computed values
     *      without even needing to cmov.
     *
     *    - If y1 = -y2 but x1 != x2, which does occur thanks to certain
     *      properties of our curve (specifically, 1 has nontrivial cube
     *      roots in our field, and the curve equation has no x coefficient)
     *      then the answer is not infinity but also not given by the above
     *      equation. In this case, we cmov in place an alternate expression
     *      for lambda. Specifically (y1 - y2)/(x1 - x2). Where both these
     *      expressions for lambda are defined, they are equal, and can be
     *      obtained from each other by multiplication by (y1 + y2)/(y1 + y2)
     *      then substitution of x^3 + 7 for y^2 (using the curve equation).
     *      For all pairs of nonzero points (a, b) at least one is defined,
     *      so this covers everything.
     */

    secp256k1_fe_sqr(&zz, &a->z);                       /* z = Z1^2 */
    u1 = a->x; secp256k1_fe_normalize_weak(&u1);        /* u1 = U1 = X1*Z2^2 (1) */
    secp256k1_fe_mul(&u2, &b->x, &zz);                  /* u2 = U2 = X2*Z1^2 (1) */
    s1 = a->y; secp256k1_fe_normalize_weak(&s1);        /* s1 = S1 = Y1*Z2^3 (1) */
    secp256k1_fe_mul(&s2, &b->y, &zz);                  /* s2 = Y2*Z1^2 (1) */
    secp256k1_fe_mul(&s2, &s2, &a->z);                  /* s2 = S2 = Y2*Z1^3 (1) */
    t = u1; secp256k1_fe_add(&t, &u2);                  /* t = T = U1+U2 (2) */
    m = s1; secp256k1_fe_add(&m, &s2);                  /* m = M = S1+S2 (2) */
    secp256k1_fe_sqr(&rr, &t);                          /* rr = T^2 (1) */
    secp256k1_fe_negate(&m_alt, &u2, 1);                /* Malt = -X2*Z1^2 */
    secp256k1_fe_mul(&tt, &u1, &m_alt);                 /* tt = -U1*U2 (2) */
    secp256k1_fe_add(&rr, &tt);                         /* rr = R = T^2-U1*U2 (3) */
    /** If lambda = R/M = 0/0 we have a problem (except in the "trivial"
     *  case that Z = z1z2 = 0, and this is special-cased later on). */
    degenerate = secp256k1_fe_normalizes_to_zero(&m) &
                 secp256k1_fe_normalizes_to_zero(&rr);
    /* This only occurs when y1 == -y2 and x1^3 == x2^3, but x1 != x2.
     * This means either x1 == beta*x2 or beta*x1 == x2, where beta is
     * a nontrivial cube root of one. In either case, an alternate
     * non-indeterminate expression for lambda is (y1 - y2)/(x1 - x2),
     * so we set R/M equal to this. */
    rr_alt = s1;
    secp256k1_fe_mul_int(&rr_alt, 2);       /* rr = Y1*Z2^3 - Y2*Z1^3 (2) */
    secp256k1_fe_add(&m_alt, &u1);          /* Malt = X1*Z2^2 - X2*Z1^2 */

    secp256k1_fe_cmov(&rr_alt, &rr, !degenerate);
    secp256k1_fe_cmov(&m_alt, &m, !degenerate);
    /* Now Ralt / Malt = lambda and is guaranteed not to be 0/0.
     * From here on out Ralt and Malt represent the numerator
     * and denominator of lambda; R and M represent the explicit
     * expressions x1^2 + x2^2 + x1x2 and y1 + y2. */
    secp256k1_fe_sqr(&n, &m_alt);                       /* n = Malt^2 (1) */
    secp256k1_fe_mul(&q, &n, &t);                       /* q = Q = T*Malt^2 (1) */
    /* These two lines use the observation that either M == Malt or M == 0,
     * so M^3 * Malt is either Malt^4 (which is computed by squaring), or
     * zero (which is "computed" by cmov). So the cost is one squaring
     * versus two multiplications. */
    secp256k1_fe_sqr(&n, &n);
    secp256k1_fe_cmov(&n, &m, degenerate);              /* n = M^3 * Malt (2) */
    secp256k1_fe_sqr(&t, &rr_alt);                      /* t = Ralt^2 (1) */
    secp256k1_fe_mul(&r->z, &a->z, &m_alt);             /* r->z = Malt*Z (1) */
    infinity = secp256k1_fe_normalizes_to_zero(&r->z) * (1 - a->infinity);
    secp256k1_fe_mul_int(&r->z, 2);                     /* r->z = Z3 = 2*Malt*Z (2) */
    secp256k1_fe_negate(&q, &q, 1);                     /* q = -Q (2) */
    secp256k1_fe_add(&t, &q);                           /* t = Ralt^2-Q (3) */
    secp256k1_fe_normalize_weak(&t);
    r->x = t;                                           /* r->x = Ralt^2-Q (1) */
    secp256k1_fe_mul_int(&t, 2);                        /* t = 2*x3 (2) */
    secp256k1_fe_add(&t, &q);                           /* t = 2*x3 - Q: (4) */
    secp256k1_fe_mul(&t, &t, &rr_alt);                  /* t = Ralt*(2*x3 - Q) (1) */
    secp256k1_fe_add(&t, &n);                           /* t = Ralt*(2*x3 - Q) + M^3*Malt (3) */
    secp256k1_fe_negate(&r->y, &t, 3);                  /* r->y = Ralt*(Q - 2x3) - M^3*Malt (4) */
    secp256k1_fe_normalize_weak(&r->y);
    secp256k1_fe_mul_int(&r->x, 4);                     /* r->x = X3 = 4*(Ralt^2-Q) */
    secp256k1_fe_mul_int(&r->y, 4);                     /* r->y = Y3 = 4*Ralt*(Q - 2x3) - 4*M^3*Malt (4) */

    /** In case a->infinity == 1, replace r with (b->x, b->y, 1). */
    secp256k1_fe_cmov(&r->x, &b->x, a->infinity);
    secp256k1_fe_cmov(&r->y, &b->y, a->infinity);
    secp256k1_fe_cmov(&r->z, &fe_1, a->infinity);
    r->infinity = infinity;
}

void secp256k1_gej_rescale(secp256k1_gej *r, const secp256k1_fe *s) {
    /* Operations: 4 mul, 1 sqr */
    secp256k1_fe zz;
    VERIFY_CHECK(!secp256k1_fe_is_zero(s));
    secp256k1_fe_sqr(&zz, s);
    secp256k1_fe_mul(&r->x, &r->x, &zz);                /* r->x *= s^2 */
    secp256k1_fe_mul(&r->y, &r->y, &zz);
    secp256k1_fe_mul(&r->y, &r->y, s);                  /* r->y *= s^3 */
    secp256k1_fe_mul(&r->z, &r->z, s);                  /* r->z *= s   */
}

void secp256k1_ge_to_storage(secp256k1_ge_storage *r, const secp256k1_ge *a) {
    secp256k1_fe x, y;
    VERIFY_CHECK(!a->infinity);
    x = a->x;
    secp256k1_fe_normalize(&x);
    y = a->y;
    secp256k1_fe_normalize(&y);
    secp256k1_fe_to_storage(&r->x, &x);
    secp256k1_fe_to_storage(&r->y, &y);
}

void secp256k1_ge_from_storage(secp256k1_ge *r, const secp256k1_ge_storage *a) {
    secp256k1_fe_from_storage(&r->x, &a->x);
    secp256k1_fe_from_storage(&r->y, &a->y);
    r->infinity = 0;
}

SECP256K1_INLINE void secp256k1_ge_storage_cmov(secp256k1_ge_storage *r, const secp256k1_ge_storage *a, int flag) {
    secp256k1_fe_storage_cmov(&r->x, &a->x, flag);
    secp256k1_fe_storage_cmov(&r->y, &a->y, flag);
}

#ifdef USE_ENDOMORPHISM
void secp256k1_ge_mul_lambda(secp256k1_ge *r, const secp256k1_ge *a) {
    static const secp256k1_fe beta = SECP256K1_FE_CONST(
        0x7ae96a2bul, 0x657c0710ul, 0x6e64479eul, 0xac3434e9ul,
        0x9cf04975ul, 0x12f58995ul, 0xc1396c28ul, 0x719501eeul
    );
    *r = *a;
    secp256k1_fe_mul(&r->x, &r->x, &beta);
}
#endif

int secp256k1_gej_has_quad_y_var(const secp256k1_gej *a) {
    secp256k1_fe yz;

    if (a->infinity) {
        return 0;
    }

    /* We rely on the fact that the Jacobi symbol of 1 / a->z^3 is the same as
     * that of a->z. Thus a->y / a->z^3 is a quadratic residue iff a->y * a->z
       is */
    secp256k1_fe_mul(&yz, &a->y, &a->z);
    return secp256k1_fe_is_quad_var(&yz);
}
///End of secp256k1/src/group_impl.h


///From secp256k1/src/ecmult_impl.h
#if defined(EXHAUSTIVE_TEST_ORDER)
/* We need to lower these values for exhaustive tests because
 * the tables cannot have infinities in them (this breaks the
 * affine-isomorphism stuff which tracks z-ratios) */
#  if EXHAUSTIVE_TEST_ORDER > 128
#    define WINDOW_A 5
#    define WINDOW_G 8
#  elif EXHAUSTIVE_TEST_ORDER > 8
#    define WINDOW_A 4
#    define WINDOW_G 4
#  else
#    define WINDOW_A 2
#    define WINDOW_G 2
#  endif
#else
/* optimal for 128-bit and 256-bit exponents. */
#define WINDOW_A 5
/** larger numbers may result in slightly better performance, at the cost of
    exponentially larger precomputed tables. */
#ifdef USE_ENDOMORPHISM
/** Two tables for window size 15: 1.375 MiB. */
#define WINDOW_G 15
#else
/** One table for window size 16: 1.375 MiB. */
#define WINDOW_G 16
#endif
#endif

/** The number of entries a table with precomputed multiples needs to have. */
#define ECMULT_TABLE_SIZE(w) (1 << ((w)-2))

/** Fill a table 'prej' with precomputed odd multiples of a. Prej will contain
 *  the values [1*a,3*a,...,(2*n-1)*a], so it space for n values. zr[0] will
 *  contain prej[0].z / a.z. The other zr[i] values = prej[i].z / prej[i-1].z.
 *  Prej's Z values are undefined, except for the last value.
 */
void secp256k1_ecmult_odd_multiples_table(int n, secp256k1_gej *prej, secp256k1_fe *zr, const secp256k1_gej *a) {
    secp256k1_gej d;
    secp256k1_ge a_ge, d_ge;
    int i;

    VERIFY_CHECK(!a->infinity);

    secp256k1_gej_double_var(&d, a, NULL);

    /*
     * Perform the additions on an isomorphism where 'd' is affine: drop the z coordinate
     * of 'd', and scale the 1P starting value's x/y coordinates without changing its z.
     */
    d_ge.x = d.x;
    d_ge.y = d.y;
    d_ge.infinity = 0;

    secp256k1_ge_set_gej_zinv(&a_ge, a, &d.z);
    prej[0].x = a_ge.x;
    prej[0].y = a_ge.y;
    prej[0].z = a->z;
    prej[0].infinity = 0;

    zr[0] = d.z;
    for (i = 1; i < n; i++) {
        secp256k1_gej_add_ge_var(&prej[i], &prej[i-1], &d_ge, &zr[i]);
    }

    /*
     * Each point in 'prej' has a z coordinate too small by a factor of 'd.z'. Only
     * the final point's z coordinate is actually used though, so just update that.
     */
    secp256k1_fe_mul(&prej[n-1].z, &prej[n-1].z, &d.z);
}

/** Fill a table 'pre' with precomputed odd multiples of a.
 *
 *  There are two versions of this function:
 *  - secp256k1_ecmult_odd_multiples_table_globalz_windowa which brings its
 *    resulting point set to a single constant Z denominator, stores the X and Y
 *    coordinates as ge_storage points in pre, and stores the global Z in rz.
 *    It only operates on tables sized for WINDOW_A wnaf multiples.
 *  - secp256k1_ecmult_odd_multiples_table_storage_var, which converts its
 *    resulting point set to actually affine points, and stores those in pre.
 *    It operates on tables of any size, but uses heap-allocated temporaries.
 *
 *  To compute a*P + b*G, we compute a table for P using the first function,
 *  and for G using the second (which requires an inverse, but it only needs to
 *  happen once).
 */
void secp256k1_ecmult_odd_multiples_table_globalz_windowa(secp256k1_ge *pre, secp256k1_fe *globalz, const secp256k1_gej *a) {
    secp256k1_gej prej[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_fe zr[ECMULT_TABLE_SIZE(WINDOW_A)];

    /* Compute the odd multiples in Jacobian form. */
    secp256k1_ecmult_odd_multiples_table(ECMULT_TABLE_SIZE(WINDOW_A), prej, zr, a);
    /* Bring them to the same Z denominator. */
    secp256k1_ge_globalz_set_table_gej(ECMULT_TABLE_SIZE(WINDOW_A), pre, globalz, prej, zr);
}

void secp256k1_ecmult_odd_multiples_table_storage_var(int n, secp256k1_ge_storage *pre, const secp256k1_gej *a, const secp256k1_callback *cb) {
    secp256k1_gej *prej = (secp256k1_gej*)checked_malloc(cb, sizeof(secp256k1_gej) * n);
    secp256k1_ge *prea = (secp256k1_ge*)checked_malloc(cb, sizeof(secp256k1_ge) * n);
    secp256k1_fe *zr = (secp256k1_fe*)checked_malloc(cb, sizeof(secp256k1_fe) * n);
    int i;

    /* Compute the odd multiples in Jacobian form. */
    secp256k1_ecmult_odd_multiples_table(n, prej, zr, a);
    /* Convert them in batch to affine coordinates. */
    secp256k1_ge_set_table_gej_var(prea, prej, zr, n);
    /* Convert them to compact storage form. */
    for (i = 0; i < n; i++) {
        secp256k1_ge_to_storage(&pre[i], &prea[i]);
    }

    free(prea);
    free(prej);
    free(zr);
}

/** The following two macro retrieves a particular odd multiple from a table
 *  of precomputed multiples. */
#define ECMULT_TABLE_GET_GE(r,pre,n,w) do { \
    VERIFY_CHECK(((n) & 1) == 1); \
    VERIFY_CHECK((n) >= -((1 << ((w)-1)) - 1)); \
    VERIFY_CHECK((n) <=  ((1 << ((w)-1)) - 1)); \
    if ((n) > 0) { \
        *(r) = (pre)[((n)-1)/2]; \
    } else { \
        secp256k1_ge_neg((r), &(pre)[(-(n)-1)/2]); \
    } \
} while(0)

#define ECMULT_TABLE_GET_GE_STORAGE(r,pre,n,w) do { \
    VERIFY_CHECK(((n) & 1) == 1); \
    VERIFY_CHECK((n) >= -((1 << ((w)-1)) - 1)); \
    VERIFY_CHECK((n) <=  ((1 << ((w)-1)) - 1)); \
    if ((n) > 0) { \
        secp256k1_ge_from_storage((r), &(pre)[((n)-1)/2]); \
    } else { \
        secp256k1_ge_from_storage((r), &(pre)[(-(n)-1)/2]); \
        secp256k1_ge_neg((r), (r)); \
    } \
} while(0)

void secp256k1_ecmult_context_init(secp256k1_ecmult_context *ctx) {
    ctx->pre_g = NULL;
#ifdef USE_ENDOMORPHISM
    ctx->pre_g_128 = NULL;
#endif
}

void secp256k1_ecmult_context_build(secp256k1_ecmult_context *ctx, const secp256k1_callback *cb) {
    secp256k1_gej gj;

    if (ctx->pre_g != NULL) {
        return;
    }

    /* get the generator */
    secp256k1_gej_set_ge(&gj, &secp256k1_ge_const_g);

    ctx->pre_g = (secp256k1_ge_storage (*)[])checked_malloc(cb, sizeof((*ctx->pre_g)[0]) * ECMULT_TABLE_SIZE(WINDOW_G));

    /* precompute the tables with odd multiples */
    secp256k1_ecmult_odd_multiples_table_storage_var(ECMULT_TABLE_SIZE(WINDOW_G), *ctx->pre_g, &gj, cb);

#ifdef USE_ENDOMORPHISM
    {
        secp256k1_gej g_128j;
        int i;

        ctx->pre_g_128 = (secp256k1_ge_storage (*)[])checked_malloc(cb, sizeof((*ctx->pre_g_128)[0]) * ECMULT_TABLE_SIZE(WINDOW_G));

        /* calculate 2^128*generator */
        g_128j = gj;
        for (i = 0; i < 128; i++) {
            secp256k1_gej_double_var(&g_128j, &g_128j, NULL);
        }
        secp256k1_ecmult_odd_multiples_table_storage_var(ECMULT_TABLE_SIZE(WINDOW_G), *ctx->pre_g_128, &g_128j, cb);
    }
#endif
}

void secp256k1_ecmult_context_clone(secp256k1_ecmult_context *dst,
                                           const secp256k1_ecmult_context *src, const secp256k1_callback *cb) {
    if (src->pre_g == NULL) {
        dst->pre_g = NULL;
    } else {
        size_t size = sizeof((*dst->pre_g)[0]) * ECMULT_TABLE_SIZE(WINDOW_G);
        dst->pre_g = (secp256k1_ge_storage (*)[])checked_malloc(cb, size);
        memcpy(dst->pre_g, src->pre_g, size);
    }
#ifdef USE_ENDOMORPHISM
    if (src->pre_g_128 == NULL) {
        dst->pre_g_128 = NULL;
    } else {
        size_t size = sizeof((*dst->pre_g_128)[0]) * ECMULT_TABLE_SIZE(WINDOW_G);
        dst->pre_g_128 = (secp256k1_ge_storage (*)[])checked_malloc(cb, size);
        memcpy(dst->pre_g_128, src->pre_g_128, size);
    }
#endif
}

int secp256k1_ecmult_context_is_built(const secp256k1_ecmult_context *ctx) {
    return ctx->pre_g != NULL;
}

void secp256k1_ecmult_context_clear(secp256k1_ecmult_context *ctx) {
    free(ctx->pre_g);
#ifdef USE_ENDOMORPHISM
    free(ctx->pre_g_128);
#endif
    secp256k1_ecmult_context_init(ctx);
}

/** Convert a number to WNAF notation. The number becomes represented by sum(2^i * wnaf[i], i=0..bits),
 *  with the following guarantees:
 *  - each wnaf[i] is either 0, or an odd integer between -(1<<(w-1) - 1) and (1<<(w-1) - 1)
 *  - two non-zero entries in wnaf are separated by at least w-1 zeroes.
 *  - the number of set values in wnaf is returned. This number is at most 256, and at most one more
 *    than the number of bits in the (absolute value) of the input.
 */
int secp256k1_ecmult_wnaf(int *wnaf, int len, const secp256k1_scalar *a, int w) {
    secp256k1_scalar s = *a;
    int last_set_bit = -1;
    int bit = 0;
    int sign = 1;
    int carry = 0;

    VERIFY_CHECK(wnaf != NULL);
    VERIFY_CHECK(0 <= len && len <= 256);
    VERIFY_CHECK(a != NULL);
    VERIFY_CHECK(2 <= w && w <= 31);

    memset(wnaf, 0, len * sizeof(wnaf[0]));

    if (secp256k1_scalar_get_bits(&s, 255, 1)) {
        secp256k1_scalar_negate(&s, &s);
        sign = -1;
    }

    while (bit < len) {
        int now;
        int word;
        if (secp256k1_scalar_get_bits(&s, bit, 1) == (unsigned int)carry) {
            bit++;
            continue;
        }

        now = w;
        if (now > len - bit) {
            now = len - bit;
        }

        word = secp256k1_scalar_get_bits_var(&s, bit, now) + carry;

        carry = (word >> (w-1)) & 1;
        word -= carry << w;

        wnaf[bit] = sign * word;
        last_set_bit = bit;

        bit += now;
    }
#ifdef VERIFY
    CHECK(carry == 0);
    while (bit < 256) {
        CHECK(secp256k1_scalar_get_bits(&s, bit++, 1) == 0);
    }
#endif
    return last_set_bit + 1;
}

void secp256k1_ecmult(const secp256k1_ecmult_context *ctx, secp256k1_gej *r, const secp256k1_gej *a, const secp256k1_scalar *na, const secp256k1_scalar *ng) {
    secp256k1_ge pre_a[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_ge tmpa;
    secp256k1_fe Z;
#ifdef USE_ENDOMORPHISM
    secp256k1_ge pre_a_lam[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_scalar na_1, na_lam;
    /* Splitted G factors. */
    secp256k1_scalar ng_1, ng_128;
    int wnaf_na_1[130];
    int wnaf_na_lam[130];
    int bits_na_1;
    int bits_na_lam;
    int wnaf_ng_1[129];
    int bits_ng_1;
    int wnaf_ng_128[129];
    int bits_ng_128;
#else
    int wnaf_na[256];
    int bits_na;
    int wnaf_ng[256];
    int bits_ng;
#endif
    int i;
    int bits;

#ifdef USE_ENDOMORPHISM
    /* split na into na_1 and na_lam (where na = na_1 + na_lam*lambda, and na_1 and na_lam are ~128 bit) */
    secp256k1_scalar_split_lambda(&na_1, &na_lam, na);

    /* build wnaf representation for na_1 and na_lam. */
    bits_na_1   = secp256k1_ecmult_wnaf(wnaf_na_1,   130, &na_1,   WINDOW_A);
    bits_na_lam = secp256k1_ecmult_wnaf(wnaf_na_lam, 130, &na_lam, WINDOW_A);
    VERIFY_CHECK(bits_na_1 <= 130);
    VERIFY_CHECK(bits_na_lam <= 130);
    bits = bits_na_1;
    if (bits_na_lam > bits) {
        bits = bits_na_lam;
    }
#else
    /* build wnaf representation for na. */
    bits_na     = secp256k1_ecmult_wnaf(wnaf_na,     256, na,      WINDOW_A);
    bits = bits_na;
#endif

    /* Calculate odd multiples of a.
     * All multiples are brought to the same Z 'denominator', which is stored
     * in Z. Due to secp256k1' isomorphism we can do all operations pretending
     * that the Z coordinate was 1, use affine addition formulae, and correct
     * the Z coordinate of the result once at the end.
     * The exception is the precomputed G table points, which are actually
     * affine. Compared to the base used for other points, they have a Z ratio
     * of 1/Z, so we can use secp256k1_gej_add_zinv_var, which uses the same
     * isomorphism to efficiently add with a known Z inverse.
     */
    secp256k1_ecmult_odd_multiples_table_globalz_windowa(pre_a, &Z, a);

#ifdef USE_ENDOMORPHISM
    for (i = 0; i < ECMULT_TABLE_SIZE(WINDOW_A); i++) {
        secp256k1_ge_mul_lambda(&pre_a_lam[i], &pre_a[i]);
    }

    /* split ng into ng_1 and ng_128 (where gn = gn_1 + gn_128*2^128, and gn_1 and gn_128 are ~128 bit) */
    secp256k1_scalar_split_128(&ng_1, &ng_128, ng);

    /* Build wnaf representation for ng_1 and ng_128 */
    bits_ng_1   = secp256k1_ecmult_wnaf(wnaf_ng_1,   129, &ng_1,   WINDOW_G);
    bits_ng_128 = secp256k1_ecmult_wnaf(wnaf_ng_128, 129, &ng_128, WINDOW_G);
    if (bits_ng_1 > bits) {
        bits = bits_ng_1;
    }
    if (bits_ng_128 > bits) {
        bits = bits_ng_128;
    }
#else
    bits_ng     = secp256k1_ecmult_wnaf(wnaf_ng,     256, ng,      WINDOW_G);
    if (bits_ng > bits) {
        bits = bits_ng;
    }
#endif

    secp256k1_gej_set_infinity(r);

    for (i = bits - 1; i >= 0; i--) {
        int n;
        secp256k1_gej_double_var(r, r, NULL);
#ifdef USE_ENDOMORPHISM
        if (i < bits_na_1 && (n = wnaf_na_1[i])) {
            ECMULT_TABLE_GET_GE(&tmpa, pre_a, n, WINDOW_A);
            secp256k1_gej_add_ge_var(r, r, &tmpa, NULL);
        }
        if (i < bits_na_lam && (n = wnaf_na_lam[i])) {
            ECMULT_TABLE_GET_GE(&tmpa, pre_a_lam, n, WINDOW_A);
            secp256k1_gej_add_ge_var(r, r, &tmpa, NULL);
        }
        if (i < bits_ng_1 && (n = wnaf_ng_1[i])) {
            ECMULT_TABLE_GET_GE_STORAGE(&tmpa, *ctx->pre_g, n, WINDOW_G);
            secp256k1_gej_add_zinv_var(r, r, &tmpa, &Z);
        }
        if (i < bits_ng_128 && (n = wnaf_ng_128[i])) {
            ECMULT_TABLE_GET_GE_STORAGE(&tmpa, *ctx->pre_g_128, n, WINDOW_G);
            secp256k1_gej_add_zinv_var(r, r, &tmpa, &Z);
        }
#else
        if (i < bits_na && (n = wnaf_na[i])) {
            ECMULT_TABLE_GET_GE(&tmpa, pre_a, n, WINDOW_A);
            secp256k1_gej_add_ge_var(r, r, &tmpa, NULL);
        }
        if (i < bits_ng && (n = wnaf_ng[i])) {
            ECMULT_TABLE_GET_GE_STORAGE(&tmpa, *ctx->pre_g, n, WINDOW_G);
            secp256k1_gej_add_zinv_var(r, r, &tmpa, &Z);
        }
#endif
    }

    if (!r->infinity) {
        secp256k1_fe_mul(&r->z, &r->z, &Z);
    }
}
///end of secp256k1/src/ecmult_impl.h


///From secp256k1/src/ecmult_const_impl.h
/**********************************************************************
 * Copyright (c) 2015 Pieter Wuille, Andrew Poelstra                  *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/
#ifdef USE_ENDOMORPHISM
    #define WNAF_BITS 128
#else
    #define WNAF_BITS 256
#endif
#define WNAF_SIZE(w) ((WNAF_BITS + (w) - 1) / (w))

/* This is like `ECMULT_TABLE_GET_GE` but is constant time */
#define ECMULT_CONST_TABLE_GET_GE(r,pre,n,w) do { \
    int m; \
    int abs_n = (n) * (((n) > 0) * 2 - 1); \
    int idx_n = abs_n / 2; \
    secp256k1_fe neg_y; \
    VERIFY_CHECK(((n) & 1) == 1); \
    VERIFY_CHECK((n) >= -((1 << ((w)-1)) - 1)); \
    VERIFY_CHECK((n) <=  ((1 << ((w)-1)) - 1)); \
    VERIFY_SETUP(secp256k1_fe_clear(&(r)->x)); \
    VERIFY_SETUP(secp256k1_fe_clear(&(r)->y)); \
    for (m = 0; m < ECMULT_TABLE_SIZE(w); m++) { \
        /* This loop is used to avoid secret data in array indices. See
         * the comment in ecmult_gen_impl.h for rationale. */ \
        secp256k1_fe_cmov(&(r)->x, &(pre)[m].x, m == idx_n); \
        secp256k1_fe_cmov(&(r)->y, &(pre)[m].y, m == idx_n); \
    } \
    (r)->infinity = 0; \
    secp256k1_fe_negate(&neg_y, &(r)->y, 1); \
    secp256k1_fe_cmov(&(r)->y, &neg_y, (n) != abs_n); \
} while(0)


/** Convert a number to WNAF notation. The number becomes represented by sum(2^{wi} * wnaf[i], i=0..return_val)
 *  with the following guarantees:
 *  - each wnaf[i] an odd integer between -(1 << w) and (1 << w)
 *  - each wnaf[i] is nonzero
 *  - the number of words set is returned; this is always (WNAF_BITS + w - 1) / w
 *
 *  Adapted from `The Width-w NAF Method Provides Small Memory and Fast Elliptic Scalar
 *  Multiplications Secure against Side Channel Attacks`, Okeya and Tagaki. M. Joye (Ed.)
 *  CT-RSA 2003, LNCS 2612, pp. 328-443, 2003. Springer-Verlagy Berlin Heidelberg 2003
 *
 *  Numbers reference steps of `Algorithm SPA-resistant Width-w NAF with Odd Scalar` on pp. 335
 */
int secp256k1_wnaf_const(int *wnaf, secp256k1_scalar s, int w) {
    int global_sign;
    int skew = 0;
    int word = 0;

    /* 1 2 3 */
    int u_last;
    int u;

    int flip;
    int bit;
    secp256k1_scalar neg_s;
    int not_neg_one;
    /* Note that we cannot handle even numbers by negating them to be odd, as is
     * done in other implementations, since if our scalars were specified to have
     * width < 256 for performance reasons, their negations would have width 256
     * and we'd lose any performance benefit. Instead, we use a technique from
     * Section 4.2 of the Okeya/Tagaki paper, which is to add either 1 (for even)
     * or 2 (for odd) to the number we are encoding, returning a skew value indicating
     * this, and having the caller compensate after doing the multiplication. */

    /* Negative numbers will be negated to keep their bit representation below the maximum width */
    flip = secp256k1_scalar_is_high(&s);
    /* We add 1 to even numbers, 2 to odd ones, noting that negation flips parity */
    bit = flip ^ !secp256k1_scalar_is_even(&s);
    /* We check for negative one, since adding 2 to it will cause an overflow */
    secp256k1_scalar_negate(&neg_s, &s);
    not_neg_one = !secp256k1_scalar_is_one(&neg_s);
    secp256k1_scalar_cadd_bit(&s, bit, not_neg_one);
    /* If we had negative one, flip == 1, s.d[0] == 0, bit == 1, so caller expects
     * that we added two to it and flipped it. In fact for -1 these operations are
     * identical. We only flipped, but since skewing is required (in the sense that
     * the skew must be 1 or 2, never zero) and flipping is not, we need to change
     * our flags to claim that we only skewed. */
    global_sign = secp256k1_scalar_cond_negate(&s, flip);
    global_sign *= not_neg_one * 2 - 1;
    skew = 1 << bit;

    /* 4 */
    u_last = secp256k1_scalar_shr_int(&s, w);
    while (word * w < WNAF_BITS) {
        int sign;
        int even;

        /* 4.1 4.4 */
        u = secp256k1_scalar_shr_int(&s, w);
        /* 4.2 */
        even = ((u & 1) == 0);
        sign = 2 * (u_last > 0) - 1;
        u += sign * even;
        u_last -= sign * even * (1 << w);

        /* 4.3, adapted for global sign change */
        wnaf[word++] = u_last * global_sign;

        u_last = u;
    }
    wnaf[word] = u * global_sign;

    VERIFY_CHECK(secp256k1_scalar_is_zero(&s));
    VERIFY_CHECK(word == WNAF_SIZE(w));
    return skew;
}


void secp256k1_ecmult_const(secp256k1_gej *r, const secp256k1_ge *a, const secp256k1_scalar *scalar) {
    secp256k1_ge pre_a[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_ge tmpa;
    secp256k1_fe Z;

    int skew_1;
    int wnaf_1[1 + WNAF_SIZE(WINDOW_A - 1)];
#ifdef USE_ENDOMORPHISM
    secp256k1_ge pre_a_lam[ECMULT_TABLE_SIZE(WINDOW_A)];
    int wnaf_lam[1 + WNAF_SIZE(WINDOW_A - 1)];
    int skew_lam;
    secp256k1_scalar q_1, q_lam;
#endif

    int i;
    secp256k1_scalar sc = *scalar;

    /* build wnaf representation for q. */
#ifdef USE_ENDOMORPHISM
    /* split q into q_1 and q_lam (where q = q_1 + q_lam*lambda, and q_1 and q_lam are ~128 bit) */
    secp256k1_scalar_split_lambda(&q_1, &q_lam, &sc);
    skew_1   = secp256k1_wnaf_const(wnaf_1,   q_1,   WINDOW_A - 1);
    skew_lam = secp256k1_wnaf_const(wnaf_lam, q_lam, WINDOW_A - 1);
#else
    skew_1   = secp256k1_wnaf_const(wnaf_1, sc, WINDOW_A - 1);
#endif

    /* Calculate odd multiples of a.
     * All multiples are brought to the same Z 'denominator', which is stored
     * in Z. Due to secp256k1' isomorphism we can do all operations pretending
     * that the Z coordinate was 1, use affine addition formulae, and correct
     * the Z coordinate of the result once at the end.
     */
    secp256k1_gej_set_ge(r, a);
    secp256k1_ecmult_odd_multiples_table_globalz_windowa(pre_a, &Z, r);
    for (i = 0; i < ECMULT_TABLE_SIZE(WINDOW_A); i++) {
        secp256k1_fe_normalize_weak(&pre_a[i].y);
    }
#ifdef USE_ENDOMORPHISM
    for (i = 0; i < ECMULT_TABLE_SIZE(WINDOW_A); i++) {
        secp256k1_ge_mul_lambda(&pre_a_lam[i], &pre_a[i]);
    }
#endif

    /* first loop iteration (separated out so we can directly set r, rather
     * than having it start at infinity, get doubled several times, then have
     * its new value added to it) */
    i = wnaf_1[WNAF_SIZE(WINDOW_A - 1)];
    VERIFY_CHECK(i != 0);
    ECMULT_CONST_TABLE_GET_GE(&tmpa, pre_a, i, WINDOW_A);
    secp256k1_gej_set_ge(r, &tmpa);
#ifdef USE_ENDOMORPHISM
    i = wnaf_lam[WNAF_SIZE(WINDOW_A - 1)];
    VERIFY_CHECK(i != 0);
    ECMULT_CONST_TABLE_GET_GE(&tmpa, pre_a_lam, i, WINDOW_A);
    secp256k1_gej_add_ge(r, r, &tmpa);
#endif
    /* remaining loop iterations */
    for (i = WNAF_SIZE(WINDOW_A - 1) - 1; i >= 0; i--) {
        int n;
        int j;
        for (j = 0; j < WINDOW_A - 1; ++j) {
            secp256k1_gej_double_nonzero(r, r, NULL);
        }

        n = wnaf_1[i];
        ECMULT_CONST_TABLE_GET_GE(&tmpa, pre_a, n, WINDOW_A);
        VERIFY_CHECK(n != 0);
        secp256k1_gej_add_ge(r, r, &tmpa);
#ifdef USE_ENDOMORPHISM
        n = wnaf_lam[i];
        ECMULT_CONST_TABLE_GET_GE(&tmpa, pre_a_lam, n, WINDOW_A);
        VERIFY_CHECK(n != 0);
        secp256k1_gej_add_ge(r, r, &tmpa);
#endif
    }

    secp256k1_fe_mul(&r->z, &r->z, &Z);

    {
        /* Correct for wNAF skew */
        secp256k1_ge correction = *a;
        secp256k1_ge_storage correction_1_stor;
#ifdef USE_ENDOMORPHISM
        secp256k1_ge_storage correction_lam_stor;
#endif
        secp256k1_ge_storage a2_stor;
        secp256k1_gej tmpj;
        secp256k1_gej_set_ge(&tmpj, &correction);
        secp256k1_gej_double_var(&tmpj, &tmpj, NULL);
        secp256k1_ge_set_gej(&correction, &tmpj);
        secp256k1_ge_to_storage(&correction_1_stor, a);
#ifdef USE_ENDOMORPHISM
        secp256k1_ge_to_storage(&correction_lam_stor, a);
#endif
        secp256k1_ge_to_storage(&a2_stor, &correction);

        /* For odd numbers this is 2a (so replace it), for even ones a (so no-op) */
        secp256k1_ge_storage_cmov(&correction_1_stor, &a2_stor, skew_1 == 2);
#ifdef USE_ENDOMORPHISM
        secp256k1_ge_storage_cmov(&correction_lam_stor, &a2_stor, skew_lam == 2);
#endif

        /* Apply the correction */
        secp256k1_ge_from_storage(&correction, &correction_1_stor);
        secp256k1_ge_neg(&correction, &correction);
        secp256k1_gej_add_ge(r, r, &correction);

#ifdef USE_ENDOMORPHISM
        secp256k1_ge_from_storage(&correction, &correction_lam_stor);
        secp256k1_ge_neg(&correction, &correction);
        secp256k1_ge_mul_lambda(&correction, &correction);
        secp256k1_gej_add_ge(r, r, &correction);
#endif
    }
}
///end of secp256k1/src/ecmult_const_impl.h


///From secp256k1/src/hash_impl.h
/**********************************************************************
 * Copyright (c) 2014 Pieter Wuille                                   *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/
#define Ch(x,y,z) ((z) ^ ((x) & ((y) ^ (z))))
#define Maj(x,y,z) (((x) & (y)) | ((z) & ((x) | (y))))
#define Sigma0(x) (((x) >> 2 | (x) << 30) ^ ((x) >> 13 | (x) << 19) ^ ((x) >> 22 | (x) << 10))
#define Sigma1(x) (((x) >> 6 | (x) << 26) ^ ((x) >> 11 | (x) << 21) ^ ((x) >> 25 | (x) << 7))
#define sigma0(x) (((x) >> 7 | (x) << 25) ^ ((x) >> 18 | (x) << 14) ^ ((x) >> 3))
#define sigma1(x) (((x) >> 17 | (x) << 15) ^ ((x) >> 19 | (x) << 13) ^ ((x) >> 10))

#define Round(a,b,c,d,e,f,g,h,k,w) do { \
    uint32_t t1 = (h) + Sigma1(e) + Ch((e), (f), (g)) + (k) + (w); \
    uint32_t t2 = Sigma0(a) + Maj((a), (b), (c)); \
    (d) += t1; \
    (h) = t1 + t2; \
} while(0)

#ifdef WORDS_BIGENDIAN
#define BE32(x) (x)
#else
#define BE32(p) ((((p) & 0xFF) << 24) | (((p) & 0xFF00) << 8) | (((p) & 0xFF0000) >> 8) | (((p) & 0xFF000000) >> 24))
#endif

void secp256k1_sha256_initialize(secp256k1_sha256_t *hash) {
    hash->s[0] = 0x6a09e667ul;
    hash->s[1] = 0xbb67ae85ul;
    hash->s[2] = 0x3c6ef372ul;
    hash->s[3] = 0xa54ff53aul;
    hash->s[4] = 0x510e527ful;
    hash->s[5] = 0x9b05688cul;
    hash->s[6] = 0x1f83d9abul;
    hash->s[7] = 0x5be0cd19ul;
    hash->bytes = 0;
}

/** Perform one SHA-256 transformation, processing 16 big endian 32-bit words. */
void secp256k1_sha256_transform(uint32_t* s, const uint32_t* chunk) {
    uint32_t a = s[0], b = s[1], c = s[2], d = s[3], e = s[4], f = s[5], g = s[6], h = s[7];
    uint32_t w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15;

    Round(a, b, c, d, e, f, g, h, 0x428a2f98, w0 = BE32(chunk[0]));
    Round(h, a, b, c, d, e, f, g, 0x71374491, w1 = BE32(chunk[1]));
    Round(g, h, a, b, c, d, e, f, 0xb5c0fbcf, w2 = BE32(chunk[2]));
    Round(f, g, h, a, b, c, d, e, 0xe9b5dba5, w3 = BE32(chunk[3]));
    Round(e, f, g, h, a, b, c, d, 0x3956c25b, w4 = BE32(chunk[4]));
    Round(d, e, f, g, h, a, b, c, 0x59f111f1, w5 = BE32(chunk[5]));
    Round(c, d, e, f, g, h, a, b, 0x923f82a4, w6 = BE32(chunk[6]));
    Round(b, c, d, e, f, g, h, a, 0xab1c5ed5, w7 = BE32(chunk[7]));
    Round(a, b, c, d, e, f, g, h, 0xd807aa98, w8 = BE32(chunk[8]));
    Round(h, a, b, c, d, e, f, g, 0x12835b01, w9 = BE32(chunk[9]));
    Round(g, h, a, b, c, d, e, f, 0x243185be, w10 = BE32(chunk[10]));
    Round(f, g, h, a, b, c, d, e, 0x550c7dc3, w11 = BE32(chunk[11]));
    Round(e, f, g, h, a, b, c, d, 0x72be5d74, w12 = BE32(chunk[12]));
    Round(d, e, f, g, h, a, b, c, 0x80deb1fe, w13 = BE32(chunk[13]));
    Round(c, d, e, f, g, h, a, b, 0x9bdc06a7, w14 = BE32(chunk[14]));
    Round(b, c, d, e, f, g, h, a, 0xc19bf174, w15 = BE32(chunk[15]));

    Round(a, b, c, d, e, f, g, h, 0xe49b69c1, w0 += sigma1(w14) + w9 + sigma0(w1));
    Round(h, a, b, c, d, e, f, g, 0xefbe4786, w1 += sigma1(w15) + w10 + sigma0(w2));
    Round(g, h, a, b, c, d, e, f, 0x0fc19dc6, w2 += sigma1(w0) + w11 + sigma0(w3));
    Round(f, g, h, a, b, c, d, e, 0x240ca1cc, w3 += sigma1(w1) + w12 + sigma0(w4));
    Round(e, f, g, h, a, b, c, d, 0x2de92c6f, w4 += sigma1(w2) + w13 + sigma0(w5));
    Round(d, e, f, g, h, a, b, c, 0x4a7484aa, w5 += sigma1(w3) + w14 + sigma0(w6));
    Round(c, d, e, f, g, h, a, b, 0x5cb0a9dc, w6 += sigma1(w4) + w15 + sigma0(w7));
    Round(b, c, d, e, f, g, h, a, 0x76f988da, w7 += sigma1(w5) + w0 + sigma0(w8));
    Round(a, b, c, d, e, f, g, h, 0x983e5152, w8 += sigma1(w6) + w1 + sigma0(w9));
    Round(h, a, b, c, d, e, f, g, 0xa831c66d, w9 += sigma1(w7) + w2 + sigma0(w10));
    Round(g, h, a, b, c, d, e, f, 0xb00327c8, w10 += sigma1(w8) + w3 + sigma0(w11));
    Round(f, g, h, a, b, c, d, e, 0xbf597fc7, w11 += sigma1(w9) + w4 + sigma0(w12));
    Round(e, f, g, h, a, b, c, d, 0xc6e00bf3, w12 += sigma1(w10) + w5 + sigma0(w13));
    Round(d, e, f, g, h, a, b, c, 0xd5a79147, w13 += sigma1(w11) + w6 + sigma0(w14));
    Round(c, d, e, f, g, h, a, b, 0x06ca6351, w14 += sigma1(w12) + w7 + sigma0(w15));
    Round(b, c, d, e, f, g, h, a, 0x14292967, w15 += sigma1(w13) + w8 + sigma0(w0));

    Round(a, b, c, d, e, f, g, h, 0x27b70a85, w0 += sigma1(w14) + w9 + sigma0(w1));
    Round(h, a, b, c, d, e, f, g, 0x2e1b2138, w1 += sigma1(w15) + w10 + sigma0(w2));
    Round(g, h, a, b, c, d, e, f, 0x4d2c6dfc, w2 += sigma1(w0) + w11 + sigma0(w3));
    Round(f, g, h, a, b, c, d, e, 0x53380d13, w3 += sigma1(w1) + w12 + sigma0(w4));
    Round(e, f, g, h, a, b, c, d, 0x650a7354, w4 += sigma1(w2) + w13 + sigma0(w5));
    Round(d, e, f, g, h, a, b, c, 0x766a0abb, w5 += sigma1(w3) + w14 + sigma0(w6));
    Round(c, d, e, f, g, h, a, b, 0x81c2c92e, w6 += sigma1(w4) + w15 + sigma0(w7));
    Round(b, c, d, e, f, g, h, a, 0x92722c85, w7 += sigma1(w5) + w0 + sigma0(w8));
    Round(a, b, c, d, e, f, g, h, 0xa2bfe8a1, w8 += sigma1(w6) + w1 + sigma0(w9));
    Round(h, a, b, c, d, e, f, g, 0xa81a664b, w9 += sigma1(w7) + w2 + sigma0(w10));
    Round(g, h, a, b, c, d, e, f, 0xc24b8b70, w10 += sigma1(w8) + w3 + sigma0(w11));
    Round(f, g, h, a, b, c, d, e, 0xc76c51a3, w11 += sigma1(w9) + w4 + sigma0(w12));
    Round(e, f, g, h, a, b, c, d, 0xd192e819, w12 += sigma1(w10) + w5 + sigma0(w13));
    Round(d, e, f, g, h, a, b, c, 0xd6990624, w13 += sigma1(w11) + w6 + sigma0(w14));
    Round(c, d, e, f, g, h, a, b, 0xf40e3585, w14 += sigma1(w12) + w7 + sigma0(w15));
    Round(b, c, d, e, f, g, h, a, 0x106aa070, w15 += sigma1(w13) + w8 + sigma0(w0));

    Round(a, b, c, d, e, f, g, h, 0x19a4c116, w0 += sigma1(w14) + w9 + sigma0(w1));
    Round(h, a, b, c, d, e, f, g, 0x1e376c08, w1 += sigma1(w15) + w10 + sigma0(w2));
    Round(g, h, a, b, c, d, e, f, 0x2748774c, w2 += sigma1(w0) + w11 + sigma0(w3));
    Round(f, g, h, a, b, c, d, e, 0x34b0bcb5, w3 += sigma1(w1) + w12 + sigma0(w4));
    Round(e, f, g, h, a, b, c, d, 0x391c0cb3, w4 += sigma1(w2) + w13 + sigma0(w5));
    Round(d, e, f, g, h, a, b, c, 0x4ed8aa4a, w5 += sigma1(w3) + w14 + sigma0(w6));
    Round(c, d, e, f, g, h, a, b, 0x5b9cca4f, w6 += sigma1(w4) + w15 + sigma0(w7));
    Round(b, c, d, e, f, g, h, a, 0x682e6ff3, w7 += sigma1(w5) + w0 + sigma0(w8));
    Round(a, b, c, d, e, f, g, h, 0x748f82ee, w8 += sigma1(w6) + w1 + sigma0(w9));
    Round(h, a, b, c, d, e, f, g, 0x78a5636f, w9 += sigma1(w7) + w2 + sigma0(w10));
    Round(g, h, a, b, c, d, e, f, 0x84c87814, w10 += sigma1(w8) + w3 + sigma0(w11));
    Round(f, g, h, a, b, c, d, e, 0x8cc70208, w11 += sigma1(w9) + w4 + sigma0(w12));
    Round(e, f, g, h, a, b, c, d, 0x90befffa, w12 += sigma1(w10) + w5 + sigma0(w13));
    Round(d, e, f, g, h, a, b, c, 0xa4506ceb, w13 += sigma1(w11) + w6 + sigma0(w14));
    Round(c, d, e, f, g, h, a, b, 0xbef9a3f7, w14 + sigma1(w12) + w7 + sigma0(w15));
    Round(b, c, d, e, f, g, h, a, 0xc67178f2, w15 + sigma1(w13) + w8 + sigma0(w0));

    s[0] += a;
    s[1] += b;
    s[2] += c;
    s[3] += d;
    s[4] += e;
    s[5] += f;
    s[6] += g;
    s[7] += h;
}

void secp256k1_sha256_write(secp256k1_sha256_t *hash, const unsigned char *data, size_t len) {
    size_t bufsize = hash->bytes & 0x3F;
    hash->bytes += len;
    while (bufsize + len >= 64) {
        /* Fill the buffer, and process it. */
        memcpy(((unsigned char*)hash->buf) + bufsize, data, 64 - bufsize);
        data += 64 - bufsize;
        len -= 64 - bufsize;
        secp256k1_sha256_transform(hash->s, hash->buf);
        bufsize = 0;
    }
    if (len) {
        /* Fill the buffer with what remains. */
        memcpy(((unsigned char*)hash->buf) + bufsize, data, len);
    }
}

void secp256k1_sha256_finalize(secp256k1_sha256_t *hash, unsigned char *out32) {
    static const unsigned char pad[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t sizedesc[2];
    uint32_t out[8];
    int i = 0;
    sizedesc[0] = BE32(hash->bytes >> 29);
    sizedesc[1] = BE32(hash->bytes << 3);
    secp256k1_sha256_write(hash, pad, 1 + ((119 - (hash->bytes % 64)) % 64));
    secp256k1_sha256_write(hash, (const unsigned char*)sizedesc, 8);
    for (i = 0; i < 8; i++) {
        out[i] = BE32(hash->s[i]);
        hash->s[i] = 0;
    }
    memcpy(out32, (const unsigned char*)out, 32);
}

void secp256k1_hmac_sha256_initialize(secp256k1_hmac_sha256_t *hash, const unsigned char *key, size_t keylen) {
    int n;
    unsigned char rkey[64];
    if (keylen <= 64) {
        memcpy(rkey, key, keylen);
        memset(rkey + keylen, 0, 64 - keylen);
    } else {
        secp256k1_sha256_t sha256;
        secp256k1_sha256_initialize(&sha256);
        secp256k1_sha256_write(&sha256, key, keylen);
        secp256k1_sha256_finalize(&sha256, rkey);
        memset(rkey + 32, 0, 32);
    }

    secp256k1_sha256_initialize(&hash->outer);
    for (n = 0; n < 64; n++) {
        rkey[n] ^= 0x5c;
    }
    secp256k1_sha256_write(&hash->outer, rkey, 64);

    secp256k1_sha256_initialize(&hash->inner);
    for (n = 0; n < 64; n++) {
        rkey[n] ^= 0x5c ^ 0x36;
    }
    secp256k1_sha256_write(&hash->inner, rkey, 64);
    memset(rkey, 0, 64);
}

void secp256k1_hmac_sha256_write(secp256k1_hmac_sha256_t *hash, const unsigned char *data, size_t size) {
    secp256k1_sha256_write(&hash->inner, data, size);
}

void secp256k1_hmac_sha256_finalize(secp256k1_hmac_sha256_t *hash, unsigned char *out32) {
    unsigned char temp[32];
    secp256k1_sha256_finalize(&hash->inner, temp);
    secp256k1_sha256_write(&hash->outer, temp, 32);
    memset(temp, 0, 32);
    secp256k1_sha256_finalize(&hash->outer, out32);
}


void secp256k1_rfc6979_hmac_sha256_initialize(secp256k1_rfc6979_hmac_sha256_t *rng, const unsigned char *key, size_t keylen) {
    secp256k1_hmac_sha256_t hmac;
    static const unsigned char zero[1] = {0x00};
    static const unsigned char one[1] = {0x01};

    memset(rng->v, 0x01, 32); /* RFC6979 3.2.b. */
    memset(rng->k, 0x00, 32); /* RFC6979 3.2.c. */

    /* RFC6979 3.2.d. */
    secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
    secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
    secp256k1_hmac_sha256_write(&hmac, zero, 1);
    secp256k1_hmac_sha256_write(&hmac, key, keylen);
    secp256k1_hmac_sha256_finalize(&hmac, rng->k);
    secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
    secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
    secp256k1_hmac_sha256_finalize(&hmac, rng->v);

    /* RFC6979 3.2.f. */
    secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
    secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
    secp256k1_hmac_sha256_write(&hmac, one, 1);
    secp256k1_hmac_sha256_write(&hmac, key, keylen);
    secp256k1_hmac_sha256_finalize(&hmac, rng->k);
    secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
    secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
    secp256k1_hmac_sha256_finalize(&hmac, rng->v);
    rng->retry = 0;
}

void secp256k1_rfc6979_hmac_sha256_generate(secp256k1_rfc6979_hmac_sha256_t *rng, unsigned char *out, size_t outlen) {
    /* RFC6979 3.2.h. */
    static const unsigned char zero[1] = {0x00};
    if (rng->retry) {
        secp256k1_hmac_sha256_t hmac;
        secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
        secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
        secp256k1_hmac_sha256_write(&hmac, zero, 1);
        secp256k1_hmac_sha256_finalize(&hmac, rng->k);
        secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
        secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
        secp256k1_hmac_sha256_finalize(&hmac, rng->v);
    }

    while (outlen > 0) {
        secp256k1_hmac_sha256_t hmac;
        int now = outlen;
        secp256k1_hmac_sha256_initialize(&hmac, rng->k, 32);
        secp256k1_hmac_sha256_write(&hmac, rng->v, 32);
        secp256k1_hmac_sha256_finalize(&hmac, rng->v);
        if (now > 32) {
            now = 32;
        }
        memcpy(out, rng->v, now);
        out += now;
        outlen -= now;
    }

    rng->retry = 1;
}

void secp256k1_rfc6979_hmac_sha256_finalize(secp256k1_rfc6979_hmac_sha256_t *rng) {
    memset(rng->k, 0, 32);
    memset(rng->v, 0, 32);
    rng->retry = 0;
}

#undef BE32
#undef Round
#undef sigma1
#undef sigma0
#undef Sigma1
#undef Sigma0
#undef Maj
#undef Ch
///end of secp256k1/src/hash_impl.h






///from [autogenerated]ecmult_static_context.h
#ifdef USE_ECMULT_STATIC_PRECOMPUTATION
#define SC SECP256K1_GE_STORAGE_CONST
static const secp256k1_ge_storage secp256k1_ecmult_static_context[64][16] = {
{
    SC(983487347u, 1861041900u, 2599115456u, 565528146u, 1451326239u, 148794576u, 4224640328u, 3120843701u, 2076989736u, 3184115747u, 3754320824u, 2656004457u, 2876577688u, 2388659905u, 3527541004u, 1170708298u),
    SC(3830281845u, 3284871255u, 1309883393u, 2806991612u, 1558611192u, 1249416977u, 1614773327u, 1353445208u, 633124399u, 4264439010u, 426432620u, 167800352u, 2355417627u, 2991792291u, 3042397084u, 505150283u),
    SC(1792710820u, 2165839471u, 3876070801u, 3603801374u, 2437636273u, 1231643248u, 860890267u, 4002236272u, 3258245037u, 4085545079u, 2695347418u, 288209541u, 484302592u, 139267079u, 14621978u, 2750167787u),
    SC(11094760u, 1663454715u, 3104893589u, 1290390142u, 1334245677u, 2671416785u, 3982578986u, 2050971459u, 2136209393u, 1792200847u, 367473428u, 114820199u, 1096121039u, 425028623u, 3983611854u, 923011107u),
    SC(3063072592u, 3527226996u, 3276923831u, 3785926779u, 346414977u, 2234429237u, 547163845u, 1847763663u, 2978762519u, 623753375u, 2207114031u, 3006533282u, 3147176505u, 1421052999u, 4188545436u, 1210097867u),
    SC(1763690305u, 2162645845u, 1202943473u, 469109438u, 1159538654u, 390308918u, 1603161004u, 2906790921u, 2394291613u, 4089459264u, 1827402608u, 2166723935u, 3207526147u, 1197293526u, 436375990u, 1773481373u),
    SC(1882939156u, 2815960179u, 295089455u, 2929502411u, 2990911492u, 2056857815u, 3502518067u, 957616604u, 1591168682u, 1240626880u, 1298264859u, 1839469436u, 3185927997u, 2386526557u, 4025121105u, 260593756u),
    SC(699984967u, 3527536033u, 3843799838u, 958940236u, 927446270u, 2095887205u, 733758855u, 793790581u, 2288595512u, 2237855935u, 4158071588u, 103726164u, 1804839263u, 2006149890u, 3944535719u, 3558448075u),
    SC(1145702317u, 3893958345u, 851308226u, 566580932u, 1803510929u, 244954233u, 754894895u, 1321302288u, 772295727u, 4004336128u, 2009158070u, 4026087258u, 1899732245u, 1392930957u, 3019192545u, 149625039u),
    SC(3772604811u, 577564124u, 4116730494u, 548732504u, 241159976u, 965811878u, 3286803623u, 3781136673u, 2690883927u, 863484863u, 463101630u, 2948469162u, 1712070245u, 3742601912u, 2535479384u, 1015456764u),
    SC(2610513434u, 780361970u, 4072278968u, 3165566617u, 362677842u, 1775830058u, 4195110448u, 2813784845u, 1072168452u, 1018450691u, 1028609376u, 2101464438u, 2419500187u, 2190549840u, 1837865365u, 625038589u),
    SC(1347265449u, 3654928411u, 3255194520u, 1322421425u, 3049188507u, 1827004342u, 3467202132u, 4261348427u, 3419671838u, 2239837129u, 3441474020u, 268041876u, 4157379961u, 971431753u, 2053887746u, 2038404815u),
    SC(3723233964u, 515696298u, 2908946645u, 1626400003u, 2191461318u, 1201029625u, 186243574u, 1212380923u, 858781105u, 4236445790u, 1936144063u, 1009147468u, 2407567966u, 1865959325u, 1701035060u, 241151649u),
    SC(3696430315u, 3089900654u, 1103438577u, 3528924465u, 1259662835u, 2438429227u, 1692672370u, 2989843137u, 1446894995u, 2239587625u, 2340544036u, 434491102u, 128239031u, 2734594294u, 2667284742u, 1865591178u),
    SC(1980028431u, 1099813170u, 2013628738u, 4214038867u, 3231891435u, 3896266769u, 2756820145u, 1490749299u, 951981230u, 3655451652u, 1676645053u, 3593230746u, 3010864552u, 405419875u, 1336073872u, 1398624425u),
    SC(3414779716u, 2008156201u, 4125277506u, 2287126283u, 2446053551u, 212726297u, 2794923956u, 3421277562u, 1460719994u, 552209919u, 2551004934u, 953727248u, 3096710400u, 3712627263u, 3614955842u, 557715603u)
},
{
    SC(461660907u, 483260338u, 3090624303u, 3468817529u, 2869411999u, 3408320195u, 157674611u, 1298485121u, 103769941u, 3030878493u, 1440637991u, 4223892787u, 3840844824u, 2730509202u, 2748389383u, 214732837u),
    SC(4143598594u, 459898515u, 2922648667u, 1209678535u, 1176716252u, 1612841999u, 2202917330u, 13015901u, 1575346251u, 891263272u, 3091905261u, 3543244385u, 3935435865u, 2372913859u, 1075649255u, 201888830u),
    SC(3481295448u, 3640243220u, 2859715852u, 3846556079u, 1065182531u, 2330293896u, 2178091110u, 3893510868u, 4099261975u, 2577582684u, 4207143791u, 589834100u, 2090766670u, 4242818989u, 2413240316u, 1338191979u),
    SC(1222367653u, 2295459885u, 1856816550u, 918616911u, 3733449540u, 288527426u, 308654335u, 175301747u, 2585816357u, 1572985110u, 3820086017u, 3400646033u, 3928615806u, 2543586180u, 1619974000u, 1257448912u),
    SC(3467862907u, 681146163u, 2909728989u, 83906098u, 2626131995u, 3872919971u, 2290108548u, 1697087700u, 1793941143u, 3236443826u, 1940064764u, 1563989881u, 527371209u, 610869743u, 1604941439u, 3670721525u),
    SC(2302729378u, 1391194029u, 1641771531u, 3876177737u, 1929557473u, 2752989331u, 2519109900u, 1131448856u, 3786463166u, 506905989u, 2345013855u, 2144715811u, 1583628159u, 291930150u, 3243470493u, 4130365181u),
    SC(2855519179u, 3147287790u, 1536116015u, 1784764672u, 959281397u, 3099717666u, 86403980u, 3409201632u, 3921301684u, 2101228153u, 575924517u, 1382150904u, 641478876u, 3860064926u, 1937521554u, 2358132463u),
    SC(972265053u, 3025511526u, 2467192450u, 4011934802u, 4015820825u, 3179306985u, 1744647725u, 423238442u, 2406064939u, 901607195u, 3316491016u, 4128592049u, 1397491632u, 439641584u, 90500461u, 2834580417u),
    SC(1730532518u, 2821193463u, 2700804628u, 2416923244u, 3795632308u, 2799866320u, 3434703577u, 3883111373u, 1777933228u, 2963254493u, 3042948878u, 1746288680u, 2832145340u, 544625602u, 3633879343u, 2300858165u),
    SC(62331695u, 2228442612u, 3527845246u, 2989876118u, 3995298903u, 3601545798u, 4170931516u, 445717530u, 1981201926u, 94264130u, 2668647577u, 953251412u, 3322279962u, 3837653687u, 3116466555u, 3369796531u),
    SC(2739333573u, 3637259489u, 443756582u, 825678124u, 2455706402u, 2994548791u, 3653546249u, 2584145078u, 1245698352u, 89066746u, 1738138166u, 2916153640u, 1850062717u, 3472193431u, 2110631011u, 1214009088u),
    SC(2386327178u, 3993497770u, 1051345891u, 4137183237u, 3078790224u, 3598213568u, 3344610192u, 1517270932u, 869515922u, 2057215060u, 2792454282u, 4228826509u, 3425305972u, 2708629086u, 880185559u, 1356729037u),
    SC(2989561710u, 3550122639u, 1990591383u, 2036612756u, 3588709655u, 595888062u, 4189293408u, 1955008963u, 987876526u, 542093629u, 1953520395u, 2315684331u, 2929815182u, 3270759899u, 393611756u, 1677885197u),
    SC(2331762734u, 371120497u, 1141333410u, 3466824114u, 4113916626u, 3698793791u, 2483365276u, 4265751258u, 3804325409u, 4085909553u, 3531838853u, 2629626707u, 625187055u, 3045263564u, 198131065u, 3993694760u),
    SC(27419592u, 3267954699u, 2966738458u, 3143461717u, 3869766944u, 2163162934u, 1886283869u, 2052225367u, 958768216u, 2006727717u, 2069130137u, 1939449196u, 3015752138u, 258766841u, 3290132621u, 4163970366u),
    SC(903383785u, 2983456345u, 4269392462u, 3731664159u, 1837248343u, 1888413004u, 652691803u, 897487558u, 3732206419u, 3625013640u, 1917594162u, 967935585u, 1804564817u, 883883125u, 2389854768u, 2347234078u)
},
{
    SC(1793692126u, 406948681u, 23075151u, 2805328754u, 3264854407u, 427926777u, 2859563730u, 198037267u, 2129133850u, 1089701106u, 3842694445u, 2533380467u, 663211132u, 2312829798u, 807127373u, 38506815u),
    SC(571890638u, 3882751380u, 1536180709u, 3437159763u, 3953528399u, 516828575u, 3769463872u, 1449076325u, 4270798907u, 3135758980u, 3520630973u, 1452980147u, 3957715387u, 3054428574u, 2391664223u, 2297670550u),
    SC(2724204046u, 2456139736u, 265045669u, 1367810338u, 1722635060u, 1306450931u, 2894913322u, 3094293390u, 3490680992u, 2550020195u, 3028635086u, 4200216295u, 1066664286u, 4170330175u, 777827015u, 183484181u),
    SC(947228665u, 1559209921u, 3080864826u, 3123295835u, 2934045313u, 1590990229u, 2766960143u, 3113606941u, 1136432319u, 3758046297u, 2054046144u, 1377389889u, 3244301201u, 127071274u, 1752358610u, 2783507663u),
    SC(1460807831u, 3649051054u, 2799484569u, 1231562901u, 3377754600u, 3577118892u, 1234337315u, 380370215u, 3272388869u, 3656237932u, 2653126291u, 786263023u, 1028996455u, 4274234235u, 4225822550u, 10734444u),
    SC(2071087047u, 1934036755u, 611830132u, 2015415885u, 1373497691u, 3709112893u, 3810392851u, 1519037663u, 779113716u, 2738053543u, 2754096050u, 2121500804u, 982626833u, 1064427872u, 1627071029u, 1799421889u),
    SC(490669665u, 331510235u, 927653097u, 4010558541u, 1341899186u, 2739641489u, 1436050289u, 1379364712u, 441190387u, 3816107121u, 4151493979u, 3530159022u, 2848669857u, 2894763699u, 1938279708u, 3206735972u),
    SC(1164630680u, 735028522u, 1426163473u, 1764145219u, 2188722839u, 2599797011u, 2331123230u, 996298865u, 2803113036u, 1732133918u, 4135374745u, 1403496102u, 61305906u, 1982207767u, 35608603u, 680731708u),
    SC(3097030574u, 2239944926u, 3004506636u, 3698971324u, 438440050u, 806226289u, 3299217652u, 2137747676u, 2376642592u, 2372355096u, 1444993877u, 4198291752u, 3194432604u, 579432496u, 3143260503u, 58153128u),
    SC(3073570790u, 2457870973u, 3254087300u, 132589961u, 3090464363u, 4031655485u, 3397735349u, 3738272915u, 2438408586u, 1610016484u, 3607490511u, 1979839295u, 1993157220u, 1628966973u, 2566520843u, 2415504793u),
    SC(2516700697u, 2521039798u, 2777488721u, 3196543385u, 3593950703u, 2445108602u, 4227515375u, 3361503440u, 3741757104u, 1367007706u, 4282009789u, 2127358709u, 2970274265u, 108953332u, 1376097231u, 3612352600u),
    SC(2841122028u, 289695603u, 908429972u, 1449591303u, 3496532142u, 430811028u, 1377898285u, 198605765u, 702014643u, 1582973696u, 1654127041u, 4145703462u, 294032334u, 4235431914u, 3438393459u, 865474483u),
    SC(3545445168u, 3333415739u, 2928811023u, 1435493501u, 3112072977u, 3466119300u, 61597844u, 839813414u, 3787328278u, 1928915478u, 3046796186u, 549615137u, 3862451403u, 1325262296u, 3520760105u, 1333228419u),
    SC(1325790793u, 3907821545u, 4134901119u, 1951705246u, 3223387882u, 561480379u, 1136389443u, 2963679361u, 3722857515u, 626885912u, 3665060294u, 2975869036u, 1378007717u, 1212143055u, 3672021732u, 2520983812u),
    SC(436660944u, 1593040065u, 2835874356u, 3054866405u, 1746716106u, 2901130226u, 3275156703u, 889550475u, 1667636846u, 2171317649u, 477876339u, 169193861u, 3301423024u, 2923695575u, 1084572294u, 981889567u),
    SC(3803276281u, 4055280968u, 3904809427u, 186227966u, 932166956u, 2399165660u, 3851784532u, 3001852135u, 813014380u, 4116676373u, 2706810629u, 527442580u, 120296772u, 3128162880u, 662936789u, 1729392771u)
},
{
    SC(1686440452u, 1988561476u, 754604000u, 1313277943u, 3972816537u, 316394247u, 994407191u, 1904170630u, 2086644946u, 2443632379u, 2709748921u, 1003213045u, 3157743406u, 1758245536u, 3227689301u, 1181052876u),
    SC(1258105424u, 4154135555u, 2219123623u, 3901620566u, 4152326230u, 2255006844u, 2043811343u, 3401743053u, 1077175625u, 4217078864u, 23446180u, 3296093630u, 2983403379u, 483875022u, 1821322007u, 933769937u),
    SC(4094896192u, 2631249021u, 2047644402u, 1580854103u, 3103587285u, 3577080832u, 2726417365u, 309664155u, 1801899606u, 2578001137u, 150353312u, 1950478529u, 895600852u, 3405805048u, 2316670682u, 3067768105u),
    SC(443311827u, 441757202u, 1505167796u, 3339695156u, 4080303377u, 2032258566u, 4249816510u, 3524388084u, 3057881006u, 1951550910u, 755229308u, 2331249069u, 1739558582u, 2222328965u, 511821487u, 2764767310u),
    SC(989753134u, 2338266356u, 549068233u, 4113024610u, 2746193091u, 2634969710u, 3079940655u, 3384912157u, 143838693u, 4047635856u, 4286586687u, 149695182u, 1777393012u, 52209639u, 2932952119u, 3267437714u),
    SC(682610480u, 2717190045u, 3874701500u, 2657184992u, 2055845501u, 1316949440u, 1867841182u, 3514766617u, 3083609836u, 2586162565u, 866399081u, 1085717952u, 3259379257u, 575055971u, 3866877694u, 451222497u),
    SC(328731030u, 2942825188u, 1841689481u, 3492191519u, 967390237u, 99172838u, 3036642267u, 3931425637u, 933459735u, 3523655044u, 2662830483u, 2533317360u, 1151283556u, 1285468956u, 15891850u, 3194406721u),
    SC(3082245252u, 2305218459u, 2853219703u, 1279555698u, 3695999195u, 2225441691u, 2702374346u, 2002979755u, 3394310641u, 1438568303u, 441738339u, 2319547123u, 745721770u, 3663132780u, 3613740038u, 3163545587u),
    SC(3109530474u, 209548946u, 1705898345u, 1227555051u, 1300903197u, 521706788u, 1046889791u, 392785355u, 1195852439u, 1128202903u, 589172095u, 3844020294u, 989062243u, 3765536158u, 3601935109u, 563198009u),
    SC(1408383323u, 2941773350u, 4185382573u, 3662857379u, 4172908289u, 4118722458u, 1935569844u, 1296819381u, 439467796u, 917888253u, 1573015538u, 2875181025u, 22626495u, 313409715u, 121133518u, 1579603291u),
    SC(838355261u, 2323744266u, 929233883u, 1533162328u, 2939669145u, 1021427197u, 2448693967u, 1568998094u, 455286333u, 2516902543u, 1708158744u, 278073872u, 978123683u, 2512836694u, 3972232382u, 1433020779u),
    SC(2010810703u, 4018381427u, 571706262u, 1692351234u, 4256546562u, 1231266051u, 268479287u, 2820752911u, 2261632188u, 845795375u, 3555293251u, 4247559674u, 3383569817u, 4149228066u, 180667610u, 1402241180u),
    SC(3525485702u, 3451430050u, 2349871300u, 60510511u, 4165534527u, 3431222792u, 4244473672u, 526926602u, 763199050u, 672899723u, 1978849638u, 489006191u, 1575850086u, 1948428588u, 201110001u, 2038136322u),
    SC(3829603224u, 567257667u, 2324557421u, 3080821304u, 1922441927u, 1741539649u, 2023385976u, 3349327437u, 1997432110u, 3734074051u, 1330703636u, 3180299184u, 1913578229u, 141656008u, 2692604045u, 1602929664u),
    SC(29051889u, 27392875u, 2013870801u, 1608859787u, 4192290684u, 944038467u, 2706126772u, 4086572363u, 3654238115u, 631287070u, 4277765317u, 2361271762u, 4170133585u, 2022489410u, 2834765713u, 1378478404u),
    SC(2835113470u, 3839837803u, 3596950757u, 2129670392u, 1881028173u, 4057879348u, 2459142230u, 3736551989u, 3032996358u, 1333513239u, 3006303259u, 3885122327u, 4228039994u, 134788219u, 3631677646u, 450886807u)
},
{
    SC(2450731413u, 2768047193u, 2114778718u, 2363611449u, 3811833768u, 1142236074u, 836975073u, 719658637u, 89564040u, 2055034782u, 2279505737u, 2354364196u, 748992674u, 2341838369u, 3471590741u, 3103440079u),
    SC(464369172u, 1784969737u, 2303680721u, 1699015058u, 1839678160u, 53342253u, 3929309179u, 3713202491u, 1764215120u, 2190365769u, 3137266333u, 3919018972u, 3446276485u, 1027535494u, 3649392538u, 1979045036u),
    SC(3689697965u, 1535268856u, 4095087266u, 1879342666u, 1901613989u, 4062220771u, 1231692686u, 3479254943u, 517178359u, 3704348661u, 3200159500u, 592930780u, 3995209641u, 2367381241u, 1790597847u, 2276745810u),
    SC(1563410665u, 2779883331u, 320555798u, 143478861u, 1984047202u, 2486036480u, 1819096593u, 876845470u, 4160262809u, 1685665332u, 1096211683u, 3396846267u, 1079209808u, 1622135728u, 2746449213u, 2258485533u),
    SC(1981422448u, 2212169687u, 873443773u, 3576733408u, 3923794933u, 1875069523u, 3053667173u, 4292418240u, 2192702144u, 1027092432u, 278807989u, 2315740043u, 485097259u, 4099751129u, 1350843241u, 1137138810u),
    SC(3929635582u, 2647315129u, 1255145681u, 2059161179u, 1939751218u, 2574940312u, 1013734441u, 3958841903u, 615021475u, 1092396560u, 1516857705u, 4167743313u, 744612233u, 1609870616u, 1905505775u, 2106400820u),
    SC(1036005687u, 2272703162u, 2208830030u, 2182996589u, 441615709u, 3591433922u, 3586649797u, 164179585u, 3077875769u, 1792522157u, 2657252843u, 657567108u, 656390324u, 1816007391u, 3075467586u, 3873231707u),
    SC(1236896749u, 2895887291u, 1978987518u, 822801819u, 516389325u, 1102535042u, 1787993035u, 3557481093u, 3231661433u, 991180576u, 3686912074u, 1297456949u, 3327185778u, 308709174u, 495078044u, 2969592590u),
    SC(2019907021u, 744703189u, 2139199843u, 518542186u, 3124680574u, 142934434u, 551498542u, 3021773546u, 4091561632u, 1051317147u, 825719313u, 3707224763u, 335483791u, 4028731434u, 1335000639u, 4102709448u),
    SC(1093818871u, 985937516u, 327542691u, 2046117782u, 1264752065u, 697293694u, 1615263505u, 1156460629u, 2812684388u, 1192736815u, 3019618111u, 4209127823u, 2556369187u, 2112950523u, 637809851u, 2176824541u),
    SC(1687299893u, 3728297084u, 490922479u, 3634470646u, 250826345u, 3692215527u, 3273717576u, 965983458u, 2226919381u, 1460789800u, 2122435754u, 2519058236u, 1620196106u, 4066817802u, 1130044433u, 3889340415u),
    SC(852530522u, 3312783835u, 1596416107u, 1741549460u, 2684468674u, 3424816114u, 2501858342u, 1775689041u, 2140910620u, 3593295971u, 3269455071u, 2386348485u, 3506744308u, 1454965514u, 1429132807u, 1936823584u),
    SC(606602909u, 3019871883u, 3512048756u, 3287518999u, 3877975051u, 3914786486u, 3870177904u, 1340649290u, 520571284u, 3028797996u, 2616337132u, 1103844529u, 3133726039u, 1357152000u, 1508799653u, 31330228u),
    SC(2817743510u, 2877820134u, 3034826170u, 1694674814u, 3472934533u, 2992700940u, 940570741u, 734740020u, 2101869811u, 3976806699u, 3986671415u, 556491401u, 2336314226u, 3375597171u, 2706276162u, 2068498899u),
    SC(2875346415u, 3996130283u, 2530370154u, 2292821435u, 1717542531u, 4166402291u, 2045046397u, 210928306u, 1305773764u, 891667924u, 1720475570u, 2097400197u, 3748242244u, 1645769622u, 3986372109u, 4259524466u),
    SC(258680563u, 3407077353u, 3701760456u, 1531445568u, 3746171918u, 2983392727u, 1490964851u, 3947644742u, 2779475335u, 3867487462u, 2573576052u, 3434694262u, 2755711440u, 3366989652u, 566303708u, 3091229946u)
},
{
    SC(2925506593u, 3911544000u, 1647760999u, 3077282783u, 810174083u, 3532746750u, 1218244633u, 1800164995u, 3882366571u, 1552758454u, 417617232u, 3581187042u, 1107218813u, 308444727u, 2996521844u, 3546298006u),
    SC(2766498247u, 1567718891u, 906631353u, 1539374134u, 2937267715u, 3075423755u, 466239025u, 348294756u, 2802746156u, 3822638356u, 2215866298u, 2676073175u, 2327206803u, 3701444736u, 533673746u, 1949565232u),
    SC(779912645u, 2120746348u, 3775586707u, 1719694388u, 3225985094u, 1124933767u, 2466028289u, 3688916232u, 2352972448u, 3100332066u, 3699049642u, 105143046u, 3528587757u, 3202351686u, 3275195870u, 2542878955u),
    SC(4208701680u, 3032319563u, 1934783333u, 1683344422u, 1898255641u, 1818484420u, 1090856325u, 4203146066u, 3166734039u, 1425051511u, 411614967u, 1272168350u, 905464202u, 2860309946u, 2899721999u, 4016531256u),
    SC(1252276677u, 705548877u, 3321309972u, 2587486609u, 1841091772u, 1176108340u, 2483104333u, 1124739854u, 1417860124u, 2145011089u, 1095816787u, 561231448u, 3047186502u, 2188442748u, 782343512u, 2073487869u),
    SC(773625401u, 1399854511u, 2112273744u, 3798562401u, 2328245221u, 4053035765u, 884849756u, 2543299151u, 3064173848u, 3322400978u, 2493736578u, 4109781307u, 3356431908u, 2033183790u, 3916558464u, 937192909u),
    SC(1676839026u, 1837563838u, 681907940u, 1979087218u, 3861274680u, 1004821519u, 3526269549u, 3587326189u, 4130121750u, 5876755u, 277168273u, 3347389376u, 1295078502u, 3055055655u, 988901279u, 1750695367u),
    SC(1466696987u, 793586382u, 3395028606u, 688541023u, 227515247u, 433349930u, 1151320534u, 2638968365u, 2730052118u, 2419949779u, 4184196159u, 3075595332u, 1762597117u, 3208522231u, 3793454426u, 2205574333u),
    SC(2271935805u, 2221340650u, 4006866556u, 3892925071u, 3300102857u, 4023132062u, 1966820825u, 193229358u, 3829742367u, 3288127030u, 2999566231u, 1746318860u, 611198282u, 1740582489u, 586692015u, 272371975u),
    SC(1512874083u, 1683202061u, 3100471136u, 875884760u, 2252521753u, 3056609126u, 2397470151u, 3238829627u, 398340158u, 1086173909u, 2650682699u, 3851040891u, 267796754u, 1063916466u, 134772391u, 616879617u),
    SC(1190901836u, 3498895828u, 121518848u, 4122627266u, 4044339275u, 3929319666u, 3725675569u, 2249645810u, 1648430039u, 805152867u, 604009597u, 428134903u, 3660078748u, 1495738811u, 2912743026u, 3529964664u),
    SC(1098872981u, 3803982233u, 1184687675u, 1724685244u, 1166128174u, 3324080552u, 2889006549u, 591614595u, 442372335u, 2188313994u, 392144341u, 559497602u, 2786744839u, 1080958720u, 963196350u, 4153188088u),
    SC(2439538370u, 4080798018u, 3371249236u, 2272355420u, 3780648680u, 116755088u, 1743646150u, 3071185844u, 3348389643u, 3506488228u, 3592742183u, 3935997343u, 3470563636u, 4177761627u, 2879753187u, 203653531u),
    SC(3278048310u, 2898758456u, 2355004932u, 2165371155u, 909690763u, 4208028121u, 3529336571u, 120122699u, 1468577489u, 2088039937u, 3804192119u, 4005659309u, 496708233u, 114985314u, 4186471387u, 1516837088u),
    SC(1694326758u, 3482448156u, 2533790413u, 3535432659u, 1293417127u, 2007819995u, 3512854075u, 2476797465u, 936262398u, 4149678787u, 807292055u, 1683402105u, 3767740082u, 682769936u, 2956180563u, 2800734304u),
    SC(804843744u, 1565609957u, 1986774659u, 4163563545u, 1192892219u, 2967653559u, 1407927717u, 134508609u, 2584983666u, 3798685912u, 1759632157u, 1938927553u, 3974685712u, 2763800386u, 3702401831u, 3969543832u)
},
{
    SC(2016238746u, 3648008750u, 3741265531u, 1468285316u, 3314132186u, 3225615603u, 2260838904u, 650230459u, 666608997u, 1079817106u, 1685466519u, 3417306450u, 465799968u, 1454367507u, 1432699603u, 4060146438u),
    SC(1761873323u, 2175765323u, 123320402u, 1086415744u, 3425420197u, 3163463272u, 2096010147u, 892174421u, 3834451486u, 191088153u, 650609666u, 1384830375u, 430440180u, 1275312435u, 936713210u, 3964237847u),
    SC(3490530311u, 4154571850u, 1473147571u, 1874492814u, 3394183939u, 690761407u, 1765847887u, 4254640890u, 3957252213u, 852293459u, 403059579u, 1419995731u, 373422976u, 1691953324u, 1513498303u, 3782064719u),
    SC(2587537765u, 1727580331u, 2067598687u, 2050934719u, 1018600463u, 825517190u, 281367288u, 396667874u, 2125440864u, 2142555808u, 3739024155u, 471264185u, 2298783646u, 926505635u, 485317745u, 4237064052u),
    SC(4177694527u, 1331122857u, 2632274962u, 2272030823u, 2711200568u, 493910969u, 64158788u, 2976239616u, 2805230971u, 1856476899u, 706343172u, 883417303u, 3085501222u, 2167885061u, 2608970459u, 1305891290u),
    SC(3887930902u, 1612140391u, 329833229u, 737708613u, 660227298u, 2588285981u, 3429746116u, 4247477263u, 2536670475u, 1091054728u, 1521783433u, 4262529359u, 3261855757u, 453613765u, 484850910u, 3619344637u),
    SC(3635973664u, 4002263582u, 683484955u, 1188525929u, 3024525647u, 1588813480u, 3496033065u, 109022234u, 2342061519u, 1416918501u, 2207158673u, 948640868u, 637445219u, 508491813u, 3897434662u, 680054967u),
    SC(1039851594u, 403130855u, 3868498597u, 1611578944u, 2942424644u, 2874427101u, 1261647069u, 261871566u, 2520758170u, 2840740989u, 3799279215u, 381717039u, 3582347301u, 2025353438u, 2948438214u, 2918501540u),
    SC(81851588u, 3029358979u, 3777821133u, 2109529880u, 3684139703u, 3572137489u, 2624799u, 2076188243u, 53500651u, 2606703535u, 3206313372u, 346558880u, 465806762u, 434266486u, 1902707603u, 4080110534u),
    SC(3612241613u, 1917140707u, 4136616607u, 4041104182u, 2193790431u, 801466537u, 3599423303u, 3561003895u, 1189069231u, 8494816u, 4244955339u, 451969883u, 3908494655u, 517115239u, 1812731691u, 777430858u),
    SC(3522137911u, 2027939004u, 2210696271u, 3920541975u, 875695915u, 2825269477u, 687289812u, 4252564584u, 1824925315u, 507608234u, 2614820601u, 2462525050u, 3886866857u, 668083682u, 2768243607u, 3293579201u),
    SC(1682273922u, 1330912967u, 3636074852u, 840196898u, 1025234484u, 1557176067u, 2837118766u, 3109038869u, 594323342u, 3200796742u, 1959017554u, 1440926582u, 3021668826u, 3738492638u, 446292405u, 2414347832u),
    SC(4116164451u, 4091036540u, 474505628u, 1269644927u, 3643568118u, 1673027873u, 1438360759u, 4022285580u, 4024623082u, 1654730750u, 1581385912u, 3853471495u, 335076979u, 2185560806u, 2494598452u, 3520671279u),
    SC(4099595861u, 2215053464u, 488918654u, 2772869965u, 2247823716u, 1588093320u, 1138185172u, 732569291u, 247618738u, 1702163570u, 1772683376u, 1056938600u, 1997535786u, 2064838561u, 3705150691u, 1453615480u),
    SC(3809909081u, 1962152573u, 3909100601u, 1479514000u, 1615313752u, 3569344372u, 997113509u, 3043376485u, 3480943705u, 4021042580u, 2284195092u, 2749518560u, 3037939132u, 3554704413u, 185068622u, 683070990u),
    SC(3163624176u, 326387389u, 438403431u, 1924575191u, 1706136937u, 2432230714u, 4175139676u, 713582699u, 175432919u, 505729353u, 375905517u, 3179239595u, 2233296987u, 472507277u, 1822318909u, 3059447908u)
},
{
    SC(3955300819u, 2390314746u, 8780989u, 1526705205u, 4147934248u, 1494146575u, 1667625450u, 2277923659u, 406493586u, 957460913u, 3449491434u, 912766689u, 1387230361u, 2368913075u, 3538729245u, 2943257094u),
    SC(2095358020u, 3831852940u, 1752942227u, 477088929u, 2503091779u, 898077u, 2215106688u, 1298885808u, 352224250u, 3952364758u, 3669616566u, 664714721u, 1826685582u, 1576488055u, 2121138397u, 1442223205u),
    SC(1378268686u, 187975558u, 3210161726u, 870689300u, 1860632239u, 902013623u, 571573600u, 25414363u, 3412397724u, 3841538145u, 215707661u, 324367139u, 2323478150u, 3794355190u, 1128115053u, 2519022352u),
    SC(566244395u, 2591175241u, 2926679038u, 2852174582u, 200192886u, 521908517u, 2098042185u, 3563798587u, 1529741033u, 1248315044u, 233787221u, 2706044694u, 2870731528u, 3970719810u, 4167465378u, 525407392u),
    SC(2196340159u, 4056996284u, 1702457669u, 2086317410u, 3933566271u, 3751624213u, 4023204768u, 677196918u, 2137509058u, 4037704026u, 2299370032u, 1748548051u, 3326874481u, 1974512389u, 1751264060u, 266112293u),
    SC(1812114662u, 524787647u, 285577300u, 3638318945u, 3389691808u, 585441476u, 145370930u, 1149989778u, 1314386440u, 3471672106u, 908522311u, 4171434326u, 329350743u, 2954206373u, 856961382u, 2008618089u),
    SC(2318825510u, 3826102862u, 687747522u, 4263777564u, 2387018418u, 1135189382u, 1414060091u, 217356911u, 2998889592u, 698204196u, 801530770u, 3479982231u, 1117806357u, 154519605u, 960816747u, 3149429798u),
    SC(3250819610u, 351683992u, 296382659u, 4149667465u, 2183346760u, 1485561783u, 2218034265u, 420633334u, 1869679065u, 1205517989u, 3666184780u, 1975151679u, 371905540u, 367504198u, 1917294142u, 2403996454u),
    SC(3958230362u, 3773825115u, 783748416u, 1243337893u, 4032003144u, 3908441244u, 201600922u, 2000451013u, 728826842u, 3533421010u, 3229478766u, 278198864u, 3933272000u, 1331731276u, 3202405750u, 1474627286u),
    SC(3181836998u, 2581633616u, 3993055681u, 4020956268u, 2094932060u, 3551878275u, 393027783u, 2154269634u, 2283536956u, 2260289773u, 832949759u, 2403309662u, 3488387345u, 1652392255u, 393935478u, 2309058441u),
    SC(4141036972u, 1727820200u, 832481848u, 1055621047u, 1091666560u, 1393833209u, 3406509646u, 2428157250u, 2974564551u, 2286298667u, 3776410458u, 815994971u, 1241023789u, 775596275u, 1035618310u, 3934253771u),
    SC(206932164u, 4239023187u, 2046365950u, 2616857124u, 4246776524u, 4059028269u, 129664965u, 907402684u, 3859465657u, 4204192080u, 91453633u, 301171900u, 385561248u, 2689085222u, 1614465584u, 3977451005u),
    SC(3683171878u, 3148577689u, 4042394721u, 1085044656u, 682611813u, 2857177748u, 2417075323u, 2983755657u, 3777418770u, 2448398967u, 3909780770u, 4000218621u, 4227580585u, 2425908645u, 1704039191u, 3712924954u),
    SC(290465694u, 3921687099u, 2971845338u, 1854613741u, 1583022754u, 371222458u, 1744154613u, 3918664956u, 1960343256u, 1291903121u, 4010470137u, 1525668440u, 4091170130u, 1370665614u, 3468958243u, 1262617601u),
    SC(469638518u, 1129475898u, 3766065538u, 1777952666u, 2589258222u, 3182239596u, 2626554219u, 1853296675u, 2912212627u, 2518041806u, 2743002885u, 3765128027u, 851537937u, 2059010589u, 1827964742u, 3630398912u),
    SC(2458599023u, 2699477701u, 2305781427u, 2536499567u, 2118412162u, 1356010449u, 1426052710u, 725853717u, 1358092245u, 4196538471u, 66159936u, 4076320019u, 3065284443u, 2664736186u, 1943959552u, 939016920u)
},
{
    SC(3159079334u, 690659487u, 1550245019u, 1719420482u, 1795694879u, 2846363486u, 15987067u, 569538014u, 1561199762u, 967336684u, 3110376818u, 1863433225u, 3468533545u, 3644881587u, 369296717u, 3652676359u),
    SC(3207794512u, 2847938045u, 2415472979u, 1444858769u, 666387488u, 1660608279u, 1038886882u, 10876848u, 2468284561u, 2494495733u, 2622688628u, 2362399325u, 2213804831u, 3448783854u, 3958704532u, 3639349832u),
    SC(54374990u, 186360229u, 3420619566u, 1356363720u, 2768151763u, 3862789233u, 4270651882u, 2681019589u, 2332931746u, 928338209u, 3968478928u, 3908570621u, 923281930u, 2285715383u, 3620920276u, 130031468u),
    SC(4009596626u, 493238747u, 1786722937u, 653638870u, 1636723425u, 1884625267u, 2113708566u, 1448416211u, 3613674959u, 239497564u, 404863679u, 1521570751u, 2819432609u, 623319225u, 3073321373u, 565867032u),
    SC(1220575379u, 4235426741u, 1889734996u, 43054857u, 879216917u, 3299856237u, 2922851906u, 1054251029u, 693641076u, 1704223409u, 961665328u, 2828086835u, 2727513652u, 1580557310u, 4169876178u, 682569510u),
    SC(1757813477u, 22814395u, 3549822650u, 2254547303u, 372100012u, 1555116803u, 2587184145u, 3995169383u, 2645743307u, 188252331u, 3723854483u, 2138484090u, 1895504984u, 3538655836u, 1183003060u, 1439034601u),
    SC(2578441833u, 3136721732u, 380864696u, 817462912u, 2257087586u, 2256998015u, 93155068u, 930999672u, 2793712092u, 2223512111u, 3157527446u, 1098951014u, 3490358734u, 1362531303u, 2421324125u, 1961922428u),
    SC(1049179776u, 2969815936u, 3869567708u, 2883407597u, 1876243265u, 3498929528u, 2248008570u, 1231166427u, 3544374122u, 2839689583u, 1991744998u, 2798946627u, 736844268u, 1293771673u, 153373649u, 1931110485u),
    SC(3785289356u, 1913060964u, 169967200u, 3348219956u, 3732729076u, 987877186u, 3063387163u, 3310757163u, 3480818987u, 1991307039u, 2882756981u, 1215305494u, 855630497u, 1471153868u, 1338946323u, 398364632u),
    SC(1356154057u, 3013675057u, 3810909135u, 1796458190u, 2691409967u, 3963509663u, 2487357466u, 2764459334u, 2828737787u, 378542508u, 427318427u, 2412936991u, 393927878u, 3384382899u, 1135834101u, 3447900619u),
    SC(3813669196u, 1922867812u, 483725924u, 518662823u, 3954558327u, 1908218112u, 2258643690u, 2093138355u, 1162728847u, 205977116u, 821018600u, 1237824238u, 2980682686u, 1821003630u, 3221633606u, 2717269894u),
    SC(1353035942u, 2442753208u, 348196860u, 2355246066u, 2218279077u, 2203055542u, 1964199656u, 1329637142u, 1824193111u, 3965017045u, 795175573u, 1029253807u, 3915633667u, 1084707851u, 1682462202u, 2090124205u),
    SC(190807548u, 1133131805u, 249542006u, 2858611426u, 304500253u, 2183315108u, 4145782890u, 2998333644u, 962888949u, 974441750u, 1484862994u, 801464190u, 2311388331u, 114769498u, 4260362972u, 1017092877u),
    SC(1311406963u, 465174990u, 1760870095u, 883652788u, 1015674641u, 840236175u, 3124632038u, 2756294642u, 178804852u, 3164952754u, 241649187u, 1040890173u, 82588907u, 1771630815u, 1058353446u, 2473824375u),
    SC(943051847u, 4107933890u, 535438460u, 2683519853u, 3177219980u, 3711205196u, 3390138615u, 2920849102u, 3747455519u, 4138118615u, 400899690u, 4278329560u, 2602463649u, 808685972u, 136036034u, 1078020636u),
    SC(2185570356u, 3896907774u, 3620938057u, 1790823508u, 720763411u, 2404776615u, 3257162972u, 1221107462u, 3223154083u, 2528715719u, 688766234u, 1813423773u, 2324112952u, 83241050u, 4119437520u, 552112812u)
},
{
    SC(3370489298u, 1718569235u, 523721575u, 2176389434u, 218587365u, 2490878487u, 2288222859u, 812943600u, 2821517993u, 3626217235u, 1545838667u, 3155352961u, 741681736u, 669093936u, 2382929309u, 2620482966u),
    SC(40739723u, 469402467u, 1810137291u, 109375068u, 1888845715u, 2140810583u, 1053250454u, 3220064762u, 2539857789u, 4089587896u, 1364971662u, 2996699084u, 3939034030u, 2020251221u, 1606532641u, 3453095239u),
    SC(1376139558u, 886121026u, 3003069127u, 3500718919u, 4223467610u, 3212808910u, 126355621u, 2065481301u, 218954382u, 1236555811u, 2283280895u, 4256918831u, 1550185311u, 896721211u, 4286247506u, 2515527710u),
    SC(2942433244u, 2364220023u, 3675668782u, 3695614763u, 4041312428u, 2311531471u, 543507321u, 1902188023u, 1380686629u, 2455468346u, 2346421766u, 2211296276u, 3675221499u, 3890242164u, 3592353914u, 323566438u),
    SC(971323999u, 4115912859u, 3703400072u, 2662062035u, 2355087034u, 610373016u, 2293984834u, 2456129286u, 2927901115u, 1832014620u, 1168920846u, 552716242u, 3101454502u, 1707155244u, 3450287619u, 2546358284u),
    SC(3062358608u, 1394539264u, 4158727824u, 1704721957u, 1117692646u, 4057908715u, 1958466020u, 2309578289u, 271836599u, 2617957229u, 202314495u, 2948978715u, 1423414031u, 4128837100u, 1937488702u, 3301405882u),
    SC(1276638700u, 885904232u, 3686149920u, 3283641475u, 619290126u, 2510808612u, 1691008630u, 573145513u, 506979295u, 3062936948u, 2703005699u, 4056634904u, 3460956977u, 3783023797u, 1215556973u, 3726733337u),
    SC(3145485089u, 2008513183u, 2407056102u, 633050174u, 2634406893u, 2883313710u, 1233018283u, 3273507959u, 174012667u, 2126243450u, 2258342097u, 2857351925u, 3446764464u, 1187986524u, 3004835628u, 3228122242u),
    SC(991481464u, 1720231754u, 1918287975u, 2752686681u, 1174123782u, 4227334584u, 1634945718u, 1074218184u, 3572504705u, 1199611126u, 1378243227u, 2901862427u, 2145083550u, 1055786253u, 3960418624u, 587771424u),
    SC(3060872990u, 789280525u, 184089463u, 1784976524u, 344050594u, 2949751745u, 3173202246u, 3813443247u, 1247337895u, 4000924548u, 76989753u, 2093985529u, 265772293u, 3310477933u, 717631968u, 1024610284u),
    SC(3399834097u, 2964304651u, 3593395714u, 2850196125u, 2344305533u, 3920139836u, 937580696u, 1116439644u, 4147778799u, 544787491u, 2461636418u, 2647550544u, 1451408824u, 3266700679u, 829974548u, 2625074193u),
    SC(645329496u, 2808202504u, 1366740717u, 2841442794u, 1298546911u, 2730798019u, 3834987045u, 3258634143u, 4257492959u, 2976079952u, 1735944512u, 988426767u, 2395072762u, 3103996991u, 730963792u, 4206896923u),
    SC(3457675112u, 4140282966u, 1286302693u, 575230857u, 2270112110u, 3056424235u, 1835144711u, 421529065u, 2499621064u, 1907217915u, 1365357672u, 2875249236u, 1193490885u, 644367230u, 2115448516u, 2507997379u),
    SC(70240820u, 3745431832u, 1098747160u, 82118642u, 2446590634u, 851446619u, 2715739022u, 2142293045u, 2689000746u, 4219383621u, 3140617705u, 1457579904u, 2541485894u, 3932513084u, 3406615220u, 2746135210u),
    SC(2576508439u, 3244150028u, 2516535555u, 3986514000u, 2903382402u, 2225326585u, 1780804949u, 1164188435u, 1682143109u, 2949153515u, 1249412173u, 349674695u, 3467452794u, 1021028584u, 1194554595u, 1296132950u),
    SC(1028084134u, 2577983628u, 184499631u, 1037888434u, 1676727662u, 1831883333u, 1276555462u, 4161670547u, 372201005u, 844715673u, 24290758u, 1268964661u, 297554992u, 4061435345u, 719976096u, 1670144314u)
},
{
    SC(1239892635u, 3772349433u, 1058531752u, 1409211242u, 2847698653u, 2391143499u, 2637108329u, 3000217976u, 4288568828u, 658925470u, 2552628125u, 1468771377u, 3230644908u, 2692030796u, 7587087u, 1951830015u),
    SC(488413080u, 1055864530u, 1967623060u, 3973308786u, 2745059783u, 477755698u, 544732176u, 3786002606u, 1569550024u, 2935491988u, 1047898991u, 1749060996u, 1828274710u, 2943223535u, 3716062834u, 1253889972u),
    SC(1626917485u, 492893476u, 2371366539u, 3928996000u, 3710769983u, 1237244931u, 1562679102u, 2930576193u, 2085522193u, 2968039733u, 3202113740u, 4250144171u, 3666251088u, 2016963274u, 293320478u, 3775462481u),
    SC(3337977767u, 1831658883u, 1096680294u, 2436280860u, 119870062u, 1444445305u, 1467566544u, 2038307180u, 661842797u, 2493843529u, 3851219498u, 3941720925u, 1848373617u, 4051739727u, 1120765529u, 1101800264u),
    SC(929493756u, 2211014659u, 3851484027u, 3468182176u, 147674626u, 3850162187u, 1517171722u, 907705770u, 3997080856u, 3666272567u, 659948161u, 2282159142u, 429200635u, 2563204390u, 1422415938u, 1688129608u),
    SC(551422730u, 1797390513u, 2828310972u, 97463725u, 131839682u, 3917501017u, 566617767u, 700714239u, 3061891811u, 856175415u, 1072683059u, 1754819408u, 3533865753u, 2568134802u, 4226648923u, 32646755u),
    SC(3538971706u, 2916601269u, 2891999350u, 3825811373u, 2355258376u, 2876141009u, 3940019347u, 1309282440u, 2567828520u, 1367177503u, 2910098023u, 1986452448u, 1802584940u, 1360144734u, 2877029236u, 3033303547u),
    SC(3313753312u, 261894832u, 3637017242u, 3699232915u, 3508549542u, 3960876382u, 582644479u, 3199091169u, 3644395252u, 2675904765u, 2072396219u, 4071523208u, 3976776729u, 1025403411u, 2178466200u, 1107450603u),
    SC(2163612584u, 2845646977u, 4033161886u, 2723908899u, 1902990762u, 3375716497u, 2588626243u, 513179480u, 3101622846u, 1458272618u, 3875706546u, 3028150894u, 3612001457u, 2583302957u, 3385091312u, 3719047138u),
    SC(1256280924u, 3685139058u, 1853414115u, 3160743702u, 3455476559u, 2505590918u, 2308735646u, 3742507036u, 4016470170u, 330769483u, 3470077232u, 3383715347u, 1440115354u, 2667395648u, 1883060415u, 3332144245u),
    SC(558087170u, 3027059128u, 1986900497u, 1642930671u, 5966195u, 3083778816u, 3199769457u, 1248728791u, 2110460619u, 327014118u, 2524517189u, 1776442925u, 1472982408u, 3459887088u, 1029172283u, 2232815594u),
    SC(1544258748u, 3397993939u, 2721410152u, 2948125157u, 3562231734u, 3011402493u, 3266317933u, 527195819u, 369665170u, 3216603774u, 1952585925u, 258420856u, 3339671680u, 3733846143u, 2326118329u, 2310291176u),
    SC(4140585488u, 4198875250u, 3415599245u, 3398679011u, 4155727512u, 331520374u, 785987151u, 146809315u, 2929041163u, 1558279570u, 1346822944u, 4167931729u, 2800498595u, 2809390575u, 3295157947u, 4121566122u),
    SC(3571413466u, 1596401972u, 140853088u, 3137527478u, 204556611u, 4111255020u, 3835120334u, 3048525996u, 399176328u, 3005771198u, 780994070u, 3747160103u, 3136546207u, 508755537u, 2521091931u, 1715747893u),
    SC(1156063870u, 393984449u, 1521183961u, 3649564442u, 183535572u, 3139859119u, 445469714u, 2815871833u, 1268459010u, 355340626u, 2465929503u, 750513297u, 1590602217u, 3983872541u, 97286792u, 110438349u),
    SC(2549125874u, 1976691716u, 2532749644u, 279085303u, 633988261u, 3513450026u, 1057503157u, 1110164955u, 317735960u, 3241215082u, 3855084900u, 4137511567u, 3550729054u, 819799870u, 1929320159u, 2825290282u)
},
{
    SC(2585638847u, 1394876113u, 3750575776u, 4144761638u, 1991524028u, 3165938218u, 158354186u, 812072970u, 3814951634u, 2507408645u, 1163603486u, 3566585210u, 1424854671u, 3326584505u, 3332079056u, 1901915986u),
    SC(1520752595u, 1952396314u, 3263601295u, 3458083478u, 3797830135u, 509407552u, 3232598095u, 1205382790u, 2667815610u, 2560349365u, 2472625295u, 2883979179u, 554514567u, 2376619906u, 638138357u, 2568018129u),
    SC(2442202610u, 2091297602u, 25025777u, 3622813695u, 3869161931u, 614884494u, 984078136u, 3345125623u, 3918959025u, 227030161u, 3885929851u, 1281751413u, 1612359075u, 2958486463u, 2884267132u, 3619290927u),
    SC(3048700207u, 2570072469u, 1076153001u, 3767270422u, 1408579070u, 2076435276u, 2224129615u, 1962182553u, 1823335118u, 1499162388u, 1563913085u, 2068011578u, 1991334162u, 1665201834u, 1756239294u, 648108494u),
    SC(2337582449u, 1819429591u, 3833487099u, 3870804287u, 2300831739u, 2232929806u, 1869816966u, 4084965807u, 4220168543u, 1248736546u, 924637940u, 73528534u, 2319796252u, 3657850751u, 2794932350u, 4220430348u),
    SC(3028904021u, 2992718647u, 2354594543u, 3084902105u, 3127673085u, 783373559u, 3896264500u, 3984439851u, 820119108u, 4253719169u, 2623678017u, 3039126654u, 2922756242u, 2436956481u, 442364253u, 918876081u),
    SC(1539558451u, 2306960255u, 1095386938u, 770368485u, 2906552323u, 3075682102u, 3534951832u, 2083903147u, 1308495764u, 2261904633u, 2112467113u, 1044610889u, 3222649255u, 1736090274u, 1954974285u, 1361850096u),
    SC(587984395u, 1588261189u, 4052666242u, 512106258u, 651085942u, 2768947530u, 1250487652u, 1245674804u, 857176247u, 3594046498u, 647658046u, 2882585491u, 259918032u, 3698728358u, 632752990u, 351374374u),
    SC(2404749839u, 3296323382u, 805352255u, 3954906457u, 3558496371u, 2470613864u, 2024150378u, 3564550335u, 2499521206u, 2051669779u, 607498894u, 3748811695u, 1128400961u, 3072401950u, 3042994760u, 811721793u),
    SC(3595493043u, 1889077187u, 1981480426u, 4189336058u, 2081249554u, 2321560592u, 971543366u, 358725627u, 3595364674u, 3986924883u, 2193763710u, 4189195361u, 3121216309u, 1140981210u, 3226790033u, 353586077u),
    SC(2871195936u, 2843651834u, 635723881u, 287569049u, 2067429609u, 2943584978u, 644639896u, 1264563774u, 670309272u, 2690274713u, 246950668u, 933865226u, 4053660195u, 1381269871u, 462688690u, 5420925u),
    SC(977313734u, 4104230969u, 3334283655u, 1580178205u, 1578158646u, 1460773045u, 1728595474u, 3957344726u, 553676110u, 966612385u, 1786516334u, 2979157051u, 921122693u, 911238485u, 3922067113u, 1046213221u),
    SC(91424183u, 123813459u, 1667146297u, 3387121372u, 438965888u, 4260725592u, 154972710u, 3237027664u, 3006360433u, 2505005588u, 2902337724u, 2660287100u, 1901200613u, 2646189902u, 2780155597u, 49560303u),
    SC(3586622617u, 925349590u, 415005474u, 1260234539u, 30249250u, 2179523979u, 3475887768u, 3019952034u, 3517624902u, 4230850494u, 3734868171u, 742624613u, 822822789u, 3974379285u, 3711572581u, 3366701706u),
    SC(329275906u, 1905371123u, 4004795330u, 2339811253u, 353091905u, 548998992u, 3687895576u, 356859438u, 2494263562u, 926298666u, 3983230019u, 2882391620u, 2824170047u, 2247742371u, 1881005652u, 1386887463u),
    SC(1046492158u, 2680429213u, 1614272999u, 4010933686u, 2479992689u, 595409283u, 765550354u, 2852655093u, 1983575334u, 3910696497u, 2308266592u, 3012641543u, 2582478313u, 14949228u, 60656360u, 1955264759u)
},
{
    SC(1355623958u, 2575138117u, 2562403739u, 1638722303u, 1523970956u, 2189861089u, 3498071469u, 1919711232u, 231840827u, 3230371223u, 143629793u, 1497495034u, 1677900731u, 1608282251u, 3485501508u, 3944969019u),
    SC(1317209868u, 3823870608u, 3335344652u, 3702793515u, 2425890570u, 1442662397u, 4007605978u, 2976935239u, 1444558882u, 3449074340u, 523287240u, 1767769527u, 1776192231u, 1111610095u, 4035220013u, 3434023407u),
    SC(1286632782u, 1751340143u, 184421370u, 3989392405u, 1838859918u, 3681550977u, 707040060u, 2695037953u, 1828105102u, 812532736u, 1115387936u, 1381188966u, 1389542552u, 621856846u, 1135930465u, 831833090u),
    SC(2741542793u, 3565635943u, 455105161u, 2389444906u, 2966273581u, 4048751601u, 2569017914u, 1796095397u, 1515760827u, 3870103158u, 2737365395u, 818096507u, 2179280538u, 1254083919u, 2114706477u, 1413209953u),
    SC(2036431795u, 3313380793u, 2996275588u, 625273343u, 1627738147u, 2163909313u, 2645773664u, 3066825866u, 3862562238u, 3189614065u, 3074707667u, 1611214266u, 689055345u, 1845962762u, 3616153367u, 98214289u),
    SC(1783057147u, 1095836105u, 952581152u, 665189523u, 4159236737u, 3621720388u, 2768968806u, 1541462219u, 1550070665u, 2946487171u, 3084327270u, 3528580128u, 3683323170u, 2326350340u, 681502936u, 611874814u),
    SC(2075965546u, 3954443814u, 3457426695u, 3100575745u, 795895906u, 2051458923u, 4220432661u, 3191956430u, 2978441632u, 1935083482u, 1260223004u, 1989210512u, 708452144u, 1742032782u, 412060225u, 942058976u),
    SC(1554952802u, 1148928548u, 435577880u, 1218016814u, 774531999u, 4171943086u, 2728379380u, 1755428421u, 3096769247u, 551470356u, 663936617u, 2259245103u, 3605128160u, 4254582248u, 2543346251u, 2641240630u),
    SC(2834055303u, 3779347324u, 986655417u, 1060344853u, 1961336735u, 3444096071u, 3402507696u, 1296975131u, 4013745799u, 318316127u, 3012349080u, 1543913977u, 3581569730u, 3073345556u, 1048961320u, 3338742347u),
    SC(1917475623u, 1573453706u, 3775608035u, 1560651154u, 3305702627u, 840251936u, 2021694407u, 1567223161u, 1217097878u, 4101089784u, 1480235880u, 823763473u, 1860062290u, 3212933927u, 305432786u, 2664137512u),
    SC(488290329u, 2159084342u, 1977681447u, 3072933047u, 2133970307u, 2904163387u, 2929381044u, 2852811875u, 3486789427u, 3312981159u, 2897952520u, 3716688458u, 3312599340u, 2231560239u, 2736260178u, 2100166993u),
    SC(2561748569u, 2171003952u, 3930314290u, 4171544961u, 4084487200u, 1829909478u, 4190664042u, 1205662930u, 1332053018u, 3102835265u, 2758716514u, 3094681405u, 890009818u, 1835725787u, 3657145276u, 2012429206u),
    SC(1490727773u, 2663703693u, 1786667419u, 3911642156u, 1173781475u, 1032437218u, 949369190u, 3379245680u, 3855657643u, 102309733u, 3862169655u, 1953708469u, 2899532678u, 2185103023u, 2246792392u, 2140300644u),
    SC(1105179994u, 3403119551u, 2151897995u, 2133026531u, 4095632628u, 1958582421u, 3756551819u, 1353448323u, 343568827u, 940163873u, 3647008605u, 2675342302u, 2020863909u, 3314608025u, 3678853306u, 2350764749u),
    SC(3610890660u, 7527132u, 3948519712u, 999155044u, 1566318108u, 1592356541u, 1395933920u, 3725362820u, 1628394109u, 2361449910u, 3407340106u, 1370203307u, 1521539242u, 166450716u, 1562824595u, 815891091u),
    SC(4169640806u, 3985781662u, 2412370154u, 452406588u, 105016225u, 176939651u, 3796204183u, 875428687u, 2497589429u, 82221910u, 4277856341u, 1375558239u, 286683641u, 3316069361u, 519521869u, 2295715438u)
},
{
    SC(1272080061u, 1249052793u, 3406223580u, 3180222548u, 3305857569u, 3627944464u, 989639337u, 2790050407u, 2758101533u, 2203734512u, 1518825984u, 392742217u, 2425492197u, 2028188113u, 3750975833u, 2472872035u),
    SC(23055961u, 3145183377u, 2430976923u, 2926141735u, 1297155725u, 3931229778u, 1820665319u, 2985180446u, 3042883880u, 2460902302u, 3663963302u, 4048537328u, 3995357361u, 2497655514u, 2584741032u, 1771542440u),
    SC(3555045486u, 1984442910u, 1340694232u, 3778110580u, 1134128670u, 754930307u, 645413801u, 419876731u, 718672506u, 2655370853u, 650960778u, 1175245889u, 3468383881u, 2671574337u, 44753822u, 3359158981u),
    SC(289419990u, 2387037467u, 2851881154u, 4063189789u, 1829943773u, 2629576813u, 942097665u, 562844855u, 2647906183u, 117874787u, 202211775u, 3519990636u, 3082138694u, 1836881245u, 583992800u, 2183831281u),
    SC(2721107251u, 1807232970u, 3202569269u, 3708638735u, 3532027994u, 4114767065u, 2764680156u, 135914892u, 1473879964u, 2935607101u, 4201045944u, 3202280567u, 3793176244u, 41830505u, 969791663u, 1519485648u),
    SC(1497249350u, 1416277963u, 4236912956u, 1827689230u, 1595876921u, 792380080u, 2973128767u, 43523726u, 365213078u, 1703541227u, 1608568996u, 2447861933u, 4236202627u, 2270952660u, 996772411u, 1327926083u),
    SC(930257564u, 986864131u, 3788206015u, 4282936823u, 3575152799u, 1711906087u, 3523467955u, 1026809541u, 3754676659u, 126901401u, 34761162u, 674497989u, 546239979u, 3916171265u, 4169565745u, 1773808675u),
    SC(1188611875u, 4038625723u, 846346399u, 3124471166u, 3540873247u, 133640400u, 3354116939u, 2182269853u, 3158440321u, 538434017u, 508437111u, 2461460484u, 1662547818u, 3578959375u, 209001526u, 3335522863u),
    SC(4264155336u, 4248354463u, 3273048757u, 2876562537u, 4290560912u, 509206354u, 1722430555u, 1796475043u, 864985283u, 4161684480u, 1401260098u, 2472895218u, 2342429930u, 827590760u, 300446032u, 2313806596u),
    SC(2581459341u, 3429172659u, 2024065972u, 4099664542u, 1148350145u, 3444652410u, 3577141975u, 2981349935u, 4203645620u, 3053918552u, 3258443245u, 1577847138u, 1635931506u, 873577721u, 2391948063u, 3880308298u),
    SC(348781524u, 168814463u, 525438717u, 333282992u, 3413546488u, 563982782u, 3571937262u, 2168075485u, 2567967190u, 4135534212u, 2773230423u, 2560090101u, 4070935767u, 1086323696u, 2826348049u, 1398744384u),
    SC(1019826995u, 663251023u, 3152709102u, 4103744231u, 1372971676u, 1214523997u, 1159949230u, 2703418845u, 786011241u, 2156179212u, 1156040729u, 3454726929u, 1928366760u, 4000343119u, 4288863167u, 3214674902u),
    SC(2681260382u, 4128008241u, 2510236484u, 1511367526u, 1684226652u, 979685907u, 2954161581u, 3173181201u, 2348267479u, 1347783270u, 1149362033u, 739573388u, 2484197607u, 335176176u, 4239049161u, 739872951u),
    SC(2990421330u, 2634202447u, 3179573376u, 2783566953u, 2521510477u, 3781882024u, 2239710944u, 2912891640u, 4089020966u, 4152247187u, 3694477470u, 1764138981u, 2507816564u, 3857045441u, 3960587447u, 1062920229u),
    SC(2607237939u, 3082469982u, 2290705462u, 3066564076u, 3196897175u, 4248068159u, 2751492888u, 1096521131u, 1350638971u, 3209282660u, 3725272910u, 717966828u, 1468400702u, 1807609199u, 332456241u, 3283231722u),
    SC(752680913u, 2889161941u, 555836002u, 2587892579u, 793746532u, 2681266768u, 719050347u, 3803221u, 1422540107u, 1615046554u, 1724888503u, 923959013u, 3231965435u, 2753642578u, 1839210672u, 3344430910u)
},
{
    SC(35118683u, 172484830u, 3416100291u, 3700412376u, 540823883u, 3117923166u, 4211300427u, 2853939967u, 3346783680u, 988896867u, 2435731911u, 431849862u, 1744411117u, 2614624696u, 297543835u, 4045956333u),
    SC(2040009399u, 3617093372u, 1922089948u, 419196583u, 488784755u, 779735420u, 2537006207u, 704283906u, 1092151363u, 2578829348u, 2820670194u, 2121866485u, 3135057501u, 2561548080u, 1838738028u, 3520705790u),
    SC(2347233873u, 2021920507u, 3747005552u, 3302704092u, 1421533666u, 2091592699u, 3349570591u, 3813605549u, 115030445u, 3350012162u, 2428670067u, 3833734570u, 1834087037u, 3648785167u, 3795974654u, 230561124u),
    SC(3166315679u, 1499753232u, 1332394568u, 512254231u, 3188709397u, 2787249743u, 4120940214u, 2887173650u, 3906489413u, 2295240998u, 2578634494u, 1588397589u, 1261609842u, 547227344u, 3285763119u, 2699176838u),
    SC(2920964533u, 3740093834u, 2438698186u, 1924062654u, 745692322u, 2251363856u, 1198363872u, 1945834517u, 3791006786u, 4021475876u, 1202959856u, 137650558u, 3764418806u, 2028729507u, 3549185474u, 4085572018u),
    SC(2715838951u, 1959655040u, 1103474341u, 961883214u, 3220165814u, 946461598u, 3310562057u, 3895921046u, 3423737504u, 3466676673u, 4053794032u, 4003999722u, 704282430u, 186242539u, 1929875533u, 2743489242u),
    SC(3863164996u, 1689760206u, 3183192577u, 2929742795u, 2741898431u, 3788088914u, 2356234821u, 7039846u, 36640443u, 397902308u, 1207730645u, 450227359u, 3243815017u, 2084858847u, 1390053102u, 1800322698u),
    SC(2899288970u, 284742850u, 4164169257u, 657423444u, 1943078242u, 2187671316u, 2338824812u, 1463135135u, 2886625321u, 272841068u, 3193451269u, 275059886u, 893727404u, 1588413844u, 3713690958u, 858582046u),
    SC(220208151u, 2716463025u, 2076296789u, 1220608226u, 1158026410u, 3025895717u, 2670689841u, 80726308u, 1182245224u, 514737744u, 1549626516u, 2794996864u, 1140029757u, 2873715616u, 2877687374u, 2336796195u),
    SC(1712499527u, 3009254442u, 159655935u, 3126441867u, 4265886590u, 3094626983u, 2035167860u, 2311303989u, 3444838362u, 2596170866u, 3801673179u, 1837914686u, 3231006463u, 1247923284u, 584065013u, 4147287941u),
    SC(900839097u, 216650153u, 2150488455u, 1952211291u, 2276027011u, 3518121564u, 3433005808u, 477320989u, 4007917006u, 2860081630u, 3686269191u, 921073036u, 3922496269u, 1487331039u, 3974930220u, 2054391386u),
    SC(3348685354u, 1508268709u, 1715972206u, 4188610176u, 2563479521u, 2178972493u, 3288192040u, 3754144178u, 1173914019u, 454089507u, 3398639886u, 574196980u, 135948897u, 105476021u, 2877469782u, 2140775314u),
    SC(60661201u, 2505799644u, 1330476086u, 2641855913u, 3370908611u, 3545887069u, 2369313011u, 278373074u, 1677987717u, 2174519857u, 2497481396u, 1568231376u, 3671812134u, 1893623337u, 1526376990u, 3328774765u),
    SC(2836826686u, 3566150450u, 1220364883u, 3711427451u, 3528155780u, 2723292785u, 3326692341u, 2222164977u, 1858144237u, 1869912598u, 665154087u, 1299959695u, 2415334423u, 2100885199u, 1677986979u, 848478053u),
    SC(2293836559u, 1740853836u, 1031472293u, 3209927466u, 2722427870u, 1686533972u, 3134525842u, 43165427u, 4133377528u, 4179858803u, 3614537390u, 3380004165u, 2699323023u, 2351902646u, 3408173486u, 2494501357u),
    SC(1820258417u, 3371479244u, 1743152481u, 953496909u, 4267482844u, 97428203u, 2755286865u, 830318058u, 1082737155u, 2096588114u, 869939293u, 1138867599u, 3414628151u, 3300388932u, 2755674787u, 886356844u)
},
{
    SC(1981590337u, 957784565u, 3778147127u, 3909235993u, 1637480329u, 2280601867u, 1059949562u, 2968107974u, 4043469535u, 4159249472u, 895867525u, 402468881u, 3186079639u, 86430659u, 4027560590u, 4067278225u),
    SC(174847206u, 2629171882u, 2333280466u, 3666750170u, 1365991192u, 1932613341u, 769674425u, 2870677148u, 3091982589u, 717533940u, 691292429u, 746447527u, 2346750954u, 2424023836u, 2489851473u, 1000862947u),
    SC(1294470925u, 420276534u, 18534679u, 2910625938u, 3592407247u, 3676292946u, 91786365u, 2630448437u, 4060747756u, 3372072880u, 766751258u, 2899531047u, 631745164u, 3523898915u, 3168717447u, 2801541394u),
    SC(4228902076u, 3340600279u, 3364406353u, 4167190351u, 39030410u, 2148305555u, 4106423272u, 4019775241u, 1048613489u, 896239533u, 2278643848u, 649090509u, 1858593869u, 1017004108u, 2725922618u, 2362479567u),
    SC(3279186701u, 4095625861u, 3191586341u, 3252046177u, 4161721618u, 2329134038u, 751155705u, 2989611709u, 942304573u, 3648059604u, 2883823407u, 1492175829u, 54393633u, 3106238944u, 429976962u, 1435978615u),
    SC(3849622377u, 2984399872u, 690474125u, 61954906u, 3671421106u, 3429544548u, 2830056933u, 4242121816u, 952897126u, 3854066003u, 462125754u, 3261473627u, 4248077119u, 2601130223u, 2596495819u, 1081964366u),
    SC(3544595842u, 126020837u, 2577264196u, 3433073867u, 496013073u, 2132398305u, 2482253446u, 1347711182u, 3954364337u, 261394336u, 1107608476u, 3443266300u, 104305688u, 870955527u, 3446753045u, 646876293u),
    SC(164137956u, 1354687087u, 347069906u, 2162313159u, 2097666782u, 2177194309u, 1083298213u, 1791764705u, 445921337u, 2034078155u, 2254058003u, 1297019339u, 2457505957u, 3923390662u, 3364713163u, 2092921u),
    SC(2010686846u, 2180989257u, 2265174665u, 208481647u, 547071646u, 2570387552u, 227431381u, 3946252713u, 1802054573u, 2876468168u, 3435214380u, 619729504u, 96719536u, 601795828u, 1679578869u, 3266813859u),
    SC(1689091897u, 2850488954u, 85895902u, 2363909390u, 557966933u, 189022184u, 4135255025u, 2090271113u, 2804992462u, 2897353835u, 3129164865u, 2671868525u, 1204434986u, 2421048110u, 1069687644u, 573230363u),
    SC(1864118934u, 1742326766u, 130305247u, 3848358018u, 448383585u, 389136808u, 676464280u, 133776905u, 3973153497u, 15653017u, 4189644276u, 1910866015u, 4017185152u, 3100723612u, 137322886u, 3499754296u),
    SC(2165760230u, 1978556390u, 4038887110u, 3280144759u, 2755863878u, 1292009146u, 4196675347u, 2883653205u, 2360229279u, 2940095236u, 4183119698u, 122598923u, 483221264u, 2336117478u, 1200036442u, 1470973u),
    SC(22625049u, 2110942382u, 3865539390u, 3568657648u, 4280364838u, 467068956u, 1638706151u, 934686603u, 1016938107u, 1378881668u, 2052861738u, 969631954u, 3114829317u, 2704079673u, 4202235721u, 1896331078u),
    SC(1272877817u, 322275610u, 2048255u, 3828419764u, 283292018u, 656555904u, 1730883898u, 407673382u, 3259565233u, 3319282763u, 829721223u, 1466466546u, 121051626u, 1142159685u, 3894622225u, 1384264827u),
    SC(3763136398u, 3055118026u, 3433748869u, 930030556u, 2135841826u, 2075894041u, 2845381068u, 3086878324u, 257833966u, 160279206u, 524657374u, 1855318297u, 1760771791u, 1248968332u, 2414205221u, 2464430473u),
    SC(3809273981u, 900900763u, 2895572448u, 3283497701u, 1349213062u, 580961411u, 3299214221u, 3628519825u, 3643683404u, 3319374656u, 3868217535u, 427844533u, 3841842588u, 2749654710u, 2681210929u, 1051800659u)
},
{
    SC(1622151271u, 634353693u, 3884689189u, 1079019159u, 1060108012u, 22091029u, 115446660u, 534633082u, 1649201031u, 4042006969u, 137296836u, 1833810040u, 1562442638u, 3756418044u, 1181092791u, 160208619u),
    SC(1041667920u, 3037209402u, 1477404634u, 1440610569u, 2797465015u, 2054982250u, 3391499235u, 3605494419u, 3639198696u, 1933432209u, 1915711520u, 2741088986u, 3869566747u, 1879175626u, 717801628u, 458685614u),
    SC(2768417957u, 2138940313u, 1896672548u, 1414723957u, 827016389u, 745281061u, 1045174332u, 3577682097u, 2169383377u, 1730416479u, 712654956u, 3155052928u, 1776219501u, 3353461099u, 711436547u, 1497369655u),
    SC(1896697766u, 3621973902u, 926548253u, 4069206549u, 2297004301u, 3251063401u, 993943014u, 1270589313u, 3281589988u, 588955836u, 2429665887u, 1734915238u, 3409902793u, 2578722241u, 654727507u, 3216225031u),
    SC(2536890957u, 2554531636u, 2109372546u, 2649000077u, 358086224u, 3391808161u, 1211714614u, 2605265326u, 2606629887u, 206756474u, 1092207840u, 3362434504u, 3945886373u, 4232252600u, 2886868947u, 3532954370u),
    SC(65718672u, 4071991225u, 2060698395u, 2198173427u, 3957878549u, 4022831630u, 3461473682u, 419893418u, 779469249u, 2019627177u, 2019172804u, 3609556656u, 2681069216u, 2978123659u, 1249817695u, 2366599297u),
    SC(2811735153u, 3049657771u, 1390752797u, 1411409994u, 2127695318u, 3083850245u, 787626821u, 1929564189u, 855492837u, 4008216334u, 1809444437u, 2182869717u, 813270534u, 2247412174u, 1161082081u, 1381922858u),
    SC(3920648469u, 503487540u, 2083562080u, 327383264u, 2785608988u, 867359286u, 1036950980u, 431152821u, 1419040671u, 2665230771u, 2455357484u, 351717207u, 3187759581u, 3348793239u, 2511298896u, 1213040259u),
    SC(2396309679u, 670711827u, 2849604206u, 3201137057u, 818618388u, 2531623890u, 3805810347u, 1463443182u, 79508933u, 3480790940u, 3579218280u, 263259195u, 3368747551u, 3044188079u, 1352272344u, 3090026690u),
    SC(337838342u, 789695791u, 185502398u, 1517725636u, 783544345u, 2877621235u, 2946546356u, 1215973672u, 1208860651u, 725001171u, 1289736233u, 3756237869u, 1654092362u, 364807179u, 4279861158u, 4016003402u),
    SC(1113567525u, 3780565260u, 836674522u, 1827009520u, 756906197u, 2663480421u, 3902552087u, 3507352398u, 774509259u, 224530498u, 2361577079u, 3744385228u, 3961162378u, 2586454589u, 3040342450u, 332039963u),
    SC(3041171145u, 1474749273u, 2282851768u, 649990155u, 2952549483u, 1360702019u, 1809905451u, 544396952u, 68636355u, 2878101257u, 1478326650u, 2199663643u, 320705780u, 628185476u, 2087425498u, 3828181698u),
    SC(3988280964u, 459019854u, 4007245269u, 1946776277u, 125932076u, 3922945473u, 608655237u, 759981570u, 1458494773u, 3686363491u, 3746534866u, 3692063331u, 290340676u, 486223220u, 3313127929u, 2280570810u),
    SC(233319658u, 3886064320u, 853251650u, 1236563554u, 538386922u, 1967845333u, 3003439052u, 2872751142u, 150287328u, 2176354561u, 3956114759u, 3858039u, 2003618785u, 4212993191u, 2956509701u, 3196752221u),
    SC(2121593903u, 3906201458u, 1137774967u, 3978600103u, 780659717u, 3484790562u, 769856015u, 36405780u, 695767695u, 3397748350u, 3377872749u, 1577340836u, 783581424u, 3804923626u, 2896998870u, 1723843622u),
    SC(2572703671u, 2154230449u, 1195305676u, 4208655231u, 922600921u, 448134411u, 986012643u, 2442352758u, 1662902878u, 1367546113u, 2863017129u, 59878996u, 2111442975u, 648834983u, 865532037u, 1000323350u)
},
{
    SC(2802315204u, 2299944053u, 2128407100u, 3463617348u, 2448441666u, 1070000794u, 1884246751u, 210372176u, 4075251068u, 1818330260u, 3223083664u, 3496698459u, 3376508259u, 4156094473u, 3718580079u, 1962552466u),
    SC(3866141502u, 1978128229u, 2646349807u, 2688968712u, 1012393569u, 2539553175u, 2230158790u, 2206981245u, 3747509223u, 1243575365u, 3510697084u, 4007723917u, 859148499u, 1713821117u, 199178654u, 2644187203u),
    SC(1964672019u, 297703434u, 1518880848u, 3373273121u, 959853764u, 2251122694u, 723413077u, 800337307u, 648287930u, 2947400245u, 1113383775u, 3610122168u, 1829970570u, 2892296971u, 1554744636u, 494969279u),
    SC(4031050415u, 1835549397u, 2490029791u, 1131956513u, 1204048760u, 1914510905u, 3436953651u, 3943499769u, 1759802551u, 3820069122u, 4025269834u, 2717988015u, 2671631612u, 1159803272u, 1951365142u, 4085381442u),
    SC(606110736u, 4064038873u, 70240913u, 2494945854u, 3729188113u, 2063877878u, 3912150605u, 3215847250u, 2977890044u, 3389766053u, 356841724u, 356991784u, 2228722660u, 3145515298u, 2594559598u, 1158432841u),
    SC(1794017518u, 25183950u, 1671020817u, 785574353u, 95301808u, 1715172822u, 2718673424u, 1470113919u, 1142251437u, 2499778479u, 4281783303u, 1325560741u, 2913926884u, 3804531669u, 3139007483u, 1406557472u),
    SC(2970751291u, 2450850294u, 545967636u, 1959629374u, 3303894193u, 455065073u, 41447235u, 1831795469u, 3594460859u, 4077235761u, 722461030u, 598330044u, 192707446u, 509790368u, 1051867275u, 1446366645u),
    SC(1959543921u, 1887295052u, 3154544834u, 487969766u, 2252004301u, 996805128u, 2018864848u, 597352487u, 1136669046u, 533675042u, 981364938u, 2653382923u, 1408807893u, 2742559841u, 1833041360u, 1912794731u),
    SC(2721713526u, 3549551325u, 601974093u, 2790584575u, 3951999363u, 4215366345u, 2845142034u, 4218934731u, 1726020765u, 823952138u, 3809833u, 4233069287u, 1129914456u, 1399496316u, 1915356031u, 4169077603u),
    SC(3926695685u, 1849292395u, 1522137139u, 1552827989u, 4109112844u, 2060253220u, 2853920191u, 801241282u, 3422535773u, 1693187125u, 2113050221u, 2708536698u, 2777027446u, 4174902187u, 1811957361u, 3772547370u),
    SC(3930825929u, 747327770u, 2505687587u, 2880650279u, 583976081u, 3834434841u, 1957901663u, 82062519u, 1246607062u, 4096185443u, 1298601955u, 3551964017u, 2293924654u, 2316870880u, 1326950040u, 3135743003u),
    SC(2476396705u, 2790106263u, 443544224u, 2802435205u, 819417773u, 177556618u, 4130535785u, 2505448107u, 2591437865u, 1610510350u, 3815578981u, 4114533339u, 2461835810u, 3856846001u, 1439644255u, 3343979676u),
    SC(4065627430u, 2927818196u, 950831561u, 4171626868u, 1177734694u, 150634338u, 2487656862u, 796691698u, 2119716392u, 2975402883u, 833495592u, 2179672277u, 346833760u, 3054174076u, 3573945862u, 3318693908u),
    SC(2752867821u, 4203551149u, 1685153083u, 1110714758u, 1962211454u, 2837810663u, 1792364454u, 4089022191u, 3967274249u, 192406218u, 3350506767u, 1577386058u, 1497165592u, 1817646171u, 1066733732u, 617241273u),
    SC(307712584u, 3903562077u, 681601120u, 3047177738u, 2486055863u, 3842609448u, 3660507009u, 2553494609u, 3174736607u, 3482954246u, 1496988826u, 1025695462u, 3184242644u, 1095387068u, 949053977u, 2083266597u),
    SC(3022399010u, 1538609936u, 2420072227u, 990220729u, 2914167049u, 3768364162u, 1346299210u, 1681335666u, 2574961060u, 4053930867u, 303191498u, 2606902764u, 726562386u, 2306023171u, 939416980u, 608183941u)
},
{
    SC(1862109024u, 2933191225u, 198801920u, 104305860u, 4011109577u, 4122560610u, 1283427153u, 1072910968u, 1957473321u, 1766609671u, 2854361911u, 4075423370u, 2724854995u, 3336067759u, 2831739585u, 400030103u),
    SC(3665137971u, 362515859u, 3613170351u, 1634568159u, 2407386812u, 2769867978u, 3661728638u, 966943982u, 2329232814u, 928287686u, 386060431u, 2380767940u, 993235698u, 994357638u, 4262826729u, 789587319u),
    SC(700222805u, 4205189715u, 1681820282u, 2408317852u, 3145763515u, 149703318u, 2996102375u, 2778856747u, 1243021847u, 118692771u, 660320701u, 2037909966u, 3471407521u, 3539034550u, 2338530850u, 798101514u),
    SC(202761792u, 3072251152u, 936980226u, 2112028598u, 55725596u, 545941282u, 2866544613u, 2541609642u, 2986914411u, 250525398u, 494419489u, 904338436u, 448237071u, 2519815520u, 3547503723u, 3479815920u),
    SC(2591263445u, 2313710919u, 2225850186u, 2907469855u, 1923973028u, 2439332754u, 1359667863u, 1147453888u, 591668157u, 1802961428u, 2115337573u, 3814501239u, 1652114003u, 3286770823u, 2320492326u, 1627762005u),
    SC(915583786u, 1541647557u, 857793588u, 1120457139u, 593298997u, 1235522530u, 3835902793u, 4029633796u, 2892088014u, 950803214u, 2067553664u, 3466102617u, 417988445u, 1721721291u, 2995105031u, 1833135847u),
    SC(3713015457u, 984220366u, 1636921821u, 69668826u, 2853588756u, 3372417728u, 1514016965u, 3165630303u, 549067200u, 2237752955u, 3528219045u, 2819816242u, 2536477233u, 430232621u, 1219272797u, 2682238494u),
    SC(4158909478u, 628504302u, 1961569314u, 3701318609u, 1298978065u, 2797817112u, 2778611026u, 2986972418u, 2728592083u, 1350107926u, 261737783u, 1726357156u, 2342206098u, 3937750792u, 3688276065u, 1598643893u),
    SC(673033353u, 989709407u, 1304069795u, 4233856570u, 603282839u, 3834722266u, 3349356388u, 2690783748u, 318351191u, 3370905692u, 2347975280u, 2009518842u, 2234183321u, 2940030960u, 2623873751u, 1542240694u),
    SC(2380479990u, 2443937714u, 165899369u, 1753008008u, 3688956092u, 2346743686u, 143829732u, 3830274100u, 446444093u, 1705814492u, 2316415254u, 1337109896u, 3093454689u, 1928322219u, 2296006624u, 2093435857u),
    SC(4072133379u, 1665275533u, 1975626640u, 3338948757u, 3639875020u, 2527617364u, 2537708733u, 3825629008u, 3956434656u, 2047924528u, 2149850378u, 563001677u, 1364815414u, 2503665164u, 637530147u, 630327427u),
    SC(2169035971u, 3667715128u, 133026623u, 1213164483u, 1858042667u, 1566345391u, 3257221880u, 1553218197u, 1494901497u, 2543246705u, 3407410762u, 149097838u, 2595763051u, 3921913476u, 3975713216u, 1013875562u),
    SC(4285039888u, 3972750160u, 2508056116u, 3828502305u, 1554885499u, 2478771653u, 3465835374u, 2839338634u, 936668484u, 3860842840u, 1796057260u, 539213045u, 1979230663u, 2637220243u, 3822691920u, 124051918u),
    SC(4008482152u, 442930842u, 3844390262u, 1477377511u, 2570068482u, 380269897u, 3550124210u, 1507268577u, 1690622835u, 1216029693u, 2876552462u, 1409060125u, 862828291u, 1145788484u, 2966975851u, 3091998876u),
    SC(992351977u, 3038251247u, 1125019979u, 3468273479u, 2933515034u, 2848650947u, 3581678949u, 3449520008u, 3870604714u, 2854135121u, 1257402460u, 1206940695u, 2996845551u, 725641056u, 3899090423u, 600507448u),
    SC(1594814264u, 3363681343u, 1687711901u, 1220822433u, 2890970125u, 4169329849u, 1095390946u, 3969022672u, 2174219653u, 1940964660u, 1237339498u, 2965031440u, 1016584643u, 2590104317u, 4235803743u, 3748725935u)
},
{
    SC(770670183u, 2030489407u, 913827766u, 28354808u, 2556411291u, 589717159u, 413516142u, 20574376u, 1695189435u, 3750527782u, 3546610407u, 1435363367u, 2770958348u, 2608593137u, 3331479090u, 2086258508u),
    SC(386282669u, 3729286075u, 814752142u, 1413230862u, 2616133966u, 2483044279u, 1602859126u, 1971292416u, 3070813417u, 3451205972u, 735409142u, 4007950155u, 2905395594u, 2869625175u, 3709680291u, 2952203732u),
    SC(3404816958u, 563114856u, 2100979818u, 2101934521u, 2503989815u, 1063833326u, 1723163772u, 3130704072u, 2515274210u, 1396315966u, 393457735u, 2691705207u, 828877164u, 3349330754u, 122605524u, 2602269000u),
    SC(3709941627u, 592327138u, 2051205206u, 810649302u, 871212350u, 1541388603u, 4163983787u, 2631105522u, 665062813u, 2612020092u, 3229205070u, 3819479307u, 3310127863u, 1843015221u, 2875318880u, 3723951791u),
    SC(1567440489u, 946197176u, 1275093448u, 4236630568u, 3990268727u, 196525149u, 15396621u, 1859637416u, 3138749279u, 3859238173u, 3227404352u, 2720346799u, 3006927153u, 2147957966u, 397899810u, 870180302u),
    SC(1039540230u, 838590221u, 2330450212u, 923346890u, 4067788704u, 3619481496u, 3864357516u, 1659963629u, 3299501842u, 1079788777u, 949881347u, 2502746723u, 3228809289u, 247884983u, 3118597092u, 302086001u),
    SC(3566621623u, 1671359399u, 3923258138u, 1638982085u, 325268348u, 4006635798u, 1207442469u, 3002539627u, 4047574291u, 2011583803u, 1713508996u, 1060703309u, 4012225302u, 3776068377u, 2784459927u, 3025510009u),
    SC(4215947449u, 1997878089u, 1026649407u, 646510252u, 850804277u, 1871694871u, 3390738440u, 3114862405u, 3567086852u, 195428920u, 1556755650u, 1851670178u, 2207687769u, 3388294264u, 4058594964u, 4248126948u),
    SC(45480372u, 1361999478u, 2195192123u, 956464540u, 1294436548u, 3045580134u, 2390633033u, 757048237u, 1350268583u, 862465366u, 1780970485u, 3285033794u, 559081924u, 163710122u, 3170983363u, 2626972150u),
    SC(90053239u, 741607095u, 3003181022u, 3546281037u, 1996206866u, 2019149839u, 2216417072u, 1170259974u, 4159879668u, 130215053u, 2605146665u, 3967236653u, 1930867601u, 2409157952u, 3775975830u, 1489883331u),
    SC(40478381u, 3873592210u, 35609037u, 272986081u, 3051595606u, 504620408u, 1019656134u, 250693036u, 942133950u, 156032543u, 3738710122u, 1712961843u, 2888111563u, 1171258741u, 645705716u, 511104714u),
    SC(239657447u, 2278853730u, 2391081998u, 746810345u, 3484552464u, 1369592268u, 2655434121u, 1213868536u, 2934523673u, 3058111393u, 4281279490u, 3966376385u, 1307651904u, 1645528218u, 3652190772u, 1126527756u),
    SC(123809694u, 110218531u, 117547539u, 2035819815u, 3596140063u, 1382818318u, 3664758070u, 3019339789u, 2719299822u, 3892472009u, 2876096109u, 412140786u, 2578091481u, 2196346764u, 3068803053u, 1395690512u),
    SC(880155357u, 791542602u, 112062960u, 2175792069u, 531560395u, 3155859615u, 1042526138u, 680268271u, 1355330482u, 2485441305u, 148200464u, 964096786u, 3215229166u, 2660485876u, 3076499838u, 353883041u),
    SC(2388114644u, 1552848777u, 1649071283u, 2325568488u, 3165393822u, 2695611152u, 2713875122u, 898903657u, 2377088931u, 1138573339u, 3366910425u, 3238180215u, 676550680u, 1043832292u, 1583145576u, 3925456200u),
    SC(3116588854u, 731097341u, 35427079u, 152855963u, 655343116u, 2522648040u, 3048497137u, 3838372571u, 777022751u, 2851975543u, 235569549u, 3020787559u, 727642795u, 120522014u, 2406411931u, 4235508200u)
},
{
    SC(2533741935u, 4150033708u, 3133949860u, 2798619408u, 806119564u, 266064305u, 1385120185u, 1697466874u, 3309272849u, 2305765083u, 4237655511u, 751372374u, 3319766406u, 1139025033u, 1880631363u, 2216696728u),
    SC(531691765u, 3457214584u, 2884896024u, 292273176u, 250051106u, 4144042126u, 176967583u, 4132839552u, 2406879878u, 872979134u, 3029052987u, 2283805120u, 2613859206u, 553294045u, 1245122721u, 3840523078u),
    SC(1249934121u, 993078438u, 2897493833u, 1681305911u, 57100476u, 365202891u, 2111004277u, 4247410280u, 1628827737u, 3793711703u, 3364391257u, 3510640052u, 3346661510u, 885793286u, 3903378618u, 356572920u),
    SC(680178688u, 1413780236u, 356581993u, 2539116542u, 3091268161u, 952393142u, 3601213640u, 3759147734u, 3201912600u, 2029303323u, 3233109971u, 3469579370u, 4191225303u, 2727922547u, 4241219026u, 1108397896u),
    SC(581424072u, 2231376178u, 2556335427u, 507971440u, 4133814232u, 3831053002u, 2090051536u, 2682264467u, 1696017056u, 2590078109u, 3496563305u, 1242917226u, 2491190071u, 2058502209u, 3614091208u, 50680464u),
    SC(1148224059u, 3153210519u, 1979896166u, 3699990000u, 2774705970u, 4177914488u, 1097495713u, 3943642621u, 28438271u, 1936652546u, 2951976972u, 917798112u, 3345031007u, 3414386063u, 2086388059u, 3336786964u),
    SC(3207879285u, 3245056275u, 2753912038u, 3444068917u, 3619101580u, 301796681u, 469710494u, 37792426u, 2324375961u, 3765435021u, 2308122387u, 186365381u, 1748483921u, 2929955002u, 2507797221u, 1450081310u),
    SC(2628113752u, 657975440u, 4188527535u, 3642824575u, 1167948061u, 570005820u, 1209373950u, 3114955026u, 2156903999u, 3426648275u, 258877187u, 4116394669u, 3424577769u, 1876755024u, 3610721045u, 137959590u),
    SC(1295746957u, 2893879416u, 2731249393u, 43796623u, 1509060380u, 3580712054u, 2063633991u, 246915731u, 245935590u, 2758600953u, 1174591025u, 3759438209u, 874703696u, 3900497366u, 2032803558u, 741576512u),
    SC(737124188u, 2899307081u, 1769647158u, 617077642u, 1659909664u, 278863054u, 4232490889u, 625515113u, 3013249184u, 3621100329u, 3078044036u, 1407642415u, 2069197169u, 551433765u, 2836890938u, 3978268035u),
    SC(1956698332u, 1096426127u, 1006277939u, 3889489220u, 4030026180u, 3579514159u, 4250029335u, 2203857202u, 3553085216u, 3293255490u, 1237506477u, 1050435484u, 3944172449u, 3169627003u, 1477888937u, 2421667267u),
    SC(867315816u, 669003983u, 4033294932u, 3994270030u, 1836283861u, 4220295811u, 3981502955u, 1254544883u, 2953929766u, 3399467612u, 2767815501u, 1837724890u, 359769422u, 525366934u, 2275330754u, 1596174485u),
    SC(2757381304u, 618201396u, 1587888624u, 1754675322u, 309402992u, 1862772816u, 1766295424u, 776578164u, 3139660404u, 2518031939u, 4144540600u, 2162413735u, 2788510259u, 3413511116u, 1497090248u, 130610227u),
    SC(4221771265u, 792248867u, 928054053u, 140258355u, 1340321712u, 917602285u, 1586319677u, 1429062327u, 3604542914u, 1952132240u, 3586261493u, 1380920077u, 1224870626u, 1321897505u, 3109874655u, 2938496454u),
    SC(2321281375u, 3760646295u, 420407446u, 4154009512u, 2825227525u, 4188075686u, 2041350513u, 1285713851u, 1670924786u, 1104780793u, 3524777730u, 1315724274u, 2655303597u, 1675669649u, 3173211461u, 1286635196u),
    SC(1138423224u, 1326909178u, 3451890502u, 3840823688u, 3093921534u, 4140902218u, 2007985143u, 2980979703u, 3539657192u, 1914000311u, 3861983402u, 1995841174u, 2739822780u, 4269529997u, 1752802206u, 3674790048u)
},
{
    SC(1529327297u, 3326406825u, 3128910982u, 2593525414u, 42156971u, 3661621938u, 1244490461u, 1967679138u, 1025455708u, 720268318u, 2871990393u, 1117479541u, 1562094725u, 697888549u, 2324777980u, 3391621955u),
    SC(1194208642u, 570517940u, 3796480395u, 3996975496u, 1891180536u, 2012913508u, 2586036803u, 2779419249u, 2424448764u, 654631266u, 3378681847u, 1794600320u, 850887774u, 2610529382u, 3440406071u, 442629809u),
    SC(3922776395u, 1021134129u, 4161953411u, 3695042522u, 416696694u, 3141869998u, 2208946602u, 2248782897u, 3791212714u, 2183092330u, 2442693998u, 3821686193u, 359924765u, 1313892u, 732537261u, 3441185514u),
    SC(3832647873u, 4126820624u, 1633739521u, 1776853127u, 1990846870u, 2931750872u, 723350088u, 2100866125u, 1353427778u, 3735236517u, 2936890827u, 1037652209u, 3538242522u, 1205440750u, 2681851721u, 3428134171u),
    SC(3715940368u, 3100195993u, 139205042u, 933899119u, 508675941u, 2073279390u, 3838896736u, 762162827u, 2670162920u, 363468845u, 4142816880u, 2331633868u, 1859516459u, 2571514805u, 1415575689u, 3310370398u),
    SC(1850103477u, 2861511197u, 2158258814u, 1914352173u, 4112609179u, 1613408074u, 2229142795u, 2743410061u, 386541358u, 4131835227u, 238820765u, 350328321u, 796595210u, 325800094u, 1477199872u, 130087432u),
    SC(3503083399u, 2168288449u, 3773780757u, 707691176u, 2640783803u, 600372304u, 3521490788u, 1266639681u, 3049849833u, 3696342843u, 1559948576u, 3113774976u, 2979720549u, 3508429388u, 1393959701u, 716360542u),
    SC(2281167118u, 2404489970u, 874916137u, 3296730075u, 4266077966u, 1052198560u, 3487426822u, 379036992u, 918125804u, 3064034925u, 3007906638u, 2843799763u, 13395259u, 1525101299u, 3917909303u, 323214095u),
    SC(4272733253u, 1134926458u, 1071872991u, 1594198106u, 2743911342u, 1759781849u, 3909986783u, 357998405u, 4054491364u, 588230484u, 3248723140u, 4206364217u, 407716541u, 1660843258u, 3535395038u, 735131513u),
    SC(3679104282u, 2103136756u, 3192389130u, 3635496721u, 3762160259u, 813057806u, 1922167568u, 196643685u, 1370854030u, 2657803320u, 3197001343u, 2838705898u, 1256322653u, 3731470140u, 1658864516u, 4241135314u),
    SC(4138122573u, 1064712956u, 1914688217u, 3980579663u, 234064841u, 1340868250u, 2408246134u, 2334390091u, 3574856083u, 4185747404u, 2592066932u, 72932352u, 1132443153u, 3084950430u, 2850577555u, 531426487u),
    SC(2552518597u, 1814188589u, 3771797408u, 1688271073u, 1392417060u, 1864411028u, 2178912172u, 2411760311u, 772279774u, 2791980611u, 2940533230u, 3149501999u, 370215731u, 2968115262u, 942881455u, 2310941126u),
    SC(751991992u, 3546574605u, 2773077774u, 2498170045u, 3288367839u, 3030402134u, 1196921751u, 3823185297u, 3245569995u, 3802879953u, 493640893u, 3321821285u, 1141758187u, 3411864659u, 306737884u, 2761165281u),
    SC(1865741334u, 706283811u, 2318095713u, 1419794148u, 2504644337u, 1922210484u, 263491957u, 3084520625u, 705689999u, 2554474009u, 3818190952u, 2133768662u, 3690402460u, 3381523320u, 831084441u, 1146769937u),
    SC(831531101u, 3633896804u, 1996958159u, 636851001u, 4007892767u, 380666960u, 2826737942u, 4021398986u, 1411635481u, 515161969u, 4199924051u, 371116192u, 1868116156u, 397223417u, 972171737u, 2331326509u),
    SC(974457928u, 3569708670u, 2643527780u, 699675161u, 2627045402u, 3565281489u, 1504374419u, 2979851122u, 688725044u, 4064400308u, 4156347928u, 4119156622u, 2098702491u, 2615488234u, 1090654007u, 3790938610u)
},
{
    SC(1397828129u, 1248172308u, 2194412927u, 3657598991u, 2085616102u, 1202270518u, 3253032741u, 2632389423u, 1019922267u, 332153082u, 1521672215u, 2163564334u, 3102124007u, 582149809u, 329417494u, 188520915u),
    SC(2484409087u, 165527253u, 332794704u, 523511269u, 3524328119u, 4077596669u, 3681267981u, 2969751460u, 3456338723u, 628364217u, 4089156990u, 1135761223u, 1241363911u, 2843043452u, 1927162020u, 1187988850u),
    SC(3424784620u, 4001207648u, 1967060425u, 33527184u, 588161341u, 2216089406u, 1194534688u, 3972415390u, 3430941953u, 3671974564u, 355464831u, 2638417624u, 987848314u, 3854256447u, 2513703271u, 847178398u),
    SC(944122325u, 1095537200u, 1611102749u, 3845108718u, 3985128242u, 1188491807u, 3783427529u, 722821803u, 2594736624u, 4038805042u, 2146959275u, 3199724336u, 3631416672u, 3989329185u, 1113423723u, 925573746u),
    SC(536468163u, 2790961065u, 141113925u, 985919057u, 2438788330u, 374449238u, 2980068000u, 621714839u, 2454037345u, 2810385667u, 3189321079u, 794373297u, 4178743943u, 2630861151u, 1229894711u, 2665151675u),
    SC(71889345u, 3684655732u, 3834974630u, 40555081u, 3804280840u, 423207811u, 1620826812u, 3717508581u, 1813258849u, 713714932u, 491517868u, 2389605511u, 767769458u, 2826892693u, 3923819122u, 3331321015u),
    SC(3333750894u, 150650636u, 3555142699u, 1161199649u, 3068475424u, 1509735584u, 1033908609u, 3073273527u, 3313105177u, 3410735718u, 2770838598u, 2161939200u, 1654309303u, 1247727621u, 4123284974u, 3218452135u),
    SC(4107359918u, 3667881557u, 4099213325u, 905728122u, 3167924799u, 3731720507u, 1537227227u, 659110227u, 2101733778u, 2731849932u, 1103266972u, 887588276u, 2413509058u, 3876926094u, 2675347623u, 834362982u),
    SC(3178393694u, 2636806389u, 1832500758u, 186297941u, 3662837586u, 3282938029u, 1064394039u, 2117567716u, 95811670u, 1968831533u, 3070787872u, 2658254448u, 3676980228u, 3909574788u, 2135784404u, 3803100103u),
    SC(2624310917u, 420491519u, 3322620679u, 3357048697u, 614451586u, 1196461215u, 41516451u, 3256616699u, 3715883496u, 2257787428u, 2455669147u, 880443853u, 2246776764u, 3074399406u, 278369115u, 1177356599u),
    SC(439711555u, 2231299488u, 1079942678u, 677737570u, 563039018u, 2032266501u, 3704274118u, 1877323449u, 2386821791u, 2066266240u, 2520835526u, 1611863315u, 3800297318u, 2553770190u, 1751820038u, 2175904420u),
    SC(3515911639u, 4055231138u, 2717511782u, 6831543u, 3016647759u, 2007513585u, 1217171617u, 3815960975u, 2720128636u, 364849140u, 4285658094u, 4211508323u, 127732138u, 997100418u, 3152669382u, 146802488u),
    SC(3082714386u, 513166810u, 2182067081u, 798923178u, 921230382u, 1956178560u, 883901335u, 4290259857u, 2290170782u, 3274942148u, 2025203706u, 2950735447u, 3706997198u, 979032741u, 1714061744u, 1756952042u),
    SC(1785121933u, 665679939u, 3927612276u, 926826810u, 456860581u, 4247102861u, 1802871345u, 3111467239u, 2947918463u, 4090223916u, 2765919892u, 3848356305u, 2236940933u, 2379663516u, 2033761836u, 170415812u),
    SC(723418419u, 3083992977u, 2885930256u, 4084559514u, 3550295439u, 795067132u, 3902666387u, 98659646u, 3559229619u, 3518103022u, 3093345450u, 3504265473u, 3135355783u, 1746911831u, 3896748938u, 1982334610u),
    SC(4151598450u, 129451956u, 3923175367u, 306344029u, 336516292u, 3560777935u, 2695409605u, 934056748u, 4131395595u, 112767211u, 3377236273u, 797539933u, 516899453u, 2089210576u, 1999558205u, 4107023428u)
},
{
    SC(87353816u, 3198238907u, 1232123158u, 3291424375u, 3695263554u, 2608617182u, 3798070797u, 3966302680u, 3847946128u, 278442153u, 3929504461u, 3056452729u, 3658519828u, 643043450u, 684101279u, 121314490u),
    SC(4041434108u, 1283940781u, 3208791522u, 2974918612u, 861706326u, 3183082284u, 508820598u, 682206875u, 1177134745u, 1065833400u, 1830916342u, 1348337823u, 1877305145u, 2647094535u, 2714586296u, 2450197741u),
    SC(2726369020u, 1580548584u, 150986819u, 369792970u, 2983651480u, 3064179956u, 3511715342u, 1538695618u, 3829066845u, 578378703u, 2038030944u, 3732775932u, 1174552062u, 2377418012u, 375009203u, 1203897576u),
    SC(3480144388u, 847968760u, 3831609064u, 2454845771u, 827762235u, 3561019074u, 3068061128u, 2125290281u, 500142325u, 2613926927u, 908976630u, 461018064u, 1790330457u, 2138554260u, 3099515250u, 2668195629u),
    SC(1153226571u, 752634643u, 4102962367u, 2953166708u, 3172028384u, 1546019245u, 73810680u, 2123706323u, 2289283451u, 1736737040u, 4246735980u, 196740994u, 886758605u, 1893565373u, 3405498929u, 3744937024u),
    SC(768993978u, 3888906052u, 3538251248u, 352204151u, 4022234611u, 1471705290u, 4243963811u, 2027117811u, 1763868778u, 1322271979u, 3608278288u, 3888498758u, 3465093513u, 3125049811u, 2129222282u, 295188310u),
    SC(2552844131u, 1588346847u, 4175462227u, 3528353039u, 48525488u, 1810438463u, 342094266u, 3279671133u, 111165134u, 1329165912u, 4063411685u, 1911765579u, 2818934337u, 3808545183u, 3789526924u, 1948478023u),
    SC(3331030119u, 905985030u, 3533623355u, 799989600u, 1593247216u, 4044824934u, 3057376453u, 1132187407u, 2788031862u, 3252641138u, 1745792893u, 1362467427u, 2194538589u, 4207162080u, 1731158987u, 3426969514u),
    SC(282742454u, 1925220542u, 3537150606u, 1044967349u, 4104410814u, 3036747834u, 2170951116u, 4063975818u, 2876870249u, 40785387u, 3225638952u, 2818597718u, 1556539976u, 2301588618u, 2800555653u, 916700871u),
    SC(607531008u, 2820787318u, 1270007122u, 63140951u, 2489460286u, 3749254552u, 3480926448u, 2300022433u, 3335552281u, 3577740253u, 4083676266u, 1879037356u, 3793973091u, 653990091u, 981292091u, 2669230849u),
    SC(1168110979u, 889306226u, 331429321u, 3194220363u, 4271486769u, 2440942709u, 3008822642u, 561011853u, 2621371879u, 1149493671u, 1110535664u, 2670803472u, 394628735u, 4014155619u, 3742604108u, 1418371877u),
    SC(1139004104u, 1152838795u, 3053437035u, 3533998804u, 965296070u, 2842987726u, 3847937142u, 3591812355u, 1659887171u, 3058851485u, 1843832825u, 2284970388u, 153709291u, 2147381595u, 1241923942u, 3246474482u),
    SC(2372841964u, 95150550u, 785345036u, 3777509922u, 3777338585u, 1256811659u, 530593057u, 2218391448u, 163045439u, 4110451435u, 940149273u, 3289892018u, 1950559815u, 2046468986u, 785769535u, 229305669u),
    SC(4222560409u, 1251917359u, 3419952330u, 3518946758u, 2125025139u, 840904710u, 2104865575u, 3206919775u, 407519472u, 2004634252u, 1712650404u, 3590313236u, 840286442u, 2628712493u, 3254945248u, 1148702071u),
    SC(3313735124u, 1648160975u, 2356873919u, 1752134136u, 1812666743u, 1155388994u, 2048656880u, 513774477u, 495906662u, 2103152333u, 2943961999u, 735251223u, 2523783965u, 2210023145u, 1945848363u, 2437613245u),
    SC(1086803487u, 4028294733u, 2247710942u, 1830793111u, 1634316303u, 2935377055u, 600165818u, 1578619540u, 2988076738u, 457218665u, 4176910460u, 454493682u, 1199052867u, 1940805269u, 347367761u, 1212452462u)
},
{
    SC(3715433378u, 171840999u, 971741983u, 2238541363u, 3192426674u, 4094492328u, 467620204u, 194258737u, 3399274574u, 3279461044u, 1351137305u, 2503870624u, 193649547u, 2998335432u, 1712991547u, 2208648311u),
    SC(2715750837u, 1950207216u, 2432412079u, 3161034889u, 3163700758u, 2527560734u, 1574123740u, 2830017576u, 1235654592u, 1173758764u, 3503805913u, 3353556737u, 1972267538u, 2593804497u, 4050894516u, 1536909338u),
    SC(4252707359u, 3437282014u, 3776749445u, 203710275u, 463138159u, 2772620289u, 1182212975u, 1132575015u, 2008846240u, 1446588540u, 1178588185u, 2810502365u, 3189501211u, 3192046357u, 3703545124u, 2781338651u),
    SC(127281203u, 3251296097u, 4229877600u, 1655402395u, 2971465573u, 744237737u, 3839563968u, 1414447733u, 2055975912u, 547297398u, 3544526703u, 1086573241u, 4145442250u, 370020177u, 2948813570u, 1970539713u),
    SC(3163465607u, 792227545u, 605650287u, 3454430637u, 4436412u, 957261079u, 2917570432u, 3199157324u, 317922439u, 2607400867u, 3201779931u, 1812841573u, 973872378u, 3838606231u, 3221928943u, 461831659u),
    SC(246719913u, 1498935408u, 1945961723u, 1327338499u, 2917210822u, 1660082997u, 597934446u, 1244971072u, 662537876u, 3851981101u, 2064803568u, 1228771649u, 4273868614u, 3144280868u, 3367923741u, 2660003700u),
    SC(958115915u, 3015255252u, 3159655209u, 1681296573u, 2092702329u, 3275820278u, 1666603934u, 3861667140u, 2501203189u, 4234907638u, 1084271161u, 60369385u, 1104875606u, 3495688315u, 3738262066u, 4032927728u),
    SC(1265262733u, 3131514218u, 237040963u, 4104455196u, 2691347880u, 3487609649u, 1785135800u, 1176579745u, 4089650722u, 3141152552u, 3206481300u, 1333364227u, 276607745u, 113027050u, 176916027u, 1602590030u),
    SC(2774594376u, 3129694750u, 2287032514u, 2766750820u, 29083039u, 1069500497u, 840365222u, 3485333678u, 2555809577u, 3972967703u, 629036427u, 3011729266u, 1526288233u, 1119437732u, 917067812u, 194168105u),
    SC(592881983u, 3318575349u, 4127058062u, 1732571107u, 3503756272u, 837953701u, 482225210u, 1269788935u, 1504556881u, 1896976655u, 165783184u, 328929494u, 4077662490u, 1253488686u, 3518656631u, 977900779u),
    SC(4160682596u, 2908983358u, 1718640008u, 3588190607u, 1505225185u, 4179103009u, 1685793395u, 115536342u, 817223934u, 1402206707u, 3062750872u, 450873212u, 3409531894u, 2142975045u, 1392180850u, 3108320562u),
    SC(1943394512u, 2513880371u, 1620134863u, 1529322591u, 4060169700u, 3770293993u, 1183592156u, 3047089385u, 1457468150u, 3671110754u, 1216162597u, 2044466392u, 888112901u, 3589252991u, 523705271u, 1679814981u),
    SC(2715251449u, 70744868u, 3381212136u, 1205646623u, 2056792384u, 3523601635u, 3273403565u, 2609964048u, 1635414738u, 3927671477u, 2002719738u, 17329846u, 673666863u, 4128776449u, 1023303890u, 2113317599u),
    SC(678583802u, 2909193903u, 1603800869u, 1698604501u, 292539447u, 3194048567u, 1222053939u, 4292027072u, 1744031112u, 463670025u, 1002183205u, 880963334u, 2427537891u, 2521706813u, 3815796576u, 836594698u),
    SC(945238598u, 3719965767u, 2849528520u, 3282124488u, 1093917226u, 3479450861u, 2561471910u, 139299258u, 3917471374u, 1798050709u, 2851226278u, 2410252745u, 1571541746u, 2877491529u, 1276119582u, 4206041035u),
    SC(3869162698u, 1114491339u, 1196187395u, 1533960773u, 3407411925u, 765004505u, 1831463563u, 3761422880u, 841664315u, 226257499u, 2314441323u, 2186776430u, 2801566686u, 2703073796u, 3780881787u, 1370189991u)
},
{
    SC(3356584800u, 529363654u, 613773845u, 1186481398u, 3211505163u, 123165303u, 4059481794u, 1428486699u, 3074915494u, 3726640351u, 881339493u, 977699355u, 1396125459u, 3984731327u, 1086458841u, 3721516733u),
    SC(3675076449u, 3333909775u, 1262445603u, 3668028655u, 433069981u, 3324184640u, 206500128u, 2656010471u, 782457265u, 4053687660u, 3895856132u, 315252919u, 2755213770u, 922519354u, 2055252100u, 2429801305u),
    SC(2756940336u, 2847978751u, 1709353190u, 1195969566u, 1965491900u, 3974470294u, 4065860779u, 457378802u, 2625680435u, 4168918960u, 912437805u, 1940496017u, 2831564708u, 2681452721u, 2977785501u, 178951684u),
    SC(2809970073u, 2149172818u, 128792792u, 4173216994u, 3752778392u, 3547909179u, 2139546257u, 363162335u, 1029632619u, 226065897u, 1871318430u, 3511308809u, 4293432909u, 733440206u, 3154916386u, 2246758263u),
    SC(731502074u, 2752951666u, 3348551978u, 3130709972u, 1526861742u, 2511266125u, 4044638365u, 215744304u, 1267320586u, 1960868675u, 3421832152u, 2257930073u, 2620941002u, 851383950u, 547951559u, 1340068454u),
    SC(2684856551u, 174120198u, 1829892583u, 1225976594u, 2442169561u, 2751359631u, 1396256832u, 4190566994u, 616089248u, 1633732935u, 1633964885u, 3929429044u, 842800539u, 676918864u, 1428572539u, 219449459u),
    SC(133428457u, 620472331u, 1882141266u, 1679319159u, 679060192u, 3481716513u, 213482586u, 3423863792u, 4201383258u, 1319777873u, 927348830u, 208213775u, 4087467606u, 3653264448u, 3835415188u, 3916570843u),
    SC(1895413499u, 3284443662u, 1774671761u, 36215094u, 1302729892u, 3712548907u, 689399756u, 809699792u, 1542256887u, 1010909539u, 1793915800u, 371041697u, 3719334021u, 1415418990u, 3304256413u, 1722896741u),
    SC(4292037144u, 3413799593u, 431584770u, 554753321u, 1212891070u, 139387849u, 4633456u, 4145076332u, 2956733683u, 2226540590u, 257006677u, 3020881975u, 3400787219u, 587473979u, 260993303u, 3410840543u),
    SC(4018910540u, 3254488333u, 2078930374u, 2245837925u, 2632570996u, 3139405325u, 1623001428u, 3612462970u, 2032232089u, 519993838u, 198517548u, 1752888302u, 2236384752u, 3428944014u, 3264747145u, 2955960571u),
    SC(3519760330u, 3333709979u, 1048481536u, 1985059447u, 2643412116u, 3131942587u, 1137942580u, 1547604917u, 2831143240u, 2752062158u, 438973315u, 216212421u, 839130203u, 4170782680u, 1103599719u, 3606044489u),
    SC(3979124118u, 943995448u, 2583700510u, 3458129573u, 1268799005u, 2693058918u, 2421470342u, 2310844252u, 4161944025u, 2910466020u, 1520150746u, 2594375360u, 1025693596u, 3356457299u, 1405172368u, 3357345029u),
    SC(3608628529u, 1093067289u, 2172624909u, 336171229u, 1137437622u, 2177129887u, 3319848621u, 3625148145u, 940129946u, 3128586787u, 111536296u, 1792339610u, 2781599252u, 3659875306u, 872551800u, 2302213340u),
    SC(1104919194u, 189973497u, 2565652941u, 2930155667u, 3463454839u, 2388313768u, 2445171637u, 16202936u, 1593006897u, 2191020511u, 2084184836u, 1467463398u, 2313657914u, 2691464051u, 4089268188u, 4294499481u),
    SC(4188734592u, 3528391612u, 40836399u, 4036867171u, 4090825107u, 2939803682u, 140442162u, 2546416492u, 1084596508u, 3326586985u, 72576332u, 3780421002u, 3675044591u, 2008171921u, 3141075467u, 4288443118u),
    SC(3852374110u, 4271371075u, 2076634991u, 3101716180u, 518739558u, 3284103928u, 1607286758u, 3505817896u, 42970787u, 1339303318u, 3280473330u, 1956150319u, 790791234u, 1449585627u, 3814185461u, 3901254732u)
},
{
    SC(3892284764u, 2210224198u, 97085365u, 934022966u, 3120556498u, 264721182u, 4011343025u, 1936310374u, 2593930315u, 3833725723u, 4141640186u, 2218699022u, 3726005369u, 649732123u, 1594208266u, 3687592104u),
    SC(2459115622u, 155132544u, 2344650987u, 2337329027u, 2478875455u, 1363777389u, 666384305u, 779524970u, 131624810u, 1099629813u, 755087667u, 1116544707u, 3462583113u, 1765615231u, 1221263451u, 345614861u),
    SC(283432140u, 3102718597u, 937211953u, 3334135604u, 2242058317u, 3044145753u, 1441000856u, 2163904099u, 654999768u, 3976748269u, 4108102772u, 1209693616u, 3022484925u, 2592361118u, 3806239715u, 2457345174u),
    SC(1983572202u, 34789206u, 3963513429u, 2661898079u, 3999779459u, 2657216026u, 2570146353u, 810465768u, 1310539449u, 3517224567u, 1830164911u, 2328664885u, 3323158486u, 200812613u, 1588943475u, 1631047872u),
    SC(1996456687u, 665652044u, 360516388u, 3634015955u, 3932508085u, 3762889476u, 2869080596u, 2179691892u, 1880327422u, 3850327759u, 1653803674u, 236673399u, 2154944705u, 3229042401u, 2981554507u, 485288416u),
    SC(264936494u, 3091907543u, 2050111855u, 2694936127u, 1954787063u, 722933256u, 3813405263u, 739130277u, 2256053561u, 3585156690u, 2029190911u, 3133350308u, 3458910883u, 3499638057u, 41852560u, 491183838u),
    SC(2808085465u, 1288453772u, 2477084166u, 3837131567u, 1141955368u, 3112183866u, 1372456734u, 2203526963u, 2954171016u, 3969349716u, 2868857569u, 414601865u, 4013256181u, 468368341u, 1996835394u, 3658768313u),
    SC(394302887u, 1097097404u, 3291468368u, 1194224926u, 1035172467u, 1541144594u, 3844885672u, 3479557309u, 3116596876u, 2815221788u, 2598284757u, 360029902u, 1618439794u, 2569763994u, 3258655905u, 2917038348u),
    SC(2305403224u, 515881048u, 3401955316u, 2294640138u, 2523482065u, 2913659188u, 1840514079u, 1334322081u, 1545396585u, 4197671987u, 447162882u, 3846426473u, 2663235502u, 750784192u, 4164775689u, 2390294077u),
    SC(2816642384u, 3952759529u, 3784236377u, 1797857230u, 1881467157u, 3886776601u, 754213935u, 2085935272u, 3814437883u, 3598631313u, 3014087408u, 1480756254u, 2838244491u, 132661795u, 909841870u, 675503551u),
    SC(2053456581u, 627096201u, 3974668317u, 245144267u, 3845450294u, 1209560693u, 1003623636u, 3431474873u, 3952764341u, 3855863791u, 1357940588u, 3374805012u, 2942824193u, 2988435703u, 329942625u, 4139666589u),
    SC(73006928u, 4068145413u, 2752900485u, 643186737u, 2386201439u, 296363448u, 2965535934u, 2202307569u, 1300692310u, 3766694667u, 2421404412u, 2295288621u, 1987219755u, 3682346025u, 885571108u, 1086202535u),
    SC(3800801259u, 1729576293u, 2024334221u, 266315944u, 3877353536u, 2983817286u, 1164606138u, 2981999790u, 2626097845u, 3537364374u, 3559786635u, 2149380619u, 2137897542u, 2218263339u, 206251476u, 3754285811u),
    SC(1009857555u, 1650586423u, 3853695002u, 1715580147u, 1146669099u, 1380681899u, 2219018152u, 1791877891u, 3247738482u, 1042579957u, 4035547117u, 2619207487u, 2408116465u, 3045899420u, 1771645449u, 1340019342u),
    SC(2004305920u, 978372350u, 1705342765u, 503429310u, 3635208103u, 3659317811u, 3957481997u, 297103567u, 2521968324u, 599616959u, 1167498361u, 357125999u, 3158983160u, 3114128384u, 3086595483u, 2336612985u),
    SC(4103187540u, 1182325894u, 97419735u, 1615223731u, 2031918136u, 2818146326u, 1038685355u, 1330155299u, 2657284062u, 4126074186u, 2871281156u, 2738191090u, 1922990674u, 2689532011u, 4040564095u, 99693623u)
},
{
    SC(3639643416u, 3974502485u, 1527161781u, 180938703u, 2788643910u, 3418867931u, 2912046968u, 1776807950u, 1185488163u, 2433308651u, 3682797092u, 1938004308u, 753534320u, 795320477u, 3620835863u, 105275502u),
    SC(2971894151u, 635573958u, 1662864280u, 3637757763u, 1966418418u, 2382544768u, 3521712538u, 4180511568u, 1216311665u, 1622710591u, 2836323703u, 1065095206u, 3046512769u, 2304432132u, 1370910091u, 3540050165u),
    SC(3003078502u, 1266710982u, 63268125u, 3769826631u, 2161222028u, 1624738852u, 2999285769u, 2485757266u, 3350017650u, 1836975640u, 3947916645u, 3226839039u, 3416803572u, 2607406281u, 3224012241u, 1574498192u),
    SC(2417128114u, 3148595382u, 316383238u, 491687931u, 3782721648u, 71265990u, 725842943u, 2574280796u, 2910592942u, 1266732336u, 3293910730u, 3812834954u, 758280869u, 2044998492u, 585388705u, 2220041893u),
    SC(492257517u, 927280821u, 3326474467u, 3418658462u, 175063450u, 4228793954u, 2332128647u, 2793872080u, 3349562222u, 3060602442u, 1750735766u, 864506271u, 3021446456u, 1089650280u, 684313887u, 2273360774u),
    SC(569437869u, 3392548160u, 448456633u, 786222873u, 1891470348u, 56622530u, 1988234620u, 1200550357u, 3540465428u, 1566012807u, 3682627310u, 3118219502u, 421481320u, 474517348u, 4276632114u, 3506654966u),
    SC(200012878u, 1289466640u, 383837247u, 2978212823u, 641013196u, 1218428129u, 2429292619u, 1428313217u, 4155302101u, 1036892035u, 3775206351u, 778853475u, 3322870631u, 4195074838u, 3725481759u, 3550082329u),
    SC(126839072u, 3914304851u, 1035784989u, 2867617428u, 1989254908u, 3724484330u, 1316610484u, 1040102649u, 1452719164u, 210631948u, 1224888518u, 1113840153u, 910511278u, 2297844676u, 797967535u, 283877762u),
    SC(1244500121u, 2493482314u, 3779000024u, 2685901143u, 2759844693u, 2465008309u, 2989069530u, 1046572576u, 3374497605u, 2414541412u, 1726159904u, 3650454710u, 2872643374u, 1536622747u, 1381290537u, 3538573283u),
    SC(1982773073u, 895953548u, 653968243u, 2944168854u, 1891156211u, 862699673u, 178384938u, 2122337777u, 3992617936u, 1827424625u, 1827918311u, 4247768891u, 2116109311u, 2389157370u, 3259962586u, 3018719650u),
    SC(16401953u, 2306633926u, 2338480543u, 3225473112u, 3429377887u, 2444554167u, 3036218027u, 811186210u, 2350667613u, 3590742085u, 2594672781u, 575072326u, 272468093u, 997542396u, 3031146350u, 3776453205u),
    SC(1784787552u, 1031272746u, 3302965053u, 805306745u, 3874552409u, 2790720051u, 483200429u, 1779723984u, 1097599486u, 1897611475u, 2456960784u, 1754250527u, 3808506348u, 3902842183u, 2596972722u, 2928554842u),
    SC(2323692909u, 829274841u, 1103316386u, 1866432209u, 1938371795u, 4027514213u, 3989131198u, 2637747342u, 2193562562u, 1183535102u, 290853894u, 707762868u, 1909722738u, 2733745164u, 2354524179u, 94921256u),
    SC(390966983u, 2005348047u, 1183001210u, 3460046175u, 1194344520u, 3385791048u, 306982602u, 876126480u, 3192052847u, 3055117485u, 1493712024u, 239443620u, 3677526258u, 3935077241u, 3195438491u, 2508943164u),
    SC(3776157658u, 1760005001u, 3371368706u, 4151959572u, 4117952947u, 2782084300u, 3075220020u, 3130861900u, 3220322643u, 4251107806u, 2765944679u, 2454606920u, 3864173523u, 2241965276u, 1056706189u, 2253371852u),
    SC(10455103u, 669421195u, 538798805u, 681593482u, 4243109638u, 2765550308u, 1560790187u, 2332940655u, 157674749u, 358872640u, 2549359913u, 811329072u, 318369228u, 2192271276u, 2616093049u, 3105543667u)
},
{
    SC(3392929934u, 3483303263u, 1976307765u, 4193102460u, 1186037029u, 2559946979u, 3008510830u, 4008303279u, 2792795817u, 3991995u, 311426100u, 3736693519u, 1914150184u, 2000710916u, 1829538652u, 896726226u),
    SC(1506989834u, 781231698u, 1423994091u, 932436763u, 2811140941u, 235158077u, 3312925598u, 1277169313u, 2161654787u, 95045550u, 2507009285u, 3400899479u, 1327874861u, 2641030305u, 845165129u, 3067306163u),
    SC(81377829u, 4112377516u, 996390415u, 1466127523u, 1087938057u, 1370439327u, 2374941315u, 3221315808u, 35184362u, 4155013651u, 4157224703u, 3036174627u, 820839223u, 644204168u, 3814924360u, 2548030643u),
    SC(1091124676u, 3446444543u, 108918031u, 285417020u, 1457053816u, 2518578419u, 3204558864u, 1447981867u, 3090612039u, 774503865u, 3344583272u, 2737274269u, 3562442510u, 1127429989u, 2804182977u, 1775681652u),
    SC(2318905039u, 2047942274u, 566069924u, 123115342u, 2915025724u, 2614503051u, 611479778u, 1680640702u, 111791999u, 3565934367u, 3623017458u, 358904698u, 718271833u, 2594429479u, 2455462208u, 1049889789u),
    SC(2072590390u, 2994175732u, 776612573u, 3305897523u, 938985307u, 4037860099u, 405398386u, 312125617u, 834030222u, 4269222652u, 3952042783u, 188369721u, 969558599u, 2241466312u, 1494637662u, 3640394545u),
    SC(793329188u, 1680204464u, 4194525713u, 1397937237u, 2203558613u, 193170132u, 590149348u, 3837254789u, 2629901211u, 1547324833u, 4256276761u, 178627910u, 1204782838u, 3049171442u, 2847310157u, 1633221731u),
    SC(1445130399u, 3305816299u, 706740166u, 1986021205u, 2637844550u, 1419078314u, 1678054887u, 2432697110u, 870544859u, 890225672u, 4294515721u, 4251895411u, 1276311012u, 1177847908u, 2958585231u, 4245816799u),
    SC(4225912221u, 703507803u, 1922376483u, 3748563847u, 841832204u, 937238929u, 1762562329u, 2321245641u, 3396851205u, 4196168123u, 2898493537u, 4105193320u, 3913075709u, 3714213782u, 3736794417u, 1813506206u),
    SC(473058800u, 1281200026u, 2096535567u, 1916392924u, 2499055699u, 1592813861u, 1665248526u, 1352252079u, 2539722497u, 3800235497u, 2456011531u, 2486813252u, 2969323588u, 2786889819u, 264256920u, 4162650714u),
    SC(4093970658u, 1112717313u, 4105391438u, 692152127u, 3191447576u, 765356874u, 3774754898u, 3659714922u, 1417146611u, 4116649329u, 2382824064u, 4091923584u, 2943998996u, 2572469258u, 2350556732u, 4055180934u),
    SC(4241530692u, 3958450744u, 2400383404u, 466315350u, 35062538u, 2419973666u, 1574066566u, 718969713u, 2103427683u, 1844215170u, 377438369u, 3472936858u, 4219642124u, 2727593550u, 2415179286u, 530554266u),
    SC(1717990860u, 490767589u, 4104938990u, 1912533482u, 1727757083u, 4081637760u, 2971627803u, 4227474711u, 2482396781u, 1077462396u, 1040490667u, 188422725u, 1078987146u, 1905877850u, 3465315863u, 3779881072u),
    SC(2343360099u, 2602377036u, 540592495u, 3215700530u, 2276091252u, 330543342u, 1521140429u, 3101043196u, 1353643940u, 4257187260u, 3766970644u, 3977679607u, 2139641066u, 2691703488u, 1191064988u, 3899819176u),
    SC(4020334744u, 3662481612u, 4168714619u, 3391835711u, 3785299560u, 71469795u, 2493742903u, 3412561168u, 3292204549u, 1481564183u, 2157273751u, 477496008u, 931448839u, 2827709521u, 2133135454u, 3513095854u),
    SC(1821292885u, 77067071u, 2713776553u, 2767520127u, 1059460035u, 985220275u, 2884538737u, 221640066u, 2657382407u, 232264137u, 3155923068u, 3788271780u, 2919723565u, 1308585734u, 3615447351u, 9588952u)
},
{
    SC(2320406161u, 892569437u, 3092616448u, 1707673477u, 2810327980u, 4012118332u, 4142748730u, 3869507620u, 92116036u, 2366184953u, 1613655167u, 3287845172u, 3562699894u, 416962379u, 1296831910u, 1764080884u),
    SC(220529260u, 249394787u, 707093586u, 3327680194u, 3905189366u, 612327964u, 3292761054u, 3030686883u, 1334491337u, 3207860077u, 3280619568u, 1041320647u, 2483468975u, 1479881667u, 3211575507u, 3039423798u),
    SC(2075210586u, 859890386u, 3979249840u, 1571749934u, 1787834945u, 3779262932u, 3834468444u, 2848979155u, 3949299214u, 3265482052u, 521566179u, 4090178483u, 2634506734u, 537774764u, 1760986104u, 1885781444u),
    SC(2157623553u, 1245488719u, 2108443037u, 4226304849u, 1701247415u, 4110744868u, 1746909616u, 3191493799u, 846028927u, 3826268145u, 3155840342u, 1303740777u, 3325552898u, 2580884535u, 3592783405u, 4209959030u),
    SC(535271984u, 3867256577u, 2621667187u, 479852461u, 3031868718u, 681291605u, 3866870888u, 975222367u, 189285295u, 2489945122u, 4002580885u, 1631683077u, 2806354223u, 990581176u, 3013857114u, 805874285u),
    SC(4221232460u, 3061114345u, 3434676469u, 1406782470u, 155821803u, 124504941u, 3888697140u, 2788501814u, 1026476732u, 2216503728u, 3089015914u, 2063998098u, 272392246u, 1587339314u, 677528523u, 2432699241u),
    SC(3643892943u, 4282202220u, 2100563362u, 826776443u, 1365722925u, 2702305724u, 679208928u, 3149950187u, 1446692720u, 2990196076u, 3121167752u, 25041546u, 1204401671u, 3950457476u, 478874733u, 4191001246u),
    SC(1002796340u, 395169719u, 3087599283u, 10336612u, 2123927609u, 504611529u, 4163730275u, 706425703u, 1588733263u, 4149509341u, 1952228143u, 3819719132u, 766367752u, 1435203845u, 1906598194u, 3492363785u),
    SC(1774340829u, 3089482890u, 2870005976u, 919794943u, 2035504962u, 4034646005u, 3486869666u, 3458779364u, 2688966610u, 4246698276u, 241215855u, 1193302498u, 1307583268u, 129792487u, 301354381u, 2759318534u),
    SC(1993945167u, 2379081822u, 2587040362u, 3154537819u, 1926143939u, 2749781524u, 935556830u, 4138641196u, 1781637476u, 2939621229u, 45782825u, 4247420511u, 1775642409u, 3169645376u, 1224651656u, 1411268824u),
    SC(4099217380u, 332485632u, 702660355u, 2932600301u, 2644542769u, 1705216342u, 2043283695u, 2373746705u, 2092217219u, 1660104946u, 3159676245u, 3674605841u, 226100099u, 3987250021u, 2436672589u, 1083744721u),
    SC(775618835u, 2173251804u, 4192653515u, 3582997173u, 3769245096u, 484007740u, 503088416u, 1360222738u, 586791868u, 3760447547u, 3490651251u, 3534666198u, 2531156474u, 1207301882u, 832959081u, 3020069982u),
    SC(298341207u, 1349761730u, 1369831393u, 1101983922u, 2409775356u, 3892600618u, 3875266737u, 3482966490u, 4002034047u, 2018792567u, 1932407387u, 1184232926u, 3015567427u, 301694942u, 437132459u, 3636206614u),
    SC(4090425889u, 2348669465u, 2575850637u, 3995997864u, 3040420324u, 1615191584u, 2490849366u, 2670494936u, 2841563080u, 3763919842u, 3580970157u, 3864708123u, 187158351u, 2199194387u, 4160227448u, 2176418944u),
    SC(3040328915u, 1001466289u, 3676795030u, 2946692141u, 3593888463u, 2224708622u, 4148397123u, 4253879884u, 1993280384u, 1176406404u, 3148404923u, 4180061590u, 1786680964u, 4036906941u, 1164279397u, 3562714780u),
    SC(1286200509u, 4232891464u, 1656861418u, 3412215448u, 1086562483u, 2512121988u, 2650588176u, 3097245464u, 3192968944u, 2220731064u, 3414522916u, 4204353060u, 3690514744u, 3688465060u, 2246470987u, 498255717u)
},
{
    SC(1167035839u, 2632944828u, 1562396359u, 1120559767u, 244303722u, 181546963u, 2941229710u, 561240151u, 1460096143u, 346254175u, 110249239u, 1849542582u, 1293066381u, 147850597u, 3876457633u, 1458739232u),
    SC(3571928080u, 2436259038u, 1291130511u, 4109706148u, 535321895u, 223400632u, 1981907545u, 281269666u, 3986674262u, 1137333737u, 1403128295u, 1607985509u, 1996916063u, 3564990547u, 3398899933u, 2822030993u),
    SC(4187142002u, 2183119934u, 1635192887u, 2899344980u, 2532710469u, 3583070294u, 1537984623u, 296183513u, 2324170481u, 3475303187u, 3887648540u, 634736823u, 1254765115u, 3808584578u, 3772430219u, 561684376u),
    SC(513372587u, 1759503751u, 4262413842u, 2894839952u, 1546497784u, 1634597484u, 3075497476u, 1112503488u, 1318854936u, 1645523550u, 1808408161u, 1471049890u, 1607196116u, 1989192912u, 3845591311u, 3230210229u),
    SC(4281800629u, 256065360u, 161761292u, 2162610453u, 3289868207u, 803664088u, 1737988317u, 3468667062u, 1313091619u, 3871261661u, 4163576187u, 3519070773u, 663580583u, 2181685257u, 1282501745u, 373224564u),
    SC(1305532007u, 4040631353u, 3016994284u, 364840424u, 312087064u, 2832713285u, 813363164u, 1634515727u, 2857968226u, 2482770921u, 2702964276u, 1457003903u, 4233117491u, 978467573u, 454990490u, 2451215822u),
    SC(3309788844u, 1373644165u, 2568421202u, 4021050421u, 3214613315u, 3179866441u, 2282215282u, 4192353052u, 766132975u, 1427735093u, 3905164154u, 3510365574u, 3650419996u, 1208798186u, 2311177541u, 3425106727u),
    SC(1485656607u, 1872571460u, 3807266779u, 3227427836u, 1367154025u, 2087101352u, 2787930808u, 1683647111u, 611621831u, 1033465938u, 1055561737u, 1718623444u, 3674681330u, 3643294293u, 3841507882u, 2950124804u),
    SC(3583452191u, 43558840u, 2702416786u, 2831018419u, 4179535508u, 3293628424u, 3781032090u, 4272940814u, 1561835153u, 3434531879u, 2033417772u, 143682419u, 2206689113u, 2885101743u, 3330838914u, 3213033967u),
    SC(1563269339u, 3268845808u, 481878529u, 1366255066u, 188999428u, 2024859095u, 3740130866u, 1902201859u, 3294724532u, 3498902869u, 2063801661u, 3851840419u, 1697955856u, 1216829830u, 2472036433u, 2158918739u),
    SC(3706632627u, 1854832685u, 4075722340u, 3009760070u, 1947919686u, 1613829674u, 3359356634u, 160149010u, 3211678034u, 1403957074u, 2395316449u, 232911190u, 3595342115u, 593590477u, 4003146812u, 1042747586u),
    SC(3566751331u, 1293366329u, 237055278u, 781035984u, 3490518265u, 471671502u, 3279573882u, 4088428685u, 3341570902u, 1660948465u, 2602036180u, 3189056267u, 1448251311u, 3378653995u, 367559448u, 1247557023u),
    SC(332188181u, 124235367u, 2908363616u, 57405667u, 3860321591u, 2915594808u, 3193053797u, 3103490367u, 2893876952u, 791722516u, 2759950240u, 2647310599u, 1060814304u, 1104815755u, 3283917665u, 954167246u),
    SC(3633439037u, 1737408037u, 3240746577u, 2032524778u, 210349431u, 1157873376u, 3552462955u, 3068823u, 2593869163u, 1645741574u, 2624282012u, 1595174943u, 3150496822u, 2635369792u, 3670346328u, 1317499755u),
    SC(3066163224u, 734815666u, 3189326611u, 2603442644u, 551273493u, 3201260612u, 896218759u, 1203901890u, 3082479753u, 4206490018u, 1615910957u, 3112412856u, 3354260034u, 1776181406u, 227950091u, 2452682654u),
    SC(2235295503u, 3336503999u, 656069002u, 1855251063u, 1400966644u, 100804460u, 3316705750u, 794158471u, 3220130150u, 1524496317u, 4024763824u, 915138624u, 1872936127u, 829155670u, 1406327784u, 3285915916u)
},
{
    SC(3539989726u, 2664422354u, 3717852078u, 3493347675u, 431408204u, 2534904428u, 166307432u, 1071633271u, 2817060747u, 2307358268u, 3433391820u, 2071844151u, 219511979u, 303896099u, 3062367591u, 2892429963u),
    SC(4169968731u, 2129799654u, 437437237u, 369342547u, 1225909990u, 105177072u, 378686654u, 1403688950u, 3897807924u, 3252342965u, 1215424641u, 560413328u, 1897408132u, 317929004u, 3828647679u, 1630564758u),
    SC(2120346993u, 1574861569u, 4055542703u, 3156063114u, 2155135979u, 3395705935u, 3607950162u, 1649229112u, 1891339524u, 2871189526u, 475543260u, 4035849276u, 919486311u, 4103998043u, 2581732188u, 3337457769u),
    SC(2650342494u, 2112594502u, 300482146u, 4214370423u, 3712572735u, 2394678491u, 944484075u, 2859174140u, 1298074617u, 4123981874u, 2931863188u, 4060402101u, 408241016u, 1141274074u, 2343754010u, 2412599648u),
    SC(1561545950u, 3513590208u, 46110254u, 2131948246u, 1318148204u, 2154872738u, 1632214749u, 3758828119u, 3082206346u, 1424038120u, 2361241545u, 845137641u, 307971779u, 1724404993u, 861282060u, 1237934782u),
    SC(2774909901u, 771645224u, 1285073837u, 2193431137u, 1992145786u, 1323638656u, 695741715u, 2225025760u, 1506694954u, 4281622541u, 648809495u, 1264275594u, 2179049970u, 2134563430u, 1143161913u, 1676304803u),
    SC(146493114u, 1026262009u, 3602767471u, 2183478058u, 1903997235u, 4037497130u, 232766761u, 3333583275u, 4037065903u, 338762279u, 3658077565u, 3465013868u, 2987748329u, 1503145496u, 1553131083u, 2250198737u),
    SC(2341715858u, 2700579248u, 3859696179u, 2395756825u, 1875611477u, 3083700335u, 3413235310u, 1368601544u, 2011324934u, 2489277894u, 3393073269u, 1479863073u, 1546719681u, 1270920228u, 832404816u, 4096637834u),
    SC(3098090164u, 3937526885u, 3922595589u, 3117243593u, 3619511456u, 687964457u, 2049777986u, 2737216841u, 904576627u, 2497431372u, 3782524472u, 2176150332u, 3538905622u, 1249874595u, 386091287u, 597337724u),
    SC(653517061u, 2613638042u, 3043803086u, 3430911227u, 3939946327u, 3394071887u, 1634025406u, 422896314u, 2056719107u, 2825344479u, 4064697313u, 3122017483u, 3752686726u, 3984230999u, 2989927946u, 36279219u),
    SC(2977387875u, 1756856293u, 2305658602u, 3898809838u, 2022534013u, 3053356239u, 1719149320u, 1006974664u, 3980567886u, 911250528u, 3970581037u, 4208855094u, 2375475175u, 3461024498u, 4207299460u, 172606632u),
    SC(2123341088u, 2610619360u, 3636249805u, 2405928311u, 194895330u, 4166746397u, 1666551241u, 3089845290u, 830253287u, 1769367456u, 492844122u, 2898915009u, 1465071417u, 1748645392u, 3136192983u, 3149049830u),
    SC(182090295u, 2773063932u, 2875617227u, 2014878906u, 4034576690u, 3504190878u, 648632813u, 578906269u, 3395653562u, 3622802446u, 1642118462u, 1105217635u, 3484288771u, 4187487776u, 3066363798u, 3248936252u),
    SC(154149828u, 3967951687u, 1435057545u, 77065166u, 3232269485u, 3912916706u, 592527655u, 4277917673u, 3417904405u, 3905839920u, 1437307359u, 2532079592u, 1386597940u, 4043192840u, 828125384u, 1712244674u),
    SC(4144828863u, 1262971610u, 2738002832u, 3848745747u, 554156666u, 3660926287u, 1405749523u, 293551868u, 956195932u, 2061195588u, 3476646641u, 1003448777u, 4182963546u, 1462193925u, 2827901865u, 1370898532u),
    SC(287054389u, 4206061741u, 3909899140u, 2957058664u, 2712205523u, 1231432323u, 1252507865u, 2198483068u, 3163354130u, 595880373u, 2050058791u, 535083586u, 4093274722u, 251534866u, 1425149793u, 2349787856u)
},
{
    SC(3015000623u, 325176924u, 3212623969u, 1014540936u, 2686878702u, 3453922035u, 257234635u, 689320672u, 395365200u, 3425465866u, 3351439740u, 3293249321u, 2261203941u, 1504215424u, 2365812346u, 2486464854u),
    SC(2802351214u, 1019547153u, 1581443183u, 2237644987u, 2316167912u, 1277137594u, 922833639u, 1775757119u, 2259030628u, 3320484395u, 3474839377u, 3039388985u, 3157017009u, 701728799u, 45087422u, 1375130067u),
    SC(1408178651u, 332882372u, 2572930650u, 1429622838u, 3740348959u, 3769865143u, 1102404486u, 2395773863u, 2055053046u, 1642858333u, 434575788u, 1458579645u, 1077283311u, 3435370625u, 412513198u, 1108997u),
    SC(166351317u, 1290556120u, 1492697218u, 3828755332u, 1787027698u, 2627329842u, 818520792u, 3844511768u, 1093689215u, 2840813230u, 4268955351u, 1793367442u, 1197897289u, 1467402002u, 558600125u, 4039642298u),
    SC(2618143148u, 4195387407u, 3571081448u, 176847982u, 3021045559u, 2151239299u, 4216918791u, 349987936u, 1438071630u, 2148079477u, 510134808u, 1844452199u, 3473619148u, 3775643892u, 3701006526u, 2069649956u),
    SC(2536827719u, 256373429u, 82685205u, 2031847695u, 1685669223u, 3749398630u, 3100433967u, 2559626296u, 2614261735u, 2095898325u, 2650411530u, 4139725354u, 2433652522u, 1465137472u, 3074463995u, 2942034210u),
    SC(950856594u, 2511634642u, 447889167u, 3271534101u, 3998181635u, 850059409u, 1500318444u, 2845728509u, 2319192144u, 1285732158u, 3307511706u, 1860111207u, 106597122u, 1317987028u, 3909997475u, 2833499319u),
    SC(197466102u, 106471666u, 3969627291u, 425148315u, 2088018812u, 3287551129u, 2083642145u, 386904296u, 2967132086u, 417456225u, 2418726206u, 2685222098u, 3920069151u, 388803267u, 1008714223u, 4223482981u),
    SC(1730602173u, 1587573223u, 1136504786u, 801576255u, 1239639300u, 3897044404u, 2640640405u, 3098571739u, 2095045418u, 1782771792u, 2216047065u, 2006450887u, 1019963460u, 450135304u, 1704523436u, 4178916267u),
    SC(3045516080u, 2837283309u, 3652809443u, 3617799274u, 2953845221u, 1870697859u, 1987277049u, 671334013u, 2347392220u, 1637733040u, 408564290u, 531095235u, 1714215546u, 2668823252u, 4291679007u, 1499030154u),
    SC(1785804164u, 3771923969u, 1688952513u, 4078905240u, 4219818381u, 2140263698u, 3560443409u, 1027592498u, 981877075u, 1273450409u, 1808708945u, 366130160u, 1509712333u, 1419790056u, 3592515372u, 1023304152u),
    SC(689558936u, 2052202277u, 1573780309u, 1046114431u, 1768897198u, 1193436549u, 613072153u, 961650488u, 3203433527u, 2587127126u, 2088764244u, 3898254742u, 1779313411u, 2448405043u, 2102013432u, 2635393468u),
    SC(2025692259u, 905848568u, 1759010770u, 1792571870u, 4118995060u, 266283808u, 4139640706u, 3438115348u, 2780184652u, 3445643695u, 656585512u, 181166262u, 2272629776u, 370943424u, 1751557846u, 2309122167u),
    SC(267180733u, 424783777u, 1080203254u, 2661909603u, 1424050736u, 3737445342u, 2397112235u, 1140319020u, 3540605726u, 1560404816u, 714090654u, 3305695922u, 4001926073u, 4235374954u, 2250613806u, 603974704u),
    SC(244840167u, 1554020100u, 3702066775u, 2862773506u, 3785435454u, 3651035430u, 218349583u, 1404753202u, 3766478445u, 2586133471u, 1533117238u, 4149938439u, 2210912076u, 3594357012u, 575816505u, 525962129u),
    SC(4146528898u, 2136081288u, 1410528199u, 2682243562u, 3659634297u, 3884779676u, 1276188622u, 3650143718u, 2534539131u, 69352587u, 4188728680u, 4144009400u, 528573366u, 1948891771u, 2778384350u, 3961787045u)
},
{
    SC(771871546u, 3238832643u, 2874232693u, 1176661863u, 1772130049u, 1442937700u, 2722327092u, 1148976574u, 4122834849u, 744616687u, 1621674295u, 3475628518u, 2284524224u, 1048213347u, 4058663310u, 153122870u),
    SC(2125145888u, 3034373129u, 148397811u, 141146887u, 2520820550u, 761993323u, 2298029094u, 2891332110u, 2829144983u, 2531560926u, 2167918181u, 3311166313u, 1986747894u, 2110826144u, 1833688282u, 2697250572u),
    SC(3869871954u, 4004844136u, 2445592287u, 191554676u, 1824322074u, 1934754654u, 1806989779u, 631655906u, 1640478312u, 3779394326u, 3878618879u, 1897296401u, 116845712u, 1282189569u, 1638341398u, 253193742u),
    SC(869049848u, 3185853214u, 1086566153u, 574813225u, 768296876u, 2336838903u, 1037196762u, 3581040974u, 1545806877u, 1185761684u, 533220394u, 2594450382u, 518321105u, 3416686830u, 2271268151u, 3918676320u),
    SC(3856331543u, 2684505765u, 649861433u, 2052378851u, 4281491040u, 1056350427u, 1268888422u, 3791019043u, 2372988231u, 1754646015u, 3964172838u, 3080977165u, 1940074122u, 2762476976u, 3389041795u, 1131517310u),
    SC(1630655860u, 1949945516u, 3883647184u, 3029959080u, 1311781856u, 408642488u, 2800393690u, 3410356207u, 115351401u, 3420630797u, 2709679468u, 2872316445u, 1790203899u, 1997501520u, 3278242062u, 551284298u),
    SC(2323279372u, 1575922229u, 4047150033u, 1372010426u, 3148623809u, 2453870821u, 2339486538u, 2280451262u, 2466099576u, 2994948921u, 132102763u, 1776872552u, 3906687848u, 1416385780u, 2716658831u, 3839935313u),
    SC(1482060017u, 4064599659u, 4201421603u, 1862488009u, 1206323034u, 1506270647u, 4148487892u, 2940354206u, 221477839u, 2184047858u, 1052602625u, 1800724448u, 2376949890u, 1248004043u, 4042069004u, 1001474649u),
    SC(1973975072u, 2109156381u, 895285550u, 2806725496u, 4257596779u, 2294716595u, 2126073388u, 4029509053u, 2287557214u, 3863235224u, 910675328u, 3403565516u, 2460443864u, 4145068647u, 1675629270u, 2972605807u),
    SC(3067953236u, 2487048107u, 1053067642u, 2406833819u, 1120120518u, 2019615106u, 2151977185u, 2444444329u, 3698388134u, 2675794597u, 2346696087u, 3691916163u, 416413840u, 2548582733u, 2519917531u, 3323365251u),
    SC(4258867839u, 1450083676u, 3423817219u, 2338254228u, 956448310u, 2038800503u, 2270893323u, 23474499u, 4001071451u, 434241187u, 4225947271u, 3009484949u, 1212186223u, 3021170789u, 3408787844u, 4241328442u),
    SC(544425045u, 2335106449u, 1970249987u, 676962447u, 2451092807u, 3397085111u, 644609608u, 622894566u, 3012162452u, 742316904u, 1183695331u, 1942632009u, 3993963459u, 2025380463u, 2934502595u, 2424729664u),
    SC(489227787u, 2064607364u, 749046162u, 1223089239u, 4103152782u, 944881113u, 2156101348u, 2809656549u, 2750173639u, 2290439348u, 455194332u, 3662094961u, 2388553957u, 2373693996u, 3087294434u, 714908241u),
    SC(844100070u, 1293873339u, 240400805u, 2741251793u, 4185619158u, 3756747900u, 2600026127u, 4095003808u, 2551250677u, 1982555415u, 1538344606u, 2598805396u, 1759235723u, 1251966u, 1750681115u, 626531732u),
    SC(3996016258u, 3876613311u, 1191787057u, 3901742282u, 1577096572u, 270596184u, 3165567618u, 4061944625u, 3613068329u, 3912630805u, 2056061785u, 2568706449u, 2343664228u, 1807908509u, 1314728487u, 1028342757u),
    SC(2729604648u, 2866824008u, 1921075953u, 959207538u, 460881358u, 1786258799u, 989199155u, 1140694999u, 3534517067u, 1671080238u, 1077292982u, 69981150u, 2456995550u, 2177711190u, 3355630373u, 505438766u)
},
{
    SC(2470971363u, 1622646280u, 3521284388u, 611900249u, 53592433u, 1667691553u, 3986964859u, 3228144262u, 4160240678u, 1357358974u, 796266088u, 2135382104u, 2999113584u, 425466269u, 866665252u, 3795780335u),
    SC(1943673032u, 163567132u, 2998325065u, 4151760187u, 4286963295u, 2037110896u, 4023804057u, 2843670454u, 4267379728u, 470850548u, 1360194572u, 542908383u, 117354082u, 3909600634u, 3301531838u, 585104523u),
    SC(421763950u, 3621776882u, 1804759030u, 1922063749u, 28357531u, 2718763721u, 3528327041u, 2594458380u, 1745913977u, 1705774731u, 3785007083u, 1889010688u, 4275556992u, 2808027536u, 1706627542u, 967259307u),
    SC(3761989171u, 2069950976u, 953323220u, 30139149u, 3360357391u, 466334029u, 1085748790u, 717259079u, 3822910993u, 1348849055u, 4159668773u, 3924702853u, 4257335520u, 1714446370u, 3394938265u, 2541598048u),
    SC(2132231371u, 3951042779u, 332537683u, 2179456991u, 3112576172u, 2873883577u, 502046554u, 4014018248u, 4272356370u, 2124475345u, 3140973257u, 1234959848u, 3468807232u, 3812306463u, 2768101189u, 3493652974u),
    SC(2983624056u, 158967077u, 546553405u, 3473936990u, 3742593866u, 3986716933u, 2905591308u, 285301696u, 2640868047u, 3062221467u, 70156428u, 150492378u, 3977001273u, 1087159682u, 1233481348u, 3391921638u),
    SC(3432795737u, 4256529583u, 3151717298u, 4190687875u, 1563633254u, 158068428u, 685294219u, 733826550u, 2829744078u, 4225504275u, 2375584227u, 1429440840u, 2192098666u, 1015042413u, 840775854u, 41702830u),
    SC(3231767315u, 1865273494u, 1093659663u, 1873962287u, 1664376931u, 1435837948u, 31100007u, 316783664u, 996300708u, 334486049u, 1648124912u, 3615910102u, 2480590997u, 2253624363u, 548978494u, 3975730498u),
    SC(1923874249u, 3947343158u, 2264687656u, 1121555015u, 3593673308u, 289357572u, 3048054908u, 3707221766u, 2043411687u, 1708537123u, 3350208529u, 2939237811u, 2793137666u, 3370678100u, 1405378414u, 2235087472u),
    SC(139882711u, 1304366355u, 1276034712u, 2139658031u, 2197726287u, 3663457902u, 2357615523u, 1611719773u, 2323318078u, 260257531u, 2850134214u, 3099029628u, 553263652u, 173876122u, 2118167747u, 1771928540u),
    SC(566458485u, 3545725305u, 2257836680u, 2245189792u, 1605297549u, 245844769u, 2016071772u, 1896412522u, 821618527u, 1870442187u, 3958912319u, 4032980189u, 2069248247u, 4226059888u, 3345680132u, 1791157180u),
    SC(4148097755u, 2486537082u, 4003164230u, 2318687306u, 2491702264u, 229564758u, 4126839602u, 211561653u, 3452304873u, 2572510204u, 1630441069u, 3167885411u, 4175966562u, 1295680948u, 161732432u, 107333173u),
    SC(1923252062u, 311708286u, 1678166990u, 3717252154u, 3161198614u, 1069601573u, 4091259962u, 359278439u, 3768419820u, 2520693990u, 650972975u, 383288062u, 1217231824u, 2559091429u, 4278580592u, 2250271391u),
    SC(510621576u, 1629846927u, 3397488683u, 961386517u, 653633283u, 1754007094u, 2769834941u, 2247122605u, 2701964981u, 3912616774u, 3406969249u, 63999109u, 3141040146u, 2619453260u, 1468121925u, 4171492447u),
    SC(3961993547u, 1155134029u, 1496861029u, 1279080034u, 2846121209u, 3483514199u, 2468398271u, 505281559u, 3532558643u, 2311328115u, 2310583909u, 3085705085u, 2999958380u, 2683778623u, 32663880u, 1366954658u),
    SC(3799286526u, 1580228485u, 2766986278u, 586308614u, 2894037718u, 587959438u, 1301020570u, 2323176208u, 3827747523u, 2955860540u, 455053544u, 124753776u, 703403555u, 1658788582u, 3867772588u, 3276199889u)
},
{
    SC(2899222640u, 2858879423u, 4023946212u, 3203519621u, 2698675175u, 2895781552u, 3987224702u, 3120457323u, 2482773149u, 4275634169u, 1626305806u, 2497520450u, 1604357181u, 2396667630u, 133501825u, 425754851u),
    SC(373198437u, 4218322088u, 1482670194u, 928038760u, 4272261342u, 1584479871u, 2503531505u, 354736840u, 303523947u, 2146627908u, 2295709985u, 233918502u, 3061152653u, 3878359811u, 3090216214u, 1263334344u),
    SC(2076294749u, 898460940u, 2754527139u, 2099281956u, 3551675677u, 4195211229u, 3603181913u, 1984445192u, 1121699734u, 573102875u, 2187911072u, 656800898u, 1477748883u, 3685470532u, 3965328576u, 4253954499u),
    SC(1876288412u, 2267864341u, 434083874u, 1779401913u, 2781669786u, 3073195348u, 669142308u, 3636028767u, 127310509u, 372075961u, 2537369503u, 2705808591u, 971889633u, 2718294671u, 1415139024u, 276903675u),
    SC(3596445084u, 2918342013u, 1827011883u, 3900260359u, 1783558754u, 1921301616u, 3293933601u, 1111091218u, 3238604202u, 967515902u, 1208493040u, 1614341552u, 903992012u, 480937886u, 28823639u, 2379076161u),
    SC(1968094521u, 1600813704u, 2958098796u, 909224758u, 1752381729u, 3115930502u, 3643078327u, 2863416031u, 2510423171u, 2162796973u, 1796627662u, 3678673773u, 239312629u, 2457874359u, 3809753210u, 2494718541u),
    SC(1731463174u, 4265769542u, 194787641u, 1036371942u, 1745836602u, 660344840u, 1082796561u, 3963871960u, 4001246025u, 3118794916u, 3886266100u, 1928084049u, 3032262555u, 2306541818u, 3921311698u, 2426451176u),
    SC(4018285402u, 658949239u, 1329629679u, 2738829796u, 776877685u, 1774949833u, 2797031752u, 3236392582u, 2542061420u, 1832249084u, 183211998u, 1840198657u, 1314474881u, 3361925365u, 3440999944u, 974653576u),
    SC(1671164742u, 4271520021u, 1517391404u, 3289979834u, 1233503784u, 3050636514u, 3728319521u, 2919957525u, 3518724155u, 1272537958u, 3303667759u, 3864284110u, 234069183u, 1495943844u, 1989482539u, 3056780355u),
    SC(1575547612u, 2187321001u, 2701011625u, 2761636008u, 1864623673u, 3995428494u, 1950725639u, 3749309698u, 2711714857u, 3743669273u, 3222519898u, 621366782u, 2554696188u, 176315043u, 1467854493u, 1806812435u),
    SC(1182422499u, 3354985654u, 814715964u, 4226927046u, 3360200226u, 2503195953u, 1526762508u, 3747376732u, 1505823655u, 3718914053u, 2708056196u, 1868291203u, 1664951819u, 1982491563u, 751360443u, 1075645602u),
    SC(101076600u, 386741863u, 2955045918u, 1653351871u, 1070602553u, 321875967u, 3200546966u, 2632915072u, 225765461u, 1759013254u, 4169466720u, 3880757831u, 1769634729u, 2642211393u, 4245887731u, 3909815727u),
    SC(2379322656u, 1554830911u, 1971754317u, 1058862290u, 623917994u, 2775317172u, 3261049248u, 1667374591u, 3883068608u, 3752131736u, 2607464936u, 1251402973u, 4056909038u, 937468613u, 309280197u, 1804321090u),
    SC(395093976u, 2154850233u, 624748058u, 3473623511u, 530005996u, 1656467301u, 451942772u, 3238178099u, 691726480u, 2563588439u, 3675387583u, 3294893253u, 1205949092u, 3844564019u, 114533547u, 4193437592u),
    SC(1241354591u, 1121646490u, 1686974686u, 3373490541u, 1189649937u, 2948191343u, 2978671156u, 3827318062u, 3377194192u, 3805066092u, 3271994064u, 2484020181u, 549626522u, 1166583694u, 3299399570u, 764854172u),
    SC(2808929206u, 427994673u, 2338143204u, 3942895356u, 2304289727u, 1468778908u, 1350679341u, 3972686632u, 2399853022u, 2097821409u, 3799931826u, 2500883276u, 1352425312u, 3372587055u, 596007302u, 2017539287u)
},
{
    SC(172527491u, 737404283u, 1378219848u, 1967891125u, 3449182151u, 391223470u, 304889116u, 3996348146u, 1311927616u, 1686958697u, 766780722u, 1429807050u, 1546340567u, 1151984543u, 3172111324u, 2189332513u),
    SC(3269764283u, 1288133244u, 1314904801u, 996741356u, 1884733412u, 1544206289u, 558284137u, 1518251699u, 1924323147u, 1635892959u, 1275016917u, 3776324356u, 1705865502u, 202621081u, 499067715u, 3311904259u),
    SC(2660619816u, 3307703068u, 1451637465u, 3851776926u, 2364760323u, 1977782632u, 1515607226u, 1445106389u, 2327693248u, 2319920969u, 1115274896u, 1834441597u, 402374626u, 1205432354u, 1396686295u, 491780324u),
    SC(1996097434u, 731516361u, 974312078u, 3421366629u, 3812294134u, 3978884039u, 3352635742u, 1797690428u, 13489496u, 1642706934u, 3128398168u, 106641350u, 4016459895u, 2470770670u, 115922099u, 2925890710u),
    SC(2686884812u, 2748914055u, 1937433663u, 756783569u, 413219250u, 1566264233u, 3400883298u, 1726270584u, 1877719428u, 1988282262u, 4210071735u, 1623567192u, 186026227u, 1235988261u, 878101455u, 3591361377u),
    SC(4053231115u, 4124107153u, 3534184341u, 1110486344u, 81952807u, 4125498697u, 1693462482u, 2990125452u, 3439709895u, 1055710168u, 4246237022u, 1943085528u, 719511299u, 700284484u, 1082914808u, 1529874921u),
    SC(1481485493u, 1935423659u, 913226612u, 2395711383u, 1541429099u, 2771316424u, 3338417471u, 399999946u, 26796724u, 1562275554u, 2290450886u, 1574607684u, 2722372873u, 1229315759u, 1998792801u, 1299123352u),
    SC(3949810665u, 1328858449u, 2680298883u, 4060684833u, 1165923991u, 2656262528u, 835037267u, 1633040358u, 3109606689u, 3612027263u, 1850965274u, 2501035455u, 1956880692u, 2989837601u, 2991272131u, 514909703u),
    SC(3542886422u, 2995653583u, 3564619313u, 2091503271u, 1371789218u, 2765269616u, 3068810600u, 1666719265u, 2118314133u, 3335278251u, 3361418207u, 807286765u, 899334530u, 3994904643u, 2747385847u, 3528707340u),
    SC(3132681349u, 3533155425u, 2330764867u, 3555018576u, 1500828005u, 1243623897u, 1071818853u, 2130356426u, 4099162373u, 1333917673u, 445413180u, 915835391u, 3998951530u, 3932499234u, 2014496944u, 1476384528u),
    SC(2104877156u, 1430391164u, 3607724722u, 2456386351u, 3275987562u, 653382938u, 360082336u, 281545563u, 2556998173u, 802173963u, 1898654040u, 2873697709u, 3526274706u, 30023701u, 1532464389u, 335648001u),
    SC(1216717657u, 3420164715u, 1026103527u, 2814363815u, 3399248527u, 2265457834u, 4230549954u, 3191596424u, 2096767009u, 197782440u, 661821193u, 3129199915u, 3603027595u, 571989255u, 3350141303u, 902722054u),
    SC(86788496u, 2319129483u, 1051755765u, 871757145u, 3910221139u, 2373267495u, 991927221u, 3506242540u, 2918237538u, 555183593u, 3050652275u, 2550066259u, 1935622924u, 1141386013u, 1915989302u, 1193809339u),
    SC(2961067645u, 912271025u, 3829956364u, 976054309u, 2426360429u, 3756714048u, 860863671u, 2976390123u, 651422564u, 3348472580u, 4062622529u, 3566918328u, 1262646615u, 526922344u, 336090107u, 3690353753u),
    SC(1104160934u, 638409761u, 4090697585u, 3951520784u, 412890746u, 3037968225u, 623962484u, 1861465265u, 4172453316u, 2731726287u, 468253494u, 2636411583u, 2233875405u, 976659501u, 1885152597u, 441456529u),
    SC(228814647u, 3127034711u, 536841111u, 970423620u, 335496573u, 1496573821u, 3839638808u, 2076574157u, 3960354230u, 1830746438u, 2136594363u, 1397484405u, 335074021u, 421124372u, 4043995000u, 1296743377u)
},
{
    SC(2759056966u, 2773771898u, 915395955u, 378399267u, 1065424189u, 3786627878u, 2430240867u, 1910948145u, 1268823138u, 2460932406u, 2049702377u, 3729301642u, 2270156417u, 2935515669u, 1488232015u, 333167852u),
    SC(3963231590u, 2717344665u, 3330507643u, 2069094492u, 1576271806u, 844971343u, 3725773593u, 3293220801u, 1933125411u, 1106657228u, 3650404527u, 3511000962u, 3309805512u, 23235466u, 884265026u, 3867812812u),
    SC(2380535986u, 2007649740u, 291610222u, 4151143005u, 2330231880u, 3336494284u, 4079710776u, 3045731925u, 300175272u, 1753290057u, 2323446107u, 2448133203u, 1897525100u, 62520621u, 938748110u, 2483424933u),
    SC(3941565796u, 4020457560u, 536627435u, 849338423u, 1622694903u, 2253013822u, 1890968103u, 2458058141u, 2431563444u, 3273994144u, 2920282564u, 2871620844u, 315460419u, 2331615405u, 105614140u, 3825521500u),
    SC(1770365960u, 436268948u, 2889892729u, 3688514673u, 3952720709u, 1774783907u, 605504449u, 2947048934u, 38294098u, 846447109u, 2199988078u, 482652009u, 58745901u, 1043251865u, 1692020085u, 2977904741u),
    SC(3749156389u, 3930496686u, 342096417u, 2961755248u, 1791611872u, 2622150301u, 1430397623u, 2049694734u, 1457522946u, 1307567328u, 1594457791u, 2920040322u, 2838823131u, 3221083429u, 2327375059u, 307491364u),
    SC(439175999u, 704562179u, 1530705937u, 343762620u, 1895613568u, 82869187u, 23704978u, 3831637605u, 1611450850u, 923617677u, 3571146990u, 2520538539u, 2376639038u, 2377370369u, 3624250410u, 3615349574u),
    SC(764309941u, 395778606u, 890380761u, 1156064327u, 244397938u, 560614464u, 4033284221u, 1090955901u, 3643294611u, 2912576497u, 772374999u, 2861631454u, 564730390u, 3124994653u, 646536012u, 3616789797u),
    SC(3040822479u, 2767342245u, 2776280569u, 3485527708u, 3592541314u, 980436690u, 2153312390u, 215781809u, 2169043418u, 2501125521u, 3698439429u, 3999324854u, 2793459908u, 501030861u, 3583683133u, 3712651293u),
    SC(4078810936u, 708788696u, 3557269243u, 3488736225u, 3893932756u, 4164798985u, 1241795187u, 3595203666u, 2393791384u, 3416169943u, 714289829u, 1522223608u, 2613922570u, 3640037692u, 3871460094u, 693107847u),
    SC(2095442944u, 4280954881u, 166522183u, 982064125u, 4072843681u, 2413289870u, 966372633u, 3054322365u, 3306439070u, 657208192u, 175957468u, 411297739u, 771116169u, 1596617487u, 3454202820u, 2489020407u),
    SC(1474971529u, 4158663721u, 2047384831u, 2598838221u, 256974012u, 2456523417u, 631366020u, 3323296862u, 3331748634u, 1360209248u, 3346726166u, 365777010u, 1290850614u, 2085594058u, 2979720197u, 2832663037u),
    SC(1555709774u, 2326491405u, 2273744879u, 2585453209u, 2182701308u, 3405285511u, 2624534747u, 1273093088u, 862771016u, 2571185727u, 2627816705u, 753650915u, 1122934423u, 1670176575u, 3747348599u, 2369664950u),
    SC(90900628u, 2102730721u, 781890942u, 2802660398u, 1018645876u, 4115262915u, 4149550831u, 3399458752u, 3886843346u, 2763694604u, 1310436099u, 1905281291u, 3814148817u, 4190880658u, 4069475791u, 3679310561u),
    SC(2090876031u, 2877257381u, 2723690078u, 1430728835u, 1519931567u, 1820574481u, 3028789440u, 1269332520u, 487867652u, 423473929u, 386546855u, 57358783u, 1188070806u, 1428826466u, 1782333616u, 177182180u),
    SC(1560550296u, 3093603077u, 293048812u, 568213435u, 3420818052u, 2217333393u, 3134601365u, 71485947u, 1184987600u, 3737951852u, 162939585u, 1604396734u, 102336303u, 398862141u, 820178097u, 490472018u)
},
{
    SC(1198357412u, 890731121u, 697460724u, 351217501u, 1219769569u, 940317437u, 2678867462u, 4175440864u, 2131908090u, 1470497863u, 3243074932u, 494367929u, 1767796005u, 457609517u, 3543955443u, 4149669314u),
    SC(3330984275u, 2556191310u, 3686726368u, 344917147u, 3386773283u, 2065247867u, 3908122913u, 3695674005u, 2012204991u, 2693522884u, 103992040u, 209624682u, 1376640025u, 3686868767u, 2902487256u, 913177313u),
    SC(51667624u, 2920015049u, 3017253519u, 1071812123u, 2571723173u, 2160964558u, 1290623835u, 537361271u, 825729747u, 1392761590u, 1142623949u, 609149740u, 478665972u, 658807909u, 3553467330u, 1636424506u),
    SC(3616504574u, 1808500084u, 668829693u, 946464586u, 1979729368u, 406956181u, 4175922839u, 412791377u, 2386664246u, 1192624407u, 2943858119u, 2548487829u, 1705793661u, 3457595727u, 202485393u, 1924721832u),
    SC(2189382710u, 4186169698u, 1109472631u, 1983920883u, 3607145598u, 92147950u, 1402492489u, 429006982u, 2674194346u, 4283195956u, 1593180543u, 3760708566u, 643378372u, 4031840072u, 3394015175u, 1558737750u),
    SC(1805700700u, 1754525187u, 1654624487u, 2216136944u, 68436239u, 2233918826u, 2968997668u, 4123197178u, 634669625u, 2517670383u, 3007433093u, 3522650191u, 696793327u, 1110232330u, 152147442u, 726198231u),
    SC(742639492u, 3149716575u, 880320409u, 4630949u, 1505653181u, 1071542118u, 3069898832u, 2578767084u, 1314905164u, 2213468220u, 3680194608u, 2445142726u, 2802637025u, 3977804516u, 1184600151u, 419058566u),
    SC(1336605659u, 403108152u, 2724587657u, 3679190711u, 2874389193u, 1647236788u, 3333657299u, 528273159u, 3515102004u, 947876802u, 3658623910u, 174276546u, 653934448u, 3828171172u, 1444811038u, 2933240663u),
    SC(339431464u, 3233735983u, 2646677300u, 43177515u, 392637796u, 1436471495u, 1239428896u, 2348305406u, 2289915967u, 3084305790u, 3250948245u, 178888356u, 2146779246u, 4234024427u, 1032696742u, 3905672369u),
    SC(961540617u, 2841143833u, 962675692u, 4171962245u, 2791421965u, 2368576296u, 3328980779u, 2916707843u, 1558316022u, 134331787u, 2460382133u, 1215270659u, 146717643u, 3198704598u, 2091590890u, 2460305557u),
    SC(1042706599u, 2034894580u, 690504458u, 2345543782u, 4005260856u, 2432547988u, 112379796u, 3543073874u, 835904670u, 2590827554u, 918469413u, 3408148837u, 1789043194u, 1729294718u, 1834822488u, 2928788408u),
    SC(3301658713u, 837504950u, 1727706187u, 1845900341u, 896114239u, 2352826711u, 3111232113u, 2017659422u, 2679415011u, 2370224692u, 3953323203u, 2250773775u, 1103871456u, 1933857783u, 3328123972u, 3307902309u),
    SC(1767706194u, 3006067357u, 35851140u, 3240494485u, 2221989856u, 1899667734u, 6385932u, 2363969169u, 4105037265u, 1831329288u, 2027489194u, 884350865u, 1094001278u, 159320441u, 4110377537u, 68569781u),
    SC(1525490260u, 665735034u, 2452169880u, 171203360u, 1236274187u, 676156893u, 1374080130u, 357190845u, 1839504596u, 1514713169u, 4060710869u, 1096636593u, 2588809028u, 3627704311u, 1809407212u, 476953361u),
    SC(957000182u, 26105440u, 3440739633u, 2098069989u, 1584380370u, 2860012851u, 1732766592u, 212521659u, 3179187407u, 887560394u, 2490695882u, 2732057577u, 1018218231u, 3635922188u, 2062474881u, 2513446682u),
    SC(1107263183u, 578424674u, 37103195u, 466969755u, 2523291988u, 291121216u, 3279675483u, 2003600853u, 4199013737u, 2715326244u, 4169142308u, 3686083459u, 3512922856u, 3093381668u, 1195683747u, 1393205701u)
},
{
    SC(1331866444u, 3086683411u, 308412705u, 2554456370u, 2967351597u, 1733087234u, 827692265u, 2178921377u, 289799640u, 3318834771u, 2836568844u, 972864473u, 1500041772u, 4280362943u, 2447939655u, 904037199u),
    SC(2575383612u, 3753748540u, 2811819999u, 1587868018u, 1038431720u, 790984055u, 3731301644u, 1846621966u, 951964491u, 415041564u, 2200992348u, 4272384400u, 296027191u, 4287888493u, 2854418940u, 3573682726u),
    SC(1970740379u, 2607713160u, 3470587124u, 930264002u, 1173824281u, 122965335u, 3335069900u, 326806848u, 3632692886u, 129472919u, 3226625539u, 2728837633u, 416887061u, 1130551300u, 356705234u, 1369994655u),
    SC(4223755401u, 2079062379u, 3389104769u, 4073338565u, 3689225172u, 440818499u, 856809827u, 381405275u, 127244068u, 376610605u, 2598268701u, 2534766433u, 2820385475u, 4294123141u, 330930335u, 318185845u),
    SC(761419527u, 3536226585u, 2328998689u, 3591334816u, 1578134205u, 1103093801u, 3418753973u, 3588283844u, 1530820786u, 2684864777u, 924992522u, 3557568163u, 1869705595u, 3313643247u, 841618349u, 1632346896u),
    SC(3475240082u, 1688964704u, 2950217939u, 2829510968u, 4218043142u, 1723444205u, 599182149u, 3585292920u, 1201476124u, 1461631424u, 3796636907u, 3015591958u, 325310290u, 4221903599u, 2685464188u, 843835594u),
    SC(3270571096u, 3849271420u, 2838244847u, 4029431364u, 3703574760u, 3266810236u, 1964057057u, 1045028730u, 3535646880u, 4117469088u, 268273252u, 28527135u, 616206627u, 3498685014u, 1783632491u, 2430589238u),
    SC(1270864764u, 2335784868u, 3187652054u, 3487500065u, 3514696661u, 4279511860u, 2691960889u, 1283768022u, 3239440117u, 3088430000u, 3270700109u, 2562105500u, 920167200u, 797042551u, 4008345612u, 1713652205u),
    SC(1233553764u, 2449552413u, 3139739949u, 2886523083u, 3648218127u, 435238208u, 231513377u, 3598351734u, 1003225207u, 1550611030u, 4262337852u, 2819804714u, 3244463273u, 2073740987u, 855086785u, 975917304u),
    SC(2715954175u, 3495328708u, 4029028922u, 3684471179u, 2815956881u, 3599669751u, 4163140273u, 33191313u, 2635890672u, 3683103094u, 1579697202u, 287936530u, 2496546027u, 832886459u, 1241267398u, 3564329642u),
    SC(718666875u, 1628061148u, 3834972005u, 11037458u, 3790987439u, 2312775807u, 3375415349u, 3089087440u, 2679862136u, 918687461u, 3176925215u, 1435039099u, 1342114588u, 1906963252u, 3488735014u, 1611160706u),
    SC(4216184459u, 1084561028u, 249927207u, 3584932419u, 1355984265u, 990857900u, 1870305536u, 582023708u, 1966962179u, 1733088207u, 1190083164u, 3785297292u, 1004947745u, 1784159416u, 1841702516u, 180335137u),
    SC(4084089742u, 2441136551u, 426220168u, 1375299216u, 1841338030u, 1250354698u, 2728864721u, 2959990011u, 1071025467u, 1691914484u, 2858760972u, 1516700275u, 2771651049u, 607063247u, 4219381388u, 3373946171u),
    SC(2146554811u, 2380633398u, 431356428u, 2501496525u, 4195490782u, 4281443977u, 1707183170u, 3515016439u, 43334925u, 2064458077u, 4149827026u, 2544422546u, 1259302114u, 1919625668u, 729425798u, 2757346641u),
    SC(2475010648u, 501654469u, 1262984133u, 2284058265u, 3864896735u, 3216144340u, 3043718887u, 3290359029u, 2513504704u, 1583873907u, 787550022u, 889877880u, 4155285556u, 2519357244u, 1887123831u, 2544852082u),
    SC(1329107374u, 3899397847u, 1931705980u, 3537599611u, 2074239136u, 1267070685u, 2447524924u, 3173107761u, 2842541385u, 924561908u, 2664553616u, 395476463u, 813764142u, 3107511895u, 179660379u, 2380654703u)
},
{
    SC(286197159u, 1217476806u, 1373931377u, 3573925838u, 1757245025u, 108852419u, 959661087u, 2721509987u, 123823405u, 395119964u, 4128806145u, 3492638840u, 789641269u, 663309689u, 1335091190u, 3909761814u),
    SC(2458775681u, 3448095605u, 3846079069u, 1243939168u, 2712179703u, 2514528696u, 1400411181u, 3792085496u, 528921884u, 1230512228u, 4062090867u, 931590129u, 3669288723u, 1764179131u, 2650488188u, 764612514u),
    SC(3981461254u, 1881876860u, 3861653384u, 1419940889u, 3890280301u, 225359362u, 3772709602u, 2406778923u, 1744011295u, 836946168u, 1547583643u, 2969842237u, 3997288340u, 2150480638u, 3129156617u, 1325216902u),
    SC(3592470591u, 3671101194u, 2792523734u, 2070472959u, 1473838345u, 785123121u, 2721504084u, 2212009910u, 4070989896u, 1696639999u, 2859248441u, 3104578877u, 2309769016u, 4267049236u, 2484173427u, 1626540609u),
    SC(4267160019u, 2981312649u, 344263087u, 698599319u, 1002907346u, 93565259u, 286808078u, 1804582990u, 3599771325u, 2181306538u, 1961279765u, 187428107u, 223299791u, 4043449191u, 587626985u, 2106033479u),
    SC(501761768u, 2386293097u, 1180388710u, 1812775472u, 918601490u, 3009070794u, 1574279477u, 1505824867u, 3643095372u, 3370828988u, 832869144u, 404837899u, 3152252263u, 3925885097u, 69867335u, 3741018586u),
    SC(2051920526u, 1020215512u, 2058830843u, 1611771091u, 2552120098u, 75944844u, 1802229404u, 915313553u, 2313215016u, 1745739579u, 443475191u, 2998247588u, 3289885130u, 1289464560u, 2961919458u, 3798282256u),
    SC(1496487624u, 2215532014u, 4148657376u, 3923080315u, 216179279u, 3856996518u, 2014567019u, 880786726u, 2125033974u, 58008256u, 4039109547u, 402585883u, 2182540617u, 437175766u, 1441865826u, 1665450276u),
    SC(3078919323u, 1109978808u, 3102316446u, 4252174800u, 1046362670u, 3864571927u, 2260100326u, 3682270765u, 2139319322u, 1066628173u, 240059747u, 1164853046u, 1454716611u, 512654137u, 1544275853u, 2556727566u),
    SC(580428655u, 115762757u, 1593355348u, 2740341778u, 1504897999u, 975028678u, 2401832824u, 4197869940u, 3667767462u, 644880229u, 691878327u, 369150353u, 4026243769u, 737605979u, 2791271214u, 2620684209u),
    SC(624678531u, 4114750403u, 1274989179u, 1531504358u, 3520816024u, 2554021149u, 1865577096u, 1362433716u, 1638936249u, 3016959317u, 2526207810u, 3033412199u, 695904139u, 2060012285u, 3230414132u, 860289224u),
    SC(3442642063u, 1520946900u, 218826564u, 968761561u, 4098434233u, 3360677602u, 2204368028u, 486310067u, 2601372374u, 1399175099u, 2183933043u, 806379489u, 2424203087u, 2668736829u, 1664637882u, 3005713727u),
    SC(700899790u, 1066183324u, 3546718434u, 998702102u, 2557230354u, 2084117292u, 2934243163u, 1545771642u, 3688392810u, 3908656537u, 3447657276u, 840000010u, 2955752477u, 44371204u, 3799655472u, 3734995825u),
    SC(3265506533u, 942399325u, 173917125u, 161041810u, 2297418901u, 849604788u, 2703870825u, 2810175425u, 3617296913u, 1432689375u, 3133875354u, 1118654553u, 2616257301u, 495686053u, 4127407123u, 1943733376u),
    SC(2005668850u, 485568946u, 2260461782u, 2622034876u, 2693998905u, 2811925574u, 2831747304u, 3217266392u, 2520502878u, 1176196783u, 2567958416u, 1525744035u, 2841811417u, 1157609637u, 3871707993u, 2765099676u),
    SC(207989197u, 368293876u, 3237374184u, 1394768686u, 1254103141u, 935691540u, 375090092u, 2481205522u, 2920254212u, 492683984u, 2055637221u, 4291235240u, 3889542314u, 2465899605u, 1694380507u, 757371549u)
},
{
    SC(136266275u, 1782161742u, 3530966629u, 586004249u, 4076565170u, 3312577895u, 876489815u, 1337331291u, 888213221u, 1813863938u, 1374206604u, 2668794769u, 1377764865u, 784024905u, 1937217146u, 3627318859u),
    SC(3161427495u, 2344678392u, 1808682441u, 2396619894u, 3034006140u, 1044331129u, 4102609084u, 1058091322u, 1515502621u, 1258860285u, 1406233340u, 127619173u, 3057107171u, 225762630u, 1651671815u, 4285298193u),
    SC(630785468u, 1344100570u, 1929331818u, 828088181u, 2313124884u, 1302120759u, 3180735860u, 313275450u, 1008942268u, 2707820177u, 4248947940u, 1732478629u, 3645496831u, 611830707u, 1937638387u, 61731419u),
    SC(1347537282u, 2857000226u, 227299159u, 1108544547u, 1181072563u, 1291715943u, 3752803919u, 2688390945u, 2484326219u, 1350060758u, 452823659u, 2363636452u, 2152205190u, 1812507720u, 607624535u, 2319475408u),
    SC(3222638329u, 3875752446u, 758301165u, 51152840u, 2430504171u, 1189996379u, 44948392u, 232960619u, 3026371583u, 2974537914u, 3244781723u, 3702394182u, 2835938901u, 663347918u, 3320069474u, 3071978352u),
    SC(1947047272u, 3022037725u, 949698504u, 1728470528u, 283847009u, 1458268020u, 360012619u, 1579646653u, 4005878207u, 1765381301u, 20903539u, 2558445559u, 757888638u, 2604781527u, 2240457927u, 3990518442u),
    SC(4281545336u, 1208697934u, 2578865021u, 2456188396u, 1796646478u, 3757714293u, 2622755030u, 1606025966u, 30472258u, 3850691354u, 1208779266u, 405050222u, 3807844323u, 3748806955u, 358470323u, 4212845387u),
    SC(2041619043u, 3711576883u, 835794591u, 2392116351u, 2862318436u, 689502669u, 2866163103u, 2052898811u, 576580608u, 1144506306u, 542475550u, 474572979u, 4137279429u, 2221684538u, 331268239u, 1556318477u),
    SC(705880713u, 2092991958u, 815360595u, 3449491044u, 1305192012u, 2057063005u, 3299868133u, 1114733861u, 730760330u, 1129737257u, 4233249504u, 1217580888u, 452658791u, 2612091783u, 1764043106u, 1669202162u),
    SC(3689992902u, 700129090u, 282055655u, 756126609u, 382876308u, 4262209576u, 2436932760u, 484247369u, 1415138625u, 2340918814u, 3058199817u, 4145497883u, 334812059u, 461523021u, 2221122791u, 2995497332u),
    SC(706669295u, 3007808000u, 3728730665u, 3241577762u, 3126001367u, 292940936u, 1126531898u, 3913205978u, 304146054u, 2548053118u, 3490807704u, 3465095661u, 3938930443u, 804039554u, 297557674u, 1669808877u),
    SC(2395818908u, 3199065200u, 4060875213u, 1731284266u, 1022607637u, 1154299144u, 3879751917u, 384430926u, 86892497u, 2036004815u, 2668116514u, 901861508u, 2277490553u, 1312485879u, 562264334u, 170374972u),
    SC(2192479620u, 3046309306u, 143307916u, 3468295982u, 3110013374u, 699221760u, 273412494u, 3153322038u, 2886126025u, 1296005576u, 2326933823u, 3713038344u, 919578907u, 258326637u, 1991591857u, 604405680u),
    SC(3283196708u, 902217854u, 1295144146u, 503984315u, 566424671u, 1755595238u, 2455519229u, 120267530u, 1004363245u, 1611271287u, 1013059281u, 3646183010u, 183890924u, 188417891u, 1612883046u, 2255154239u),
    SC(1231171449u, 2524105034u, 653815517u, 585754026u, 3098352226u, 866901449u, 4223318963u, 1071806142u, 3239364285u, 4077877700u, 423690458u, 2222266564u, 4117269051u, 1893556406u, 3304547745u, 215164118u),
    SC(3229321461u, 3443938850u, 803179772u, 3340311630u, 2749197592u, 565049216u, 1674980657u, 45735981u, 3858875409u, 2208179057u, 2167864606u, 3853383863u, 3320158569u, 901453102u, 2505912317u, 1486241881u)
},
{
    SC(768143995u, 3015559849u, 803917440u, 4076216623u, 2181646206u, 1394504907u, 4103550766u, 2586780259u, 2146132903u, 2528467950u, 4288774330u, 4277434230u, 4233079764u, 751685015u, 1689565875u, 271910800u),
    SC(2894970956u, 471567486u, 2880252031u, 2717262342u, 4077383193u, 1268797362u, 4257261832u, 2560701319u, 2691453933u, 1607372210u, 2771176414u, 58794458u, 4272438220u, 2521311077u, 642919262u, 3613569198u),
    SC(549667688u, 1635817891u, 3597742712u, 2133548191u, 983618585u, 1077056145u, 1016537981u, 3024916594u, 3788763915u, 2354027825u, 234019788u, 1129974745u, 3836449602u, 132091652u, 2429034711u, 3714188356u),
    SC(3752023309u, 1237246457u, 810507218u, 1575719630u, 2984629402u, 1312110059u, 1532351529u, 3778270553u, 500991970u, 3016414634u, 2451804626u, 3116044735u, 2749076428u, 609078974u, 343845623u, 1628221103u),
    SC(1079050562u, 537097107u, 2113045556u, 1216978919u, 795109794u, 494396817u, 3615304214u, 3016596136u, 1485503229u, 2246940765u, 2872639209u, 812577075u, 3970992077u, 816616346u, 4279493103u, 2696304890u),
    SC(302016674u, 1709668681u, 88411267u, 3337357281u, 3061995584u, 3396993199u, 1858891069u, 2509301562u, 3807375387u, 3567949934u, 3737724046u, 4137514111u, 1709156749u, 1400722499u, 3253197246u, 830289695u),
    SC(86642997u, 2517748533u, 1802616926u, 3224858276u, 667521935u, 294768443u, 3699185630u, 2619978653u, 1654256627u, 789295435u, 4056501046u, 2298266369u, 3425028365u, 3740463800u, 2064449616u, 423401599u),
    SC(587205175u, 208206623u, 1253389730u, 3674422134u, 284316357u, 2112208954u, 1196434050u, 302049830u, 985808817u, 4037289748u, 2191325460u, 4289570719u, 592322138u, 3671063901u, 886295122u, 2540475213u),
    SC(2164961127u, 4048157441u, 2790139366u, 1435011700u, 4142835891u, 3320410016u, 2681849481u, 1047872443u, 2885564134u, 874029678u, 2048520878u, 2934385850u, 1097367713u, 1997417466u, 2045706034u, 898129538u),
    SC(3451958921u, 95403444u, 4056502814u, 671939501u, 2069116441u, 3101129770u, 553516228u, 1712496197u, 2639919391u, 3157824758u, 2182076931u, 2920510603u, 91421090u, 3496854290u, 1333938225u, 2005754623u),
    SC(469295760u, 426796598u, 3855795018u, 970866434u, 856973549u, 2439780350u, 2385957015u, 2589908140u, 3781058972u, 4109407963u, 32316753u, 3931244779u, 68560366u, 1699148814u, 843806029u, 3772908229u),
    SC(3846833357u, 4119412096u, 438094070u, 2645426661u, 884548695u, 2876447138u, 80918210u, 2029354870u, 135282137u, 3030947473u, 2960763605u, 1898348122u, 4127316996u, 2240743006u, 2934791826u, 887094286u),
    SC(1897883656u, 1406242187u, 2434671426u, 2734794757u, 2714201131u, 3046668149u, 257451999u, 3794951424u, 152449195u, 3454838096u, 2737741298u, 821046884u, 2554260361u, 962889686u, 1262263641u, 2203109889u),
    SC(1985684731u, 222483668u, 2849949193u, 1221492625u, 2084499056u, 1235444595u, 2655267198u, 1020186662u, 1447071023u, 3629752849u, 651251319u, 2167418603u, 2268535831u, 2985934672u, 2652239173u, 3259021212u),
    SC(3062826974u, 1796450254u, 1939504794u, 476729966u, 3521076442u, 3086668105u, 234121934u, 986487065u, 1570879569u, 2820662853u, 1206879400u, 4271520206u, 4242315964u, 2749978648u, 3007865079u, 4114755771u),
    SC(3649818358u, 3409857055u, 1537210569u, 2398557069u, 3130583052u, 536941530u, 3880813719u, 1419070102u, 1164730147u, 2533104753u, 2046210979u, 2821557175u, 2327264610u, 1639358616u, 2001893732u, 1524105344u)
},
{
    SC(294473811u, 4198428764u, 2165111046u, 977342291u, 950658751u, 1362860671u, 1381568815u, 4165654500u, 2742156443u, 3373802792u, 668387394u, 853861450u, 2637359866u, 2230427693u, 2824878545u, 103849618u),
    SC(3462974251u, 3960356708u, 3970663027u, 1911703734u, 2602955995u, 2496279357u, 210580885u, 3874806640u, 2822070051u, 4063068709u, 2061277285u, 1429537360u, 2349584518u, 2910686068u, 3963567776u, 3972103816u),
    SC(2016723458u, 2541590237u, 3532225472u, 3001659539u, 112442257u, 922189826u, 2246032020u, 3487464820u, 1658786807u, 2276379919u, 1596562072u, 457926499u, 2193005220u, 2575074329u, 529788645u, 1519231207u),
    SC(1572936313u, 886315817u, 1530415140u, 2311860166u, 3941188424u, 45807153u, 2483174955u, 1469805839u, 3162970586u, 2454510043u, 2417743140u, 2783896043u, 4229304966u, 1351489836u, 284407686u, 4050060666u),
    SC(1089549454u, 2684562245u, 1059803961u, 224950790u, 58262787u, 3033299806u, 927475933u, 1400133226u, 3082832878u, 1490904482u, 3040968407u, 593844137u, 1569781919u, 798746464u, 1083127814u, 1590280691u),
    SC(1538536818u, 1828650047u, 3754703497u, 985555578u, 1002045074u, 767791702u, 915104522u, 465342914u, 1114045622u, 3426575950u, 1922317875u, 1070157234u, 3077282627u, 509171365u, 1607316331u, 668038565u),
    SC(3323765415u, 1224391265u, 2469548057u, 3722781348u, 3031269370u, 4289586349u, 2226931390u, 957179955u, 2298143215u, 388542993u, 1780793152u, 2112973240u, 1502081645u, 1973971844u, 934878133u, 1618693887u),
    SC(3954817210u, 3380652139u, 2572526672u, 1228436929u, 465848053u, 3939966705u, 2398020514u, 2900599831u, 2007674400u, 2727714272u, 2337519533u, 1681172994u, 4089802218u, 142069883u, 4261364192u, 2856729470u),
    SC(4248537414u, 694781904u, 571619480u, 3221145068u, 2970038253u, 3370542615u, 2832314379u, 1807587465u, 1411648700u, 1964173012u, 121911610u, 1134463822u, 2574507072u, 885427058u, 3741638072u, 3097389771u),
    SC(2158675312u, 116080836u, 3333803512u, 3797833536u, 984464391u, 4149942538u, 1145746749u, 1195624987u, 426540232u, 1021913877u, 3121679962u, 3390873776u, 3273678689u, 3851165262u, 4274383191u, 1915176720u),
    SC(1158541955u, 1843489443u, 998849897u, 969171492u, 1791167915u, 2484857096u, 1119081920u, 1901041264u, 2534183757u, 1529097558u, 2956376281u, 1260291681u, 1159207651u, 3441978306u, 2518693280u, 4253362775u),
    SC(1690661001u, 2213259738u, 3615956917u, 105152953u, 308358176u, 1328282355u, 1666389191u, 1019854259u, 2059193948u, 4244545599u, 1952864052u, 329670934u, 3592985517u, 571024701u, 1172799188u, 3135874872u),
    SC(1184018396u, 889004172u, 1920099477u, 1964506637u, 189152569u, 1805931691u, 3250067608u, 3446883320u, 1471577127u, 2315956523u, 1588897116u, 2470229082u, 3602241877u, 554726955u, 1644067322u, 87402371u),
    SC(1360270758u, 326216664u, 3362619326u, 1255989535u, 4140691901u, 856602972u, 2084629207u, 3858539838u, 78510889u, 2277092409u, 3136284616u, 1772786459u, 3229606238u, 94732571u, 2598206327u, 492226777u),
    SC(1257123658u, 2873597433u, 3001150814u, 421725801u, 236310867u, 582305583u, 3367057659u, 2102668336u, 153914902u, 4226436363u, 290094468u, 690656835u, 1748591179u, 3668885459u, 165028339u, 2139821087u),
    SC(2349582063u, 631395785u, 941018791u, 1503410647u, 181331585u, 2473834542u, 2528647747u, 3710284323u, 2364124560u, 3901998444u, 3224972026u, 605068436u, 546878913u, 356944705u, 3829683853u, 160452346u)
},
{
    SC(1451965994u, 766802222u, 1324674662u, 350355960u, 2823290314u, 951779387u, 2914020724u, 508533147u, 1932833685u, 1640746212u, 1238908653u, 542788672u, 3642566481u, 2475403216u, 1859773861u, 3791645308u),
    SC(216282074u, 1906267522u, 1852437064u, 1010678235u, 3729121535u, 4197231849u, 4150055440u, 1128246703u, 3264673345u, 1375783733u, 3415088931u, 34309836u, 2603881793u, 3106237815u, 2950890176u, 505684202u),
    SC(3927516830u, 2488673756u, 327917152u, 614182630u, 2355346359u, 730432873u, 88446505u, 4240960753u, 4121410433u, 1398090547u, 2262743232u, 651724036u, 4138228417u, 3106475766u, 4179362424u, 750466827u),
    SC(434692713u, 3111300976u, 3323560909u, 3413395188u, 601658363u, 2967722170u, 1070605430u, 74966422u, 813799229u, 4061279746u, 1996953298u, 1765274397u, 4035137864u, 2359104373u, 3535793255u, 618634298u),
    SC(1231617791u, 3545122377u, 2628213180u, 2391855988u, 3734909337u, 2705206020u, 681643510u, 368801430u, 691450613u, 2224147576u, 951972679u, 2767063862u, 3676868191u, 158497152u, 2165075628u, 2832330233u),
    SC(3529008459u, 1174295398u, 55914117u, 2816083797u, 205887723u, 1756010196u, 1648915894u, 1477354329u, 86311333u, 3889682737u, 1098085375u, 3464880379u, 1139759451u, 542536350u, 186494667u, 2442759451u),
    SC(3094023174u, 1995851063u, 4191388160u, 1722723757u, 1329293492u, 727282912u, 2669776257u, 2772951118u, 1386276034u, 3089621174u, 2303649396u, 2292749559u, 1467806712u, 266878652u, 2651863592u, 1006978704u),
    SC(2450691869u, 3012269556u, 3887712993u, 4048656504u, 2160727935u, 1940770088u, 174916584u, 3472792113u, 2648524840u, 990354037u, 1957678544u, 3888925732u, 1168435347u, 3720532709u, 3528212798u, 2624020545u),
    SC(69863181u, 2459013627u, 4217968964u, 2735851825u, 1081344097u, 737361378u, 2157825722u, 2900791120u, 1412000158u, 1206005337u, 3067055303u, 230632577u, 601427243u, 2760861753u, 3679310020u, 2091861010u),
    SC(2304197829u, 1531316041u, 2716383108u, 434697890u, 508817514u, 2929310544u, 3751532879u, 3785491984u, 2716598214u, 3666495867u, 3150261948u, 1306653078u, 2283636929u, 2492138954u, 1527136744u, 3312103429u),
    SC(3387483809u, 1095455990u, 3248396980u, 3181117152u, 2258888938u, 2053848664u, 2160875912u, 553275695u, 1752757914u, 1504034431u, 1046528434u, 1855690339u, 2425857774u, 2142030048u, 237252438u, 3919745098u),
    SC(3690358562u, 221287988u, 2268047572u, 3655202989u, 756646724u, 68846869u, 1965143185u, 513684595u, 404949341u, 3706987369u, 15990563u, 3409604325u, 658214808u, 2112012281u, 1742449680u, 1802932879u),
    SC(2972942716u, 4184192946u, 4124576773u, 3089123761u, 1179063207u, 2093485395u, 512951348u, 59239037u, 3674464770u, 787225894u, 1288484371u, 1987692265u, 3767465580u, 4044585132u, 2916653148u, 2297816723u),
    SC(3784876742u, 1057734114u, 4078669159u, 2003536621u, 3146165592u, 3800656487u, 297129408u, 4248472894u, 3906942491u, 4017607636u, 1285879766u, 3310681130u, 2653159866u, 2524355569u, 84128323u, 2374174391u),
    SC(1598027967u, 344901367u, 413901309u, 2414916476u, 417612014u, 1371467558u, 1499802638u, 967537237u, 1571117481u, 1088564682u, 3141693657u, 833402800u, 723113978u, 882224086u, 3586817872u, 3592950853u),
    SC(513582137u, 3376206006u, 3649593908u, 274710963u, 395026609u, 3340190413u, 1543782101u, 90195397u, 4157807658u, 412153222u, 558068169u, 2001737608u, 3474337160u, 1679447360u, 12885220u, 843004632u)
},
{
    SC(2083716311u, 321936583u, 1157386229u, 758210093u, 3570268096u, 833886820u, 3681471481u, 4249803963u, 2130717687u, 3101800692u, 172642091u, 421697598u, 4220526099u, 1506535732u, 2318522651u, 2076732404u),
    SC(3635330426u, 3675180635u, 4282523718u, 1750526474u, 1682343466u, 1292539119u, 2893227939u, 2897346987u, 1855384826u, 3916002889u, 4211021149u, 3439442996u, 241993264u, 1634586947u, 29890244u, 2635163863u),
    SC(2111268073u, 1081371355u, 3873218083u, 4044562588u, 2141674529u, 2107952064u, 3689043955u, 3423481956u, 2548188353u, 2697516682u, 4235866514u, 2985306600u, 3687062917u, 2383095614u, 206503719u, 2548448480u),
    SC(961167287u, 839569057u, 3482959339u, 4268254472u, 364097642u, 1343091094u, 3226753483u, 2159507482u, 3968394805u, 2518014496u, 3451298154u, 38127252u, 267735247u, 3484363065u, 957363479u, 1698662790u),
    SC(2744437828u, 3863759709u, 3010153901u, 3500431594u, 2624982656u, 875272695u, 1378345519u, 1791692262u, 3726226549u, 2682325366u, 3925052276u, 389591343u, 3869112658u, 650251545u, 6263093u, 860194434u),
    SC(309822299u, 841707800u, 2661553828u, 3383039256u, 238699224u, 1100968507u, 3534897900u, 4177846894u, 3463859410u, 1435499569u, 2006933774u, 3007046995u, 2819231184u, 288756524u, 1854189890u, 3858081977u),
    SC(2088052675u, 3396090720u, 416722812u, 2597822221u, 1176386826u, 3290882216u, 1002529034u, 2156491632u, 4202546863u, 1988253003u, 164033721u, 941800849u, 1186836065u, 2298291750u, 1863561032u, 1437279190u),
    SC(2858016010u, 775169843u, 2706497878u, 2821546952u, 2660836656u, 2077717717u, 3498848893u, 658545289u, 4048269927u, 418273988u, 1144587321u, 3094511386u, 4122354470u, 4225741678u, 603926280u, 979427875u),
    SC(1933550557u, 635706492u, 1314164193u, 391588743u, 834468642u, 1475393570u, 467867971u, 1271027212u, 2540684860u, 3801872764u, 1235100171u, 2159823063u, 532708943u, 665828867u, 4215955726u, 3885758496u),
    SC(3602864699u, 4002116109u, 644187852u, 1895585048u, 2776091504u, 72205071u, 554242761u, 4049640413u, 3149249833u, 688714164u, 687706448u, 3680924185u, 2274039047u, 303853541u, 2977107717u, 1196398757u),
    SC(3014099531u, 1302405838u, 17960870u, 4110705157u, 3801652109u, 2085339416u, 223612049u, 2870889264u, 3353629397u, 3527061798u, 674241336u, 3525864585u, 2278818471u, 2069831593u, 2885891701u, 1329881521u),
    SC(943450806u, 3704544104u, 3603194299u, 3757910007u, 502151885u, 765197432u, 4190577627u, 771063523u, 2436865367u, 678307964u, 1498061278u, 4120830837u, 3369466394u, 3399332765u, 1670894068u, 2891073104u),
    SC(501595739u, 1876059299u, 4182005344u, 160804770u, 962098784u, 2636270989u, 1828906496u, 1316975808u, 4088133273u, 2943366134u, 216957582u, 1003216568u, 4242258589u, 3505873185u, 2810125978u, 3429220861u),
    SC(2021386647u, 4046435053u, 1951135097u, 3941871277u, 2261999657u, 3808836272u, 2028063026u, 3659044589u, 3595750274u, 34514326u, 1889867282u, 1898224864u, 1659225476u, 3153868894u, 1647148554u, 1185039302u),
    SC(4119269244u, 1304843028u, 2354051818u, 2031439365u, 533555049u, 1418960734u, 214120313u, 4187370667u, 4256529561u, 2635160409u, 1836564249u, 3828261559u, 3235640513u, 181194540u, 4018312346u, 680914749u),
    SC(1914329770u, 3317667974u, 1413160514u, 2952053282u, 3332782151u, 3751637695u, 2146129829u, 167804454u, 2499496888u, 4213150810u, 223599992u, 2197202825u, 2869811316u, 2635473358u, 952082661u, 1532017334u)
},
{
    SC(701959589u, 2450082966u, 3801334037u, 1119476651u, 3004037339u, 2895659371u, 1706080091u, 3016377454u, 2829429308u, 3274085782u, 3716849048u, 2275653490u, 4020356712u, 1066046591u, 4286629474u, 835127193u),
    SC(897324213u, 739161909u, 1962309113u, 3449528554u, 2634765108u, 226285020u, 2832650161u, 324642926u, 2242711487u, 162722959u, 2264531309u, 2307017293u, 4006636248u, 1035416591u, 2557266093u, 3957962218u),
    SC(1912896448u, 699621778u, 2975109255u, 1580597872u, 2818493758u, 515803157u, 1642586345u, 785148275u, 2098287545u, 1424779842u, 1039209855u, 4238164284u, 4173562747u, 3569896384u, 1089361492u, 1858690350u),
    SC(2757340308u, 2538321018u, 2388474793u, 379482919u, 882562385u, 3129659692u, 4216198588u, 3565768337u, 1772023241u, 2931080253u, 3451485646u, 748689895u, 562737327u, 663797632u, 3315310934u, 2629536884u),
    SC(242169331u, 1243063456u, 175561111u, 2950276224u, 3213816292u, 692329775u, 3181354285u, 3015261169u, 1744760252u, 3733849950u, 4219512025u, 693702734u, 2844842003u, 722286940u, 2391355922u, 3564773447u),
    SC(2291286292u, 966238959u, 506903622u, 2122264528u, 1392182009u, 3447321781u, 3873294792u, 1373792940u, 991667700u, 2332723711u, 2764968211u, 2471301595u, 649629323u, 783169152u, 1459916213u, 3846736182u),
    SC(2664330880u, 1149932862u, 1416201114u, 318583284u, 4140857901u, 1128356267u, 1095497693u, 1624736741u, 761312690u, 241788645u, 2036924781u, 1946525101u, 3225208750u, 4156033061u, 2590150721u, 3771407135u),
    SC(2862143077u, 233168744u, 2659004990u, 155440145u, 3918377979u, 1360152661u, 627903232u, 1469886352u, 2876841580u, 3955906097u, 580277652u, 3039511497u, 1597126708u, 1404269416u, 42059925u, 2098341602u),
    SC(812381463u, 3272442363u, 496180006u, 1236237424u, 2267310113u, 2237850197u, 1113026387u, 716498059u, 3503382440u, 328287114u, 1410789607u, 477863076u, 1362085890u, 3569642059u, 2006757845u, 675415451u),
    SC(747557402u, 4212477852u, 3286869720u, 3708058361u, 3240421074u, 1188732842u, 916816078u, 2444327052u, 2111479336u, 1745064524u, 3637408011u, 3599633029u, 4230973048u, 1160089497u, 1136388910u, 4138160782u),
    SC(1255139572u, 1856599651u, 1458352865u, 3271906169u, 3410637086u, 2119040671u, 1680850868u, 413922813u, 2782309328u, 3561735700u, 3723648708u, 609378416u, 268989415u, 3293584485u, 3271843364u, 1954072630u),
    SC(4155626312u, 931793228u, 1049414704u, 1037617746u, 265265177u, 616902615u, 844384832u, 3477591939u, 3106685802u, 2357099686u, 1845236259u, 3355104451u, 3327830357u, 3100545339u, 1162051156u, 2646331847u),
    SC(514329180u, 948073745u, 1774920952u, 105860125u, 2811186644u, 1695131452u, 940976033u, 2019732362u, 309099076u, 1607914408u, 4118428245u, 1337868060u, 3952860679u, 2578427283u, 265792106u, 295755030u),
    SC(3882528435u, 2629929072u, 1617404150u, 1421619579u, 2309432083u, 724299897u, 2666040048u, 1096383838u, 1836447402u, 426930713u, 3934220119u, 3232225281u, 1000075862u, 3631628825u, 3529619355u, 1219322120u),
    SC(3335633324u, 4194223138u, 3901817518u, 1335914529u, 3871871049u, 3709757137u, 3499113177u, 235348888u, 781652835u, 1102256292u, 3754223033u, 833068853u, 4178470716u, 1807198743u, 2733399861u, 3740356601u),
    SC(228568838u, 3126580587u, 4000897922u, 1303869372u, 3850020302u, 1548458239u, 2356371812u, 3570971356u, 2544858219u, 4220062752u, 2062616152u, 953792592u, 764216612u, 2052428514u, 2314665964u, 2792116584u)
},
{
    SC(2022030201u, 622422758u, 4099630680u, 255591669u, 2746707126u, 492890866u, 1170945474u, 626140794u, 2553916130u, 3034177025u, 437361978u, 3530139681u, 3716731527u, 788732176u, 2733886498u, 780490151u),
    SC(4207089618u, 3411945447u, 1960753704u, 3552759657u, 1130668432u, 848791484u, 3810908171u, 353148861u, 3312275539u, 2963747704u, 2966813687u, 2483733320u, 2880725255u, 463405312u, 3340834122u, 1292390014u),
    SC(2664721153u, 4108676217u, 2604619822u, 775242570u, 636236518u, 2873717047u, 1857718302u, 2091477716u, 1586310695u, 2528697445u, 2256487867u, 2787362203u, 2741360704u, 496928924u, 601271512u, 3586110309u),
    SC(1791685197u, 4242641311u, 3369628733u, 2052809939u, 806398185u, 3412279529u, 1946210627u, 1398934260u, 3077042954u, 2276630414u, 814388665u, 1749609309u, 3367688729u, 1959714965u, 2411157301u, 2263996211u),
    SC(439326213u, 4256425445u, 876987216u, 1314194194u, 3010100734u, 1576065730u, 598365157u, 3705087566u, 3427486218u, 1877721147u, 358249820u, 410263983u, 1386735339u, 573015435u, 3312164843u, 1274000474u),
    SC(1340417963u, 1112802360u, 10328826u, 706586684u, 2526013892u, 4135069035u, 3566832565u, 2945858092u, 107866747u, 2114273476u, 1970904771u, 965191541u, 1793617219u, 1453495760u, 4269949644u, 41605060u),
    SC(123137558u, 4245690796u, 820317976u, 1443287541u, 4203849632u, 2954045926u, 714382464u, 3076066234u, 1293485113u, 2554869888u, 1663243834u, 1823619723u, 3832632037u, 2772671780u, 1362964704u, 558960720u),
    SC(104412626u, 1897841881u, 4081037590u, 3456756312u, 3025873323u, 2036419348u, 663042483u, 1254379139u, 1882881825u, 3296543036u, 153313200u, 916960321u, 2276001640u, 759388499u, 1134495268u, 1699779658u),
    SC(4218137867u, 889442133u, 2322944798u, 2659784159u, 2592614267u, 3345396604u, 3647495000u, 2837331949u, 75759322u, 2350992064u, 2461684340u, 2333444962u, 60872001u, 106935728u, 2095087192u, 2026584532u),
    SC(818402121u, 2851948581u, 2197490142u, 4158011576u, 1665124994u, 3116095068u, 4019154383u, 478938546u, 1455910301u, 1844755722u, 2818772446u, 2743310120u, 1907022363u, 1639658700u, 517605614u, 2705809838u),
    SC(335193145u, 4147885949u, 3527556636u, 2575925391u, 2530836608u, 2938195122u, 3771589905u, 2663025172u, 4017017665u, 2146447634u, 3974365403u, 2994000421u, 3198356067u, 3382731724u, 2593683495u, 3554902256u),
    SC(1108422413u, 1982378939u, 2047758090u, 246779179u, 2568353687u, 279750626u, 1730233650u, 784289836u, 2712478714u, 3614283837u, 1824826964u, 2514128237u, 3308726345u, 3623735281u, 887459898u, 3896777957u),
    SC(3527405352u, 290146745u, 125808293u, 735109902u, 1788801307u, 3306408847u, 822599754u, 3798637803u, 1514985656u, 2967186195u, 716984495u, 3386310843u, 3156794500u, 1007814159u, 1629566196u, 4265651874u),
    SC(1178327293u, 565847309u, 518944000u, 3901419432u, 941693255u, 4276272755u, 3595637504u, 1831384538u, 553054976u, 3799273120u, 516961220u, 3048859574u, 1887176404u, 3648800625u, 2905989893u, 2971331974u),
    SC(561598562u, 3812086269u, 2571795641u, 1946669885u, 4094345694u, 1247304730u, 725275648u, 2382611624u, 3912910386u, 3657806663u, 2347179560u, 3311073478u, 3031523768u, 2672297551u, 829774364u, 4138790294u),
    SC(3908534093u, 41076189u, 4026661177u, 1264946070u, 3582612650u, 3167460834u, 3305185564u, 1828271691u, 1883569901u, 567401887u, 2154847219u, 3599749472u, 834678216u, 1517326104u, 465030801u, 2253777505u)
},
{
    SC(69398569u, 525452511u, 2938319650u, 1880483009u, 3967907249u, 2829806383u, 1621746321u, 1916983616u, 1370370736u, 248894365u, 3788903479u, 221658457u, 404383926u, 1308961733u, 2635279776u, 2619294254u),
    SC(4116760418u, 3197079795u, 2972456007u, 1278881079u, 1399016013u, 267334468u, 3129907813u, 468505870u, 1237093446u, 3810554944u, 1980244001u, 1830827024u, 4255330344u, 3556724451u, 2936427778u, 3969278111u),
    SC(3989128687u, 604159041u, 3302470711u, 1703086807u, 4153485525u, 2444501021u, 449535888u, 2817157702u, 3967126593u, 3774839729u, 4230523164u, 1130105305u, 2419296875u, 560268503u, 173246097u, 1794638932u),
    SC(1735434103u, 3810847770u, 4216841726u, 1126260487u, 1019034952u, 4140633019u, 3223272164u, 440162565u, 3864068825u, 3275406276u, 2196958479u, 4212485308u, 539037402u, 431338309u, 4061221107u, 4289896057u),
    SC(1802752446u, 2780168117u, 1133399256u, 2599868866u, 3158418134u, 2848371717u, 2893014484u, 1878597835u, 139427334u, 1841937895u, 2016179766u, 2330806831u, 3849381146u, 2224326221u, 2296824272u, 3983748073u),
    SC(1520559143u, 1690628296u, 1614953069u, 1422707415u, 257987514u, 3063997315u, 2652769123u, 3445956897u, 843436720u, 4264023440u, 365609354u, 2250088148u, 2769492081u, 59746990u, 1275187671u, 1973406172u),
    SC(2823162534u, 2631304853u, 2485683334u, 33106529u, 243176015u, 492943806u, 489814307u, 4023911334u, 4139752347u, 4133120235u, 2455727203u, 1293330101u, 1838339727u, 4219498628u, 2131345625u, 3646653738u),
    SC(4198202713u, 3167956639u, 2765023077u, 3652537372u, 1708707687u, 2324231909u, 1009881825u, 1679047879u, 2515346176u, 794145218u, 554048969u, 3173445869u, 2193645289u, 1271864237u, 1006139617u, 1072905092u),
    SC(4273823033u, 1749314885u, 4263358248u, 538495360u, 4104454924u, 1997598205u, 3080563305u, 3238994582u, 3099819109u, 3162260128u, 1706963773u, 405274298u, 1894479347u, 1596497438u, 1094591269u, 1522128209u),
    SC(2640931764u, 1304425992u, 2939922746u, 3918107623u, 1248692482u, 1121191585u, 2062140937u, 1807331998u, 3643560968u, 3236720945u, 2667270358u, 411521120u, 3664086365u, 2334989504u, 2668098536u, 3236026237u),
    SC(2404161740u, 567514400u, 3895963765u, 1201374790u, 674719322u, 2894222365u, 467511362u, 3395036514u, 1038550674u, 2948454520u, 1518702565u, 1362236790u, 157238862u, 3475771959u, 1415257606u, 2714484334u),
    SC(1831986705u, 588754101u, 4075551797u, 2767613701u, 2944855428u, 1912813036u, 1398542170u, 3440695634u, 2367865816u, 842155635u, 2602621363u, 2143763320u, 4256143529u, 1826541687u, 1851134007u, 2997377819u),
    SC(3699972731u, 227995919u, 3067674252u, 477404832u, 847958753u, 893077929u, 2153170373u, 3057114881u, 1197132301u, 3330088847u, 2465660906u, 549749504u, 722435391u, 4124201578u, 3419977887u, 636305133u),
    SC(3346980455u, 338882355u, 1940861469u, 2106574528u, 4065634984u, 939438415u, 880899904u, 173329243u, 3962520186u, 3417951565u, 2532850810u, 1158609417u, 1846710650u, 305050726u, 600225342u, 3684765712u),
    SC(1932816778u, 3409537322u, 2445361402u, 1740774412u, 3661005378u, 2854030637u, 1914937560u, 1558250179u, 3808763123u, 1298026979u, 2417248681u, 899022004u, 847010236u, 506303181u, 1296472514u, 648957572u),
    SC(600303058u, 722185115u, 3110060002u, 3818809602u, 1551617161u, 4208042174u, 526230670u, 1957951010u, 3160030963u, 3295123990u, 3121214191u, 1337066151u, 2200271451u, 1066776105u, 1163805043u, 2606444927u)
},
{
    SC(1137648243u, 3815904636u, 35128896u, 1498158156u, 2482392993u, 1978830034u, 1585381051u, 335710867u, 529205549u, 1286325760u, 863511412u, 283835652u, 936788847u, 101075250u, 116973165u, 2483395918u),
    SC(2210369250u, 711585268u, 1961210974u, 1353321439u, 1215935705u, 1641330999u, 11213011u, 2020212318u, 695107713u, 3413272123u, 1378074688u, 2790029989u, 658491086u, 1881545465u, 3409839898u, 2042086316u),
    SC(1723393102u, 3373492622u, 3599711002u, 3748987970u, 1143620470u, 2663282777u, 2229588531u, 2674289435u, 2963045423u, 2234232397u, 4178299567u, 2791622546u, 4001934471u, 757990509u, 2858420658u, 605204372u),
    SC(4272330873u, 3840847353u, 659917277u, 1664684318u, 1563018625u, 821178295u, 3329580379u, 794312951u, 2169136998u, 1706378889u, 3017987093u, 1159314572u, 2524368718u, 2444830959u, 898030098u, 68613446u),
    SC(3172236096u, 1547478676u, 3467968131u, 1603626860u, 1411948645u, 2916654969u, 2891471305u, 2110051838u, 1733578576u, 2788816800u, 1613389791u, 759324595u, 3991538909u, 4073480091u, 3323038139u, 2043658072u),
    SC(3011536148u, 2207224783u, 101813390u, 4149858178u, 961260436u, 3760245299u, 2099300570u, 3143747485u, 3209436103u, 902146054u, 3598885374u, 597299239u, 1369786353u, 2099087354u, 1506359374u, 1017249349u),
    SC(3137350455u, 1622014086u, 2828880803u, 599881832u, 2213606365u, 4248974065u, 675350384u, 1446749674u, 1254778294u, 1745968946u, 409433048u, 1103126998u, 2370471436u, 1143685003u, 3341252280u, 1003299547u),
    SC(2019014241u, 1108099665u, 1035538349u, 2878848993u, 2585673617u, 1565675366u, 2261830657u, 117854892u, 1965053814u, 2351841804u, 4065720752u, 3747135308u, 959541091u, 1629950401u, 4236240320u, 189693687u),
    SC(3443026785u, 3216851941u, 278623472u, 1568038608u, 1548544711u, 2243949731u, 3359141033u, 1425753427u, 2934907774u, 2301245979u, 2216178210u, 153063705u, 1690071616u, 791861830u, 1201756636u, 1249732113u),
    SC(2497506925u, 3815453805u, 1308318422u, 1061717857u, 710358190u, 3797004413u, 1870767051u, 2099598345u, 845543228u, 2941187056u, 1083282999u, 1311194087u, 3227025541u, 423673289u, 2634724972u, 3297305091u),
    SC(1394185841u, 1653557808u, 2313575976u, 1732811292u, 2133445032u, 171245194u, 3242484287u, 2667183179u, 1165233778u, 997752293u, 501180123u, 2529762237u, 429212016u, 1660866777u, 1766992150u, 2066419882u),
    SC(945381459u, 1085161105u, 3490034658u, 983140246u, 425352282u, 2175943302u, 1166850024u, 3968884285u, 1417959566u, 3386676357u, 3168826489u, 2984241621u, 3305143707u, 246924146u, 4113453679u, 123892017u),
    SC(1498291154u, 979168666u, 2565114847u, 3722708999u, 3116533535u, 2044826765u, 118913881u, 2684275795u, 30932180u, 3147559151u, 3769605849u, 2376328043u, 753602217u, 3789763983u, 1247346722u, 4123341034u),
    SC(3203969599u, 2514533821u, 1007395325u, 2063305304u, 520326691u, 3823758018u, 3095693832u, 1864628246u, 2586004821u, 4190638257u, 2952735262u, 2977139992u, 1124651421u, 295756268u, 3428261546u, 3110485030u),
    SC(1663042556u, 4114384947u, 1430450710u, 3825340149u, 1051862436u, 3194752601u, 3106848742u, 1383208530u, 3142397378u, 4065704146u, 1545077688u, 2297695627u, 3152458457u, 4134880529u, 2187655177u, 3419805764u),
    SC(3081663242u, 3880428040u, 2670880433u, 1398290076u, 1232125961u, 3862005121u, 1297357575u, 3334998678u, 1135063881u, 1723120988u, 2716095891u, 1113861429u, 3955845594u, 88397004u, 1699846421u, 887623013u)
},
{
    SC(2668669863u, 1518051232u, 591131964u, 3625564717u, 2443152079u, 2589878039u, 747840157u, 1417298109u, 2236109461u, 625624150u, 2276484522u, 3671203634u, 3004642785u, 2519941048u, 286358016u, 3502187361u),
    SC(1979235571u, 2198968296u, 3104128030u, 1368659294u, 3672213117u, 1391937809u, 2759329883u, 1389958836u, 2420411428u, 890766213u, 2707043165u, 2738550562u, 3382941095u, 378763942u, 3093409509u, 2964936317u),
    SC(738589056u, 2116353374u, 2279888429u, 1705963022u, 828292114u, 896734726u, 2179570630u, 199574728u, 977051187u, 779668316u, 2330529056u, 3992755888u, 1000402439u, 2191612089u, 357145081u, 1441305104u),
    SC(3372185571u, 1990378702u, 1181109789u, 3007260699u, 2430812419u, 1342872134u, 2198044770u, 1122343273u, 492870646u, 795688582u, 3226537448u, 1245881435u, 1071312339u, 1997541910u, 3829149062u, 1964864598u),
    SC(3005241683u, 2859584860u, 2297396821u, 999606499u, 3964655188u, 3075624064u, 1368424820u, 847579236u, 744318941u, 1201524211u, 1104903258u, 3771742070u, 4093550286u, 53333408u, 659192149u, 3026115299u),
    SC(3415227510u, 2060701016u, 1724277801u, 2661091313u, 215175235u, 1719160017u, 2940192603u, 1942243742u, 2398510742u, 4053370504u, 720436957u, 3760614784u, 2014232625u, 4199009336u, 2658914393u, 246186938u),
    SC(446126854u, 165933106u, 2141828870u, 892600041u, 4146883601u, 2127439849u, 3431174989u, 2697318886u, 754216027u, 2671089369u, 1463409379u, 2826265846u, 334206028u, 1562078629u, 62819702u, 350080249u),
    SC(3607678201u, 1305808009u, 3724583207u, 482185919u, 703873206u, 1075587326u, 1772056430u, 1356871295u, 4212601732u, 3762698616u, 2707284202u, 752961239u, 3089561250u, 1634547883u, 2919906767u, 31529502u),
    SC(299389109u, 1252069111u, 2304374236u, 1252642323u, 2415535563u, 271885157u, 592252779u, 1178960198u, 53568246u, 3149254195u, 2937703855u, 1474069228u, 1764301842u, 954790502u, 4245417136u, 3132108431u),
    SC(2094400513u, 3190829985u, 2239253067u, 2918833540u, 4106202305u, 2502268912u, 1731261142u, 2453877410u, 1861934729u, 934615026u, 3785479199u, 3605446967u, 3582056355u, 3042887218u, 1961855879u, 496882544u),
    SC(3179454680u, 881405516u, 158640787u, 2790186672u, 162147899u, 376983910u, 3379568747u, 1408037207u, 1411174731u, 535638557u, 1510230718u, 2856041085u, 1958999115u, 3678347246u, 2958940834u, 520309445u),
    SC(1870118851u, 1980314816u, 3987573623u, 4117586697u, 396136405u, 3149345244u, 70002589u, 2314836548u, 1713919226u, 3789182954u, 2123295507u, 3015665476u, 4069315088u, 3980795614u, 2021907367u, 4155874670u),
    SC(4078777812u, 3708497519u, 1529048728u, 3747007128u, 2780224299u, 2728976580u, 3953400499u, 550363476u, 3812495996u, 3116459113u, 2211909765u, 3967732138u, 315888386u, 4202077281u, 1437542127u, 2815522910u),
    SC(3236576167u, 3189780679u, 2030714184u, 2121402515u, 772212369u, 2193424420u, 1417920098u, 2031545011u, 4110769775u, 697022136u, 1206489717u, 1691036150u, 88940849u, 535864250u, 547921653u, 2569798466u),
    SC(598120112u, 3876471191u, 3533286352u, 3003233155u, 1039593763u, 2148663879u, 2659932582u, 279051507u, 988977723u, 3458445518u, 2950275676u, 4048574808u, 3093122873u, 831143981u, 214208408u, 3935649503u),
    SC(2893621405u, 3242329790u, 1948255717u, 4083664057u, 3803596193u, 740414223u, 4293576836u, 3875047642u, 667197150u, 2081112783u, 2447275650u, 242164299u, 706345359u, 1928593492u, 1774391838u, 3660333945u)
},
{
    SC(3009793609u, 3525092161u, 3245586135u, 574145899u, 4034974232u, 2828949446u, 3457574134u, 1546193476u, 3883480541u, 1976722737u, 3557056370u, 994794948u, 106991499u, 1626704265u, 3534503938u, 3271872260u),
    SC(2939511082u, 3508735083u, 975571643u, 1775005849u, 4144127005u, 706007446u, 420750190u, 1296964164u, 3061654480u, 2268588398u, 258119220u, 1152421762u, 2183948554u, 3016917902u, 1186604447u, 3147111215u),
    SC(405897674u, 923178082u, 1575208079u, 3088321769u, 2214762612u, 3893926734u, 3167279390u, 3951912989u, 2709000001u, 2390687969u, 3858727239u, 866338457u, 2045181240u, 3217044625u, 2328560686u, 1861539550u),
    SC(1277015638u, 1098202702u, 1559301990u, 2587773702u, 236499920u, 458659357u, 2353007333u, 2611100088u, 3428309717u, 2008274629u, 3647015407u, 268886847u, 2626192792u, 3341061984u, 1515395072u, 3708589435u),
    SC(4042661445u, 3420460388u, 402520550u, 3677541300u, 2230979515u, 1273170666u, 2514471146u, 827498216u, 1259202696u, 3072082970u, 475301020u, 2118811945u, 3612811582u, 1387362670u, 2779447975u, 2265478999u),
    SC(2229583001u, 1885758268u, 2744744533u, 2751282929u, 3032060674u, 1949605811u, 1570835257u, 793354274u, 1683039266u, 449593771u, 109462780u, 1941150268u, 1808732776u, 139050949u, 2225765509u, 1246293964u),
    SC(2802845617u, 3765730171u, 462111640u, 590276976u, 2549490668u, 1227143343u, 384473299u, 1872236586u, 2432932105u, 2621627369u, 29218585u, 3541815309u, 3762320683u, 3470760231u, 2011203130u, 2527437401u),
    SC(796052351u, 4037990088u, 4017471553u, 1320960316u, 561010825u, 3728618461u, 3540350568u, 1334322515u, 2252671868u, 3217596003u, 3122272084u, 3124892250u, 146022162u, 3584383023u, 2911266650u, 2958817688u),
    SC(161418820u, 3776882969u, 4050624816u, 1522984750u, 3239766493u, 3767349571u, 782872272u, 4177710199u, 1140123311u, 211837022u, 1955996644u, 402816745u, 3326870942u, 1443720320u, 1645866695u, 3832886909u),
    SC(452931871u, 3201459109u, 3989748495u, 3779670060u, 3234605835u, 2462489907u, 3541849378u, 3952908948u, 2234764749u, 2534999097u, 1221823414u, 2220662906u, 2593424893u, 3688122472u, 2131104831u, 243658822u),
    SC(1244527825u, 1331697159u, 1126644730u, 922926684u, 1475975786u, 704282514u, 1718439968u, 1878820141u, 2509443841u, 2182928123u, 1663057853u, 2828328506u, 1475048880u, 791101245u, 3209045799u, 807262644u),
    SC(1506123994u, 75559732u, 2487617790u, 2776679170u, 2522687136u, 3704896305u, 945074946u, 2943008309u, 1088584510u, 2469322363u, 1078526500u, 2073262975u, 691596720u, 2702927487u, 380178128u, 704842212u),
    SC(1460389583u, 4274587105u, 1447626425u, 3957246995u, 1621878179u, 1643627976u, 4030517934u, 1056559397u, 1438644008u, 32976965u, 2197709285u, 3567855255u, 2001746745u, 2603748421u, 3462117821u, 903804357u),
    SC(3179129705u, 2297226467u, 1646197352u, 950157362u, 2929140164u, 4242027992u, 1652798968u, 4193267428u, 3343133888u, 2499845914u, 423061238u, 3494957413u, 3637365392u, 784231823u, 595573026u, 2713123590u),
    SC(2810225213u, 3951319549u, 1905650326u, 3909017486u, 2335763951u, 3772810842u, 2983632261u, 489145948u, 4173940274u, 2703192453u, 2654763363u, 4064871590u, 1399005653u, 257836626u, 831912020u, 895345820u),
    SC(4037755568u, 3145789767u, 2141184942u, 4120133888u, 346636610u, 3895536529u, 2259736314u, 1057113066u, 595225270u, 3051392771u, 2813693848u, 3877775276u, 1832280309u, 1138362004u, 3061980317u, 858203300u)
},
{
    SC(941124125u, 1620226392u, 1431256941u, 3336438938u, 540497787u, 766040889u, 373284400u, 2979905322u, 177008709u, 2625544842u, 1096614388u, 1196846420u, 4186360501u, 3945210662u, 1143943919u, 3412870088u),
    SC(2868459499u, 3255324438u, 807131982u, 2853200483u, 3487859623u, 3501857558u, 3107820062u, 2163227213u, 2115527726u, 2346720657u, 2251713340u, 3377131273u, 3223650794u, 3766790266u, 177525458u, 4167009497u),
    SC(311132793u, 3961991670u, 3475828441u, 4275227465u, 4114440759u, 287999228u, 3329759386u, 2384037498u, 4228771259u, 844254234u, 256179964u, 1796107218u, 3127243322u, 1425447302u, 1385509204u, 1101567113u),
    SC(2084416542u, 1837746358u, 3915669193u, 60671540u, 2731498203u, 842785439u, 103116859u, 3404407266u, 2713222963u, 3049100113u, 368142082u, 2923502225u, 3018451818u, 2169399182u, 3017634865u, 1845463402u),
    SC(1620925474u, 3368534446u, 555437218u, 4144603563u, 1969376145u, 213474605u, 1856420595u, 3939242692u, 1705488978u, 252956811u, 1258322279u, 1776729832u, 3988114536u, 3572272198u, 1383845751u, 1398527932u),
    SC(1762997475u, 799707654u, 1609033889u, 2324053368u, 2951656833u, 2545022095u, 1325992886u, 2638191889u, 737853621u, 891297811u, 1613139572u, 594983169u, 2686965496u, 4040759974u, 1496585540u, 294269531u),
    SC(3866323582u, 3807637640u, 654389167u, 993860478u, 3985490230u, 874636344u, 2342980699u, 1928023737u, 1520117329u, 644165140u, 150615609u, 199275733u, 463804864u, 310744654u, 2057873049u, 1169977839u),
    SC(239011286u, 715635161u, 1855226016u, 2750348850u, 4059485278u, 800137564u, 3998891997u, 4048007508u, 1194893107u, 3761772527u, 273800027u, 653240081u, 1187997500u, 310579555u, 786511222u, 3092283411u),
    SC(3036944959u, 3482022954u, 3739636749u, 3919006909u, 4266819119u, 1212326408u, 103856594u, 597427799u, 1319114089u, 4260737761u, 1982976744u, 741084092u, 689793522u, 4260038527u, 1319231386u, 1661185367u),
    SC(3846585080u, 1572901113u, 2683774833u, 3251385733u, 3753876990u, 849242549u, 4245340911u, 1064393430u, 3309340124u, 2842098330u, 2556268102u, 2033409485u, 757257328u, 2031055308u, 487255243u, 3197919149u),
    SC(273355511u, 2413549351u, 710350577u, 1361281890u, 2485522754u, 1210096318u, 3839671116u, 3619357718u, 3954210633u, 312725146u, 3792397974u, 3833954588u, 1779821907u, 2701218449u, 2422680647u, 3829673069u),
    SC(379167192u, 3494512635u, 855436470u, 2928216366u, 4239059924u, 4254878455u, 3617218283u, 739826290u, 3488721213u, 1288540569u, 2623691196u, 4237777587u, 1234356449u, 2367467024u, 185343202u, 2198868227u),
    SC(333398980u, 1306721698u, 1267933489u, 3888643170u, 2305763143u, 1886386521u, 2247721544u, 1287414137u, 497238456u, 1934421131u, 1960709128u, 2688614248u, 3637710577u, 3756130276u, 1929365309u, 2796038772u),
    SC(772805737u, 461244658u, 3551164236u, 4177074918u, 3920537361u, 4259237061u, 3625379235u, 3715444221u, 3444473673u, 2576271136u, 2750230085u, 2167864295u, 2571239709u, 3663560660u, 743894391u, 703945624u),
    SC(2955504442u, 4192737708u, 2813336533u, 2037901957u, 1563142269u, 620241136u, 3249364868u, 1805455553u, 422364625u, 3061329310u, 3824436397u, 1640020182u, 2540832302u, 2063844885u, 2982901072u, 2809011473u),
    SC(4188085081u, 1849071252u, 4251112483u, 1368274267u, 2811635355u, 3535120523u, 478922770u, 1090405967u, 2358353504u, 2249592823u, 2367480425u, 1158857070u, 1979230110u, 3661225756u, 2903524693u, 1830110173u)
},
{
    SC(3638948794u, 3243385178u, 2365114888u, 1084927340u, 2097158816u, 336310452u, 231393062u, 580838002u, 3851653288u, 568877195u, 3846156888u, 2754011062u, 3396743120u, 2639744892u, 1431686029u, 1903473537u),
    SC(3268926613u, 1818698216u, 1862252109u, 1578913474u, 4289804840u, 1885759995u, 2888888373u, 2636129891u, 2360477693u, 1672434489u, 4188472821u, 2046052045u, 437371108u, 3454488779u, 2151384078u, 1514762405u),
    SC(3140765176u, 3623124217u, 3204258419u, 1994235030u, 4141313973u, 3067394014u, 3891883464u, 3387486245u, 3254639322u, 1970078634u, 2106725210u, 2833086525u, 1670513208u, 472865524u, 2121280699u, 2548725819u),
    SC(309446023u, 3610145983u, 678094472u, 3223511337u, 4188624231u, 2675209562u, 619208065u, 1214683627u, 307823706u, 3407147709u, 2103429213u, 3636822787u, 2441204583u, 1675916090u, 1444359140u, 2979809856u),
    SC(1982287011u, 2286805587u, 3436767742u, 3002584758u, 477850697u, 439716674u, 3863581947u, 2155905635u, 220608999u, 1402913678u, 2974580099u, 1207717136u, 3265452095u, 2174870701u, 464004734u, 3218951674u),
    SC(2374025586u, 3926883961u, 3555874460u, 1238670328u, 856489843u, 4258163476u, 977941661u, 3889087192u, 2262660846u, 1677408901u, 2922467369u, 1043137100u, 4279650771u, 3357788771u, 1512036754u, 2539641395u),
    SC(1142842756u, 272648505u, 914080820u, 4056304706u, 1529598235u, 1542384711u, 898735874u, 77881967u, 1035144846u, 702992091u, 2075420139u, 2454875215u, 1266516833u, 2974932401u, 3666315911u, 2262316403u),
    SC(282628724u, 2966722803u, 3533567779u, 2474391608u, 1236598744u, 3094620093u, 2714845907u, 369896328u, 366951725u, 2971547133u, 2753808137u, 618960857u, 2006195012u, 551749950u, 1402811398u, 3808228405u),
    SC(962649761u, 2486282608u, 1808066694u, 2361174774u, 234593415u, 400975056u, 83848885u, 1091105486u, 1020816894u, 1838575736u, 2668167699u, 73800319u, 2028242253u, 2121917721u, 1921251529u, 2828854963u),
    SC(2717497535u, 366873177u, 336873963u, 978494261u, 2877822089u, 2054875183u, 2521644031u, 4057807064u, 3713415744u, 3955164880u, 2229410320u, 3755022307u, 3363858805u, 1398106956u, 800395520u, 1799982442u),
    SC(399227430u, 164572050u, 2101616757u, 962629850u, 1654784623u, 3459989194u, 2240801569u, 1986371042u, 1911756881u, 2723553175u, 2964071573u, 3609789600u, 3185432638u, 2208423303u, 2967147750u, 4279453877u),
    SC(282950688u, 2418348758u, 1686423600u, 1392917024u, 3343336708u, 976718153u, 671781049u, 4166009090u, 371505957u, 2474457927u, 1126253569u, 3355537407u, 4151375790u, 2105071839u, 941370857u, 331122028u),
    SC(2127306191u, 1587304141u, 1137651997u, 1529991785u, 1356564935u, 726775332u, 1952136309u, 4003891353u, 61741949u, 780292838u, 1136081573u, 1836882786u, 528077243u, 30578492u, 465809744u, 2709331701u),
    SC(4118645416u, 3394012023u, 348789448u, 3808052591u, 1284813572u, 265335400u, 545565522u, 929596026u, 744207086u, 3837069751u, 130735480u, 1107476780u, 910486599u, 2623115273u, 1478462314u, 2130033795u),
    SC(1955617954u, 1897311939u, 3110934223u, 4221780767u, 1556888759u, 3849614629u, 306928433u, 3178221670u, 2099698284u, 308858727u, 2221495536u, 1221057715u, 974275765u, 2399830054u, 3285960273u, 1758193777u),
    SC(1309372774u, 3725783295u, 3135972452u, 3122681380u, 3898315320u, 1245625291u, 3684458552u, 2498694383u, 145248803u, 3480764710u, 874108791u, 2482726617u, 434324108u, 1522025692u, 3554266182u, 2125028368u)
},
{
    SC(4095464112u, 3774124339u, 1954448156u, 2941024780u, 584234335u, 483707475u, 286644251u, 3027719344u, 2257880535u, 651454587u, 3313147574u, 3910046631u, 3169039651u, 2576160449u, 696031594u, 3062648739u),
    SC(3459141530u, 1009969738u, 35229281u, 2373814441u, 355537356u, 4228991558u, 213496956u, 1669603654u, 1552983955u, 3304370832u, 604896268u, 499179421u, 2737968344u, 807678026u, 3567168353u, 2353882345u),
    SC(2454671851u, 2184874449u, 831795291u, 1169825676u, 1084590471u, 1942690394u, 2762211706u, 3042637679u, 2365319338u, 3552008694u, 348752618u, 993280940u, 1178602031u, 1559708076u, 3354759347u, 972286478u),
    SC(2677560697u, 4247966509u, 151962163u, 3310844434u, 2986095882u, 3914030856u, 3436387520u, 860446559u, 4289606749u, 2343453766u, 3218454181u, 293342071u, 1238022655u, 3938175190u, 1394478132u, 4256084776u),
    SC(3033685698u, 1795086146u, 719843849u, 255984080u, 2447365525u, 874035973u, 313642533u, 1163634918u, 2316564524u, 1195940716u, 1914843207u, 3907025376u, 23457264u, 1278433300u, 3111232984u, 668125878u),
    SC(2135745017u, 2899432034u, 1819124473u, 2109840859u, 3124696519u, 2070710502u, 990727745u, 2752134271u, 1963223245u, 866344359u, 606159585u, 3867224292u, 3038840373u, 3295910586u, 2433460716u, 3384811471u),
    SC(1744070416u, 383286836u, 3000319326u, 3310329765u, 4062980155u, 2749127191u, 1895582230u, 439084228u, 1884304792u, 326674045u, 377650590u, 3363592478u, 2947641322u, 1784390018u, 1332541121u, 4203919218u),
    SC(472957101u, 1135650637u, 4212757570u, 185931877u, 2096733734u, 4238795506u, 481917546u, 1405180051u, 925427330u, 1923351053u, 2204480714u, 3944038373u, 372144582u, 3395978522u, 3795034464u, 1074487901u),
    SC(227727393u, 2219043153u, 2909459085u, 3082645761u, 1970114976u, 3426610084u, 35253812u, 3123666967u, 4231900027u, 2888054525u, 2744804820u, 1500359618u, 191232240u, 3239664209u, 1569663960u, 1330983134u),
    SC(996304063u, 2759713926u, 1022152104u, 4268512678u, 2870837640u, 3507597858u, 1252922637u, 3276898019u, 3824649934u, 1524401760u, 2559990337u, 1660220688u, 2350855385u, 609332995u, 2406016501u, 2406242521u),
    SC(3333888266u, 3838886221u, 3016467419u, 3341790649u, 3667104212u, 783789160u, 1310400762u, 3633793516u, 4105695306u, 2973076533u, 455893547u, 2864660063u, 3696934279u, 2872882056u, 2264350097u, 539812697u),
    SC(3263458726u, 2820785414u, 3760367911u, 628854049u, 1473785327u, 426717862u, 2025377226u, 3498407835u, 3577945153u, 1319190911u, 1062047947u, 3346460201u, 2590672215u, 2723591074u, 1487439866u, 4217021014u),
    SC(2076058913u, 33130418u, 1949000294u, 3536165044u, 31327487u, 1891010986u, 2347335564u, 1669503944u, 3753248202u, 881959988u, 3846164684u, 3636142472u, 208517894u, 3407391141u, 3485893709u, 1074365179u),
    SC(2175348532u, 3463201667u, 168136052u, 2889266255u, 4105885613u, 3068947090u, 2279310533u, 2649966235u, 828612565u, 2017635648u, 1260407590u, 1970316631u, 2447304459u, 2893112079u, 2425504835u, 1197046834u),
    SC(2653983058u, 1419924288u, 2320709126u, 3640188854u, 2683911962u, 2643927342u, 3261193464u, 3929873787u, 2878724355u, 3436083049u, 3424148509u, 1311037973u, 3116391362u, 2037892948u, 454042580u, 970415398u),
    SC(16199673u, 2464180001u, 89776423u, 672570852u, 2291071982u, 3899998968u, 4262439281u, 412856039u, 3677249728u, 1182323568u, 3472045521u, 3554674668u, 819725249u, 4078699211u, 2037243914u, 4166444096u)
},
{
    SC(1740919499u, 3877396933u, 2326751436u, 2985697421u, 1447445291u, 2255966095u, 1611141497u, 1834170313u, 3589822942u, 2703601378u, 299681739u, 3037417379u, 4014970727u, 2126073701u, 3064037855u, 2610138122u),
    SC(2959647136u, 3814991611u, 764778261u, 1677371416u, 497556143u, 1000564042u, 4065791500u, 1027030318u, 2636763418u, 2469599275u, 839050056u, 4115114412u, 3982189672u, 2204140838u, 1747652790u, 3786215179u),
    SC(3812425833u, 3703652912u, 1980699604u, 1506061914u, 2330998846u, 3874717363u, 20614012u, 1484655664u, 2896690261u, 1196646483u, 159078055u, 1300317512u, 2570981831u, 1267318554u, 3037645632u, 3117135345u),
    SC(2012483448u, 279997059u, 1908492604u, 1638405820u, 284407565u, 1607271004u, 1423855670u, 3949669604u, 1635878907u, 4045715556u, 3600475894u, 3387647818u, 3950223476u, 3109131487u, 2524676171u, 3329048150u),
    SC(3505120665u, 1999377488u, 158974979u, 636438923u, 1767149410u, 2424026197u, 532320013u, 3350230775u, 3506414357u, 999737675u, 3415715721u, 797201045u, 3439137094u, 3636888232u, 1001867404u, 1070514934u),
    SC(803341976u, 972240723u, 2174569332u, 4037031657u, 720363583u, 1532359940u, 222173943u, 3948724459u, 669414977u, 446802288u, 4195328223u, 2316597014u, 3039478974u, 1217500351u, 1058613984u, 3974805650u),
    SC(2497689022u, 832535973u, 4012390289u, 3862385792u, 473134599u, 855172718u, 3160709443u, 2946049581u, 1340978834u, 1282260619u, 3672935594u, 1114896253u, 1194768191u, 2151967837u, 3557909289u, 83919397u),
    SC(2685697085u, 4183307820u, 393931333u, 2425217781u, 2950365274u, 2300063381u, 3990090983u, 1961757942u, 3357278228u, 2993935030u, 779960569u, 3652282828u, 1743505267u, 3193034940u, 2134245237u, 4042181132u),
    SC(2449311128u, 4037657778u, 318968012u, 1098807866u, 3241626396u, 745989749u, 4126255071u, 850508142u, 4075976689u, 357235455u, 2000916706u, 3900438139u, 2804084317u, 3036848582u, 604252796u, 2006800965u),
    SC(101955641u, 2732365617u, 2730133770u, 3908553062u, 2872853047u, 264325893u, 2086018926u, 546076667u, 582367640u, 2242336949u, 2223649162u, 1521240572u, 178342991u, 3408523296u, 2216853754u, 1636770650u),
    SC(1697876449u, 998213608u, 2367869150u, 3635535434u, 3029347602u, 2697162358u, 300760335u, 3790588806u, 3127970813u, 157171921u, 2766714052u, 3441353031u, 3760111386u, 1962222723u, 1338315915u, 1705537099u),
    SC(2069540711u, 3174156395u, 3834082852u, 2243125169u, 1332693007u, 1773075089u, 820191370u, 262117783u, 184405617u, 469065021u, 1286610377u, 946922506u, 2233109630u, 2803987975u, 489850357u, 3341265389u),
    SC(3152895344u, 3190413328u, 1371373852u, 2133030998u, 2097773989u, 3484604561u, 3233580762u, 2103971308u, 580626917u, 3723142348u, 1233964596u, 2884246809u, 1451113068u, 2274332609u, 834566918u, 4166322862u),
    SC(474309298u, 31198476u, 474732582u, 1614612386u, 2339718649u, 702598622u, 2007092771u, 1563921691u, 3096928870u, 2036801390u, 3171632090u, 2666464957u, 2581592302u, 84487705u, 4066440296u, 250703600u),
    SC(2850943751u, 3355276358u, 3608928556u, 645558581u, 1754003398u, 2401097307u, 4007141515u, 2306720640u, 2585847442u, 2486681168u, 916961025u, 2906286711u, 2183350629u, 3403456959u, 1234360906u, 608407455u),
    SC(3919397u, 2910764499u, 1130649170u, 2504839137u, 475960727u, 4198145923u, 3575554927u, 727034596u, 3487299979u, 2134210036u, 1295494166u, 1094003986u, 3153584442u, 1125501956u, 1050325095u, 3018071122u)
},
{
    SC(1456510740u, 215912204u, 253318863u, 2775298218u, 3073705928u, 3154352632u, 3237812190u, 434409115u, 3593346865u, 3020727994u, 1910411353u, 2325723409u, 1818165255u, 3742118891u, 4111316616u, 4010457359u),
    SC(2413332453u, 1353953544u, 4051432026u, 303594340u, 1259813651u, 366336945u, 3380747343u, 2634392445u, 2066562619u, 120707135u, 1398541407u, 502464084u, 2984999938u, 3829298149u, 1120989122u, 3373752257u),
    SC(1681071159u, 120984332u, 2029459879u, 1382039080u, 3634662556u, 54408822u, 48099449u, 1179080842u, 2669759950u, 3169946602u, 1520730683u, 3878549631u, 1666070500u, 1804495215u, 1101808889u, 1988315741u),
    SC(1810699040u, 1982264875u, 1311915666u, 268159494u, 1265118580u, 1494821999u, 2740360551u, 3403457379u, 2370002476u, 3663200326u, 1969174367u, 2988878975u, 2261867571u, 1896957751u, 4228495601u, 268030737u),
    SC(3788031612u, 1459331879u, 4195039120u, 148760443u, 2710036304u, 3803193725u, 2316636996u, 1290739855u, 2078515077u, 1158390637u, 187516666u, 1165781180u, 3871854912u, 2887741280u, 3432370474u, 3017515415u),
    SC(2660400581u, 1115514969u, 819611304u, 2438542525u, 1149450061u, 641570348u, 4195260176u, 114239580u, 3415942550u, 2418164759u, 3596450733u, 4170880111u, 3742333800u, 707266970u, 294392938u, 1502400257u),
    SC(4244209414u, 4144723933u, 1206802017u, 3395049043u, 1534528858u, 212213384u, 273948964u, 2465871688u, 98513287u, 526054552u, 101003852u, 2178852720u, 1739213138u, 2000068838u, 3443316390u, 2907641948u),
    SC(4170329393u, 2397160575u, 698736458u, 1726629095u, 2059726015u, 608224441u, 940962377u, 3160021800u, 2474105021u, 1418624931u, 3220142189u, 3165061177u, 609263259u, 3526248509u, 2451110984u, 882122082u),
    SC(1803413035u, 2626850042u, 3923382679u, 2501640460u, 887077755u, 2970691407u, 3982443858u, 546345352u, 545064661u, 1905866916u, 4137411501u, 4293519422u, 399697152u, 2101209662u, 4081268472u, 3745325674u),
    SC(3913855272u, 3324082002u, 2401043817u, 1769760109u, 2460560183u, 875956117u, 1942607787u, 1641754800u, 1964565342u, 442388011u, 1687580604u, 293988342u, 3046598358u, 2835075967u, 920490836u, 349604594u),
    SC(2643665013u, 1607952309u, 2279132309u, 992705865u, 1231530495u, 2682680275u, 2340070945u, 1036310446u, 2160469638u, 3849593659u, 569936175u, 133751759u, 1309000826u, 3681058360u, 1289881501u, 385711414u),
    SC(1190130845u, 2798968177u, 277741425u, 3875973536u, 2502592372u, 251555512u, 1825737360u, 462006518u, 2334535950u, 3997809264u, 2012251623u, 3408888487u, 2549759312u, 3379458376u, 2301581275u, 4171117892u),
    SC(1923456093u, 1653002750u, 3279649712u, 4281661052u, 1248011568u, 933375742u, 2109342469u, 751470571u, 2742486580u, 2572871261u, 3296809419u, 4075155428u, 3182626853u, 3435860599u, 3916597057u, 245531435u),
    SC(514908612u, 2222061780u, 506774061u, 381342968u, 789366883u, 3683832850u, 9270407u, 528428861u, 590313143u, 483933274u, 1128871308u, 2791400346u, 3033966006u, 2397900561u, 174539653u, 2363998101u),
    SC(3558289816u, 1015432688u, 3960686128u, 2087286003u, 446928557u, 4028273076u, 3055038539u, 885707705u, 942001648u, 3175434773u, 3929872598u, 2961036794u, 1122092143u, 2142675404u, 4054255588u, 1958229328u),
    SC(2852327378u, 1383667573u, 3763466478u, 3195889922u, 2107642962u, 1739908882u, 157313327u, 492435243u, 4236498733u, 1510923342u, 3227437908u, 1896980749u, 154410481u, 2958311799u, 3270353062u, 1889012642u)
},
{
    SC(822693957u, 1703644293u, 3960229340u, 2092754577u, 3495958557u, 4288710741u, 4092815138u, 1275224613u, 2592916775u, 472063207u, 2931222331u, 2597044591u, 1261640449u, 1272207288u, 2040245568u, 1417421068u),
    SC(57865933u, 2591783175u, 1332940705u, 2361514832u, 2842982424u, 2581566511u, 1328343723u, 3898369656u, 2090549923u, 2179715082u, 2370481583u, 775215786u, 3850307123u, 2489521783u, 3999750482u, 1014134079u),
    SC(2011629934u, 1914036612u, 3406392133u, 1425412057u, 1338374071u, 683386303u, 3190457777u, 428137206u, 1251032257u, 3672462899u, 2593185313u, 1953316437u, 2123216916u, 3258622817u, 3197533388u, 3442579011u),
    SC(265734183u, 884987600u, 2786263189u, 3536027957u, 3885575220u, 1854265340u, 3853595664u, 1987453181u, 2744740518u, 512197390u, 114481815u, 96285071u, 3293497789u, 4015333892u, 4092376929u, 3025411574u),
    SC(612519829u, 3198151239u, 3191059512u, 226844204u, 3503855660u, 764021515u, 3628841562u, 3951882416u, 3622158804u, 3603368155u, 2780109382u, 822859403u, 25907739u, 3882220368u, 3789068172u, 1684074913u),
    SC(3520260226u, 1656105499u, 1676578448u, 838040958u, 3130046810u, 995588852u, 3233766730u, 2629592527u, 3096399775u, 1659682138u, 1365617549u, 2450677843u, 1725372848u, 2623357383u, 1402837393u, 1993344168u),
    SC(2434333993u, 2901722469u, 518468307u, 3322336116u, 3303354477u, 2422295273u, 3584734361u, 1255342255u, 2224600785u, 3752112711u, 3720624102u, 3425652159u, 3563799906u, 957522630u, 501907560u, 3362627156u),
    SC(3271809032u, 2402529419u, 3935184016u, 3639910664u, 659985988u, 2584831332u, 1091987512u, 224789177u, 2944016703u, 3591574599u, 1273021052u, 967556634u, 1019501719u, 1864898605u, 3453844870u, 4011599553u),
    SC(1326048883u, 3477092042u, 1799777609u, 296885426u, 1109310872u, 255028335u, 163456938u, 2108662143u, 3501831646u, 225777648u, 4099069764u, 3428610561u, 4069711767u, 3876386370u, 1215899260u, 369937558u),
    SC(3466874302u, 1921411468u, 3753149186u, 3739960133u, 1909238781u, 2219053499u, 4040572016u, 1651280893u, 754573870u, 383500798u, 2400558032u, 922698902u, 2125517085u, 2541623325u, 2827334144u, 2773618829u),
    SC(2040368526u, 2190975469u, 1347589661u, 1684817146u, 2021572959u, 1656810013u, 330975936u, 994237514u, 2596719101u, 3800849855u, 600269956u, 1857741551u, 3033366103u, 1496147464u, 2628189942u, 4210116847u),
    SC(3076719908u, 2490548320u, 377911263u, 2002478742u, 2549252529u, 839159951u, 230337140u, 3095221595u, 1528132928u, 2083899038u, 2503451113u, 272698731u, 2624407067u, 161482016u, 4135914440u, 2519252428u),
    SC(2556876861u, 2107629748u, 2377697213u, 1433609947u, 3343742332u, 3505415093u, 2690575000u, 2017949066u, 4133794057u, 4184820210u, 2960078982u, 1333558937u, 3733636790u, 3960011078u, 945143131u, 3343864106u),
    SC(1801254589u, 1449097227u, 181948563u, 1034221031u, 1779862110u, 3141289560u, 3383585093u, 2578193674u, 554670851u, 2530857925u, 4076682145u, 2827602863u, 4244507626u, 2938597885u, 3223414171u, 2204001183u),
    SC(291814305u, 2937237569u, 1434020428u, 3585179044u, 3677832974u, 2016114805u, 3981784693u, 538800869u, 2673738915u, 999373833u, 1457987857u, 3180983013u, 501300267u, 4103517997u, 997980659u, 1113009463u),
    SC(3993610129u, 1037741502u, 330412440u, 2749687355u, 1555232145u, 1196959672u, 530284980u, 340384986u, 2298150586u, 3185141181u, 26985524u, 2219307959u, 2447245692u, 1065988754u, 1248620406u, 2208024308u)
},
{
    SC(3660855132u, 3816892380u, 3431508003u, 1440179111u, 768988979u, 3652895254u, 2084463131u, 3991218655u, 323118457u, 3675476946u, 2157306354u, 2684850253u, 1543808805u, 744627428u, 1091926767u, 3538062578u),
    SC(2810298495u, 3411171710u, 4062828084u, 3003344135u, 3264709694u, 1048068132u, 3549102117u, 1927032841u, 3841604555u, 1360558064u, 2204714588u, 1197341693u, 3768005385u, 2899352192u, 2849083812u, 3793398404u),
    SC(3631867959u, 3146872034u, 420513606u, 2446059169u, 2652499910u, 429155541u, 748397809u, 3543114527u, 235482177u, 894763888u, 1086818023u, 3285579564u, 1810274445u, 1142434275u, 140188668u, 4059040723u),
    SC(2682453748u, 1595694625u, 17869409u, 4001607469u, 759206176u, 3336900820u, 3693692341u, 2473365492u, 2714988574u, 637563477u, 4105755464u, 3161387095u, 2814461644u, 4283494186u, 3858290792u, 1516784203u),
    SC(4062605051u, 1956634460u, 3701616314u, 2342355265u, 1267526896u, 464674235u, 2247549950u, 3633206724u, 296547100u, 2905295542u, 4077085273u, 2746567644u, 1803616500u, 918536622u, 2709233803u, 2413530101u),
    SC(1383097263u, 1316928613u, 759541292u, 3793001510u, 257497874u, 3658838865u, 3213596633u, 3650670599u, 63812226u, 1947202098u, 3651967368u, 2399936732u, 2521262969u, 322630211u, 4004516883u, 1422335688u),
    SC(2852550033u, 3224936812u, 733055828u, 3325391168u, 1930707186u, 731324754u, 3498518219u, 4117056191u, 2179511600u, 2761523161u, 4282458808u, 3042559735u, 2438675720u, 2532100345u, 3706723018u, 4059342362u),
    SC(2048163474u, 1848349034u, 3258863528u, 3644103333u, 1151231486u, 3308192205u, 2814277731u, 4197063636u, 3510455851u, 1315219655u, 2185965649u, 3799505477u, 4254363720u, 3128925961u, 1852465545u, 4138612075u),
    SC(960983998u, 3301464188u, 2737893955u, 1522861436u, 4164105020u, 1184099683u, 64022400u, 2368856028u, 326418376u, 2065332946u, 2081529277u, 3466798514u, 208026276u, 417986090u, 3587033208u, 2294843214u),
    SC(2712989146u, 349068332u, 3978782854u, 1513755929u, 4281030368u, 4041238337u, 1631550267u, 936378809u, 3831648862u, 1780262732u, 3189639539u, 328937247u, 722753719u, 3671027558u, 215485348u, 294998383u),
    SC(170533035u, 3100330628u, 2519007245u, 2729143680u, 1780483799u, 1771308699u, 777046078u, 1252661309u, 944830935u, 3219243484u, 2959537667u, 145170296u, 892161275u, 1151850054u, 2176346749u, 598783080u),
    SC(3596882604u, 51304713u, 1277701547u, 3288737023u, 2143659411u, 1229626338u, 2504854740u, 2518260221u, 2909459409u, 3820898741u, 1076396276u, 3330086214u, 2070741501u, 1675949151u, 4169029889u, 2072266145u),
    SC(3395707749u, 1912264784u, 839246291u, 1812660322u, 2590197689u, 3115125394u, 280633483u, 1476186344u, 2182942190u, 4022517575u, 1314348304u, 2211853573u, 1730367526u, 3842875309u, 1411362967u, 749836026u),
    SC(822183119u, 2084092802u, 2957672615u, 1548122281u, 2555590320u, 4127903458u, 704941703u, 3216796016u, 1310798669u, 1681974379u, 2704001393u, 836064664u, 2498528840u, 2878347924u, 3344415063u, 1714110968u),
    SC(3763417450u, 1647484613u, 2916400914u, 1340277384u, 3671023234u, 2962715012u, 2086976330u, 2356641838u, 861453503u, 2497852292u, 3384683911u, 2044029625u, 3423593678u, 602612346u, 1947876325u, 1071593133u),
    SC(502143537u, 3800930061u, 289630048u, 2019675509u, 690814111u, 1395759030u, 2095320716u, 1658529388u, 2140950369u, 4113871752u, 2130755443u, 1184235968u, 2624156111u, 1053548247u, 1666584094u, 3436241707u)
},
{
    SC(2819478132u, 2629026829u, 2945562911u, 1854605392u, 41922071u, 2531530491u, 2316774439u, 3550381961u, 1180787169u, 3914439365u, 3786421842u, 3441223373u, 494782102u, 2858003292u, 1448968751u, 2940369046u),
    SC(1228705098u, 2320747717u, 1742025124u, 3358828738u, 1857762103u, 2669617968u, 2684123743u, 2427291148u, 3948024171u, 3841263454u, 3817968782u, 3617000488u, 3457510946u, 3443415072u, 3976288418u, 291039859u),
    SC(1118114309u, 1364783097u, 3986370035u, 1058514953u, 3723130907u, 2966082807u, 1592373613u, 4029958112u, 1261460522u, 159904028u, 385928252u, 2962822321u, 213058425u, 39305506u, 3400567258u, 2953928339u),
    SC(4004285350u, 3275325131u, 2912133301u, 482119944u, 699333459u, 1353300830u, 498723416u, 2738735797u, 3773472794u, 1167510524u, 1995708610u, 1872986795u, 1771998886u, 460328822u, 2566240531u, 3665251184u),
    SC(870908870u, 249845288u, 3674648542u, 3670939624u, 3213883826u, 2765218754u, 3292181727u, 1765634472u, 2846619223u, 156162860u, 2158300764u, 3792761756u, 4248292998u, 1588571137u, 1696144875u, 2915693433u),
    SC(1257645965u, 743351844u, 3299328612u, 1606739395u, 2242479072u, 526126122u, 3132670209u, 2327012389u, 1257540758u, 1688790030u, 864103666u, 1782879705u, 2344074317u, 878043196u, 569218289u, 3875319913u),
    SC(676712446u, 2310487862u, 3297058723u, 154140360u, 1534807165u, 2207878247u, 4002312458u, 1195155314u, 3973562995u, 203866583u, 1307033594u, 1808951889u, 3485439766u, 2123920858u, 3400721970u, 628518531u),
    SC(453432196u, 3506137302u, 962794710u, 2800823697u, 944975983u, 445662356u, 620440622u, 225699982u, 1038708892u, 3484553780u, 4174808994u, 3862318255u, 1961625058u, 2183421173u, 2682639230u, 3890472885u),
    SC(3472048934u, 1436162338u, 4281682055u, 1419885595u, 1926695253u, 861477946u, 2586543901u, 2286266784u, 2854911092u, 1779735787u, 2994125983u, 2248840912u, 677288518u, 3593153557u, 3383199489u, 2094768467u),
    SC(971218259u, 3653638590u, 3374334294u, 479058129u, 1331477004u, 2497262229u, 892109896u, 3651901580u, 1455849852u, 2738531309u, 14202660u, 1968080740u, 1927308794u, 897128363u, 3654300057u, 1275380700u),
    SC(684658124u, 660984744u, 2929312783u, 1473333980u, 1562502960u, 656352357u, 338449257u, 2159155320u, 2425193686u, 930413364u, 2001285554u, 307432757u, 2238003500u, 1858295105u, 481986971u, 1067622012u),
    SC(943383548u, 127299943u, 2909652237u, 1257655712u, 4123282405u, 78394323u, 1736026340u, 2126927829u, 296638455u, 1861436609u, 641299684u, 636649068u, 3331138991u, 1014270261u, 257248847u, 1556179874u),
    SC(2668740334u, 4261010365u, 3376970497u, 2258271000u, 3369826513u, 906131732u, 12531263u, 2501581679u, 861444520u, 2059219969u, 3536488433u, 3392343056u, 3231250347u, 3425501702u, 4204845226u, 3883035310u),
    SC(875006280u, 3061145215u, 799684212u, 4150716124u, 1344915012u, 1442298502u, 887378800u, 2722425542u, 4141895498u, 4068116328u, 601774281u, 3538746538u, 1671758462u, 3066546971u, 1116345758u, 554718074u),
    SC(1149406575u, 702696847u, 505403366u, 331269161u, 664926760u, 2151357672u, 2890104906u, 3156886545u, 1199701084u, 1614409973u, 4222014462u, 1336462493u, 3214687968u, 1279434993u, 2285235388u, 2975474024u),
    SC(2419658919u, 481424988u, 2207220911u, 2736159805u, 4086711147u, 477511738u, 1428567116u, 3971000648u, 429362137u, 3495313342u, 3653961670u, 4170077754u, 2057308114u, 1445981917u, 97057494u, 3847612010u)
},
{
    SC(3017729014u, 3423125690u, 1534829496u, 1346803271u, 888659105u, 1661894766u, 4165031912u, 697485157u, 3575889724u, 1795181757u, 1507549874u, 1480154979u, 3565672142u, 830054113u, 1507719534u, 3652903656u),
    SC(4123340423u, 2168639254u, 3491407759u, 395600125u, 2056091205u, 1233197217u, 2716612715u, 3263564356u, 2257286689u, 2753339767u, 2228663460u, 3584404544u, 3972978154u, 3637886739u, 3854541466u, 1603898424u),
    SC(641806023u, 3776877383u, 3574980110u, 2564901152u, 1378226343u, 738790225u, 4030459977u, 2255719927u, 295765315u, 60094770u, 422069111u, 439158593u, 3956842123u, 1242303994u, 150522972u, 3682386439u),
    SC(2385589330u, 2076597417u, 605447848u, 3200763641u, 3106877254u, 3374069827u, 3828392492u, 1315607291u, 3211667999u, 305089333u, 179172787u, 3225149656u, 1080822644u, 3286534940u, 2231515542u, 2699760148u),
    SC(3983719183u, 1208009460u, 767048521u, 326825213u, 1087716958u, 3599826498u, 3107818740u, 2785268698u, 1304576537u, 1847155836u, 3250405674u, 2694326935u, 2163030471u, 3253944705u, 1698753082u, 3845065767u),
    SC(2823293375u, 2790862099u, 1207038844u, 3886043838u, 3567640686u, 3799791258u, 1638354726u, 1428653770u, 2075289233u, 1582582790u, 213364421u, 2858522524u, 2809903954u, 1742449197u, 324107072u, 1051562955u),
    SC(2291926834u, 1805734123u, 3420689573u, 1003089617u, 476535216u, 1334543097u, 2045923069u, 2990972415u, 1822043289u, 2128934150u, 3541372378u, 1912558832u, 2295908612u, 1500502429u, 3539272060u, 2641558214u),
    SC(3069594753u, 3051481608u, 2339450545u, 2054924228u, 4282917353u, 65440790u, 2134400604u, 3588265957u, 2569563771u, 741034486u, 740973978u, 93172292u, 1583303041u, 2980574219u, 2969067524u, 1088571815u),
    SC(78721532u, 1566330912u, 1219109269u, 3229207312u, 2345730495u, 3209647323u, 2033975193u, 1009666575u, 2794060854u, 4218956981u, 3379703631u, 2400336569u, 100401885u, 3519721431u, 4007729122u, 3851183625u),
    SC(2344993313u, 2454241381u, 3071516966u, 4207668067u, 250885582u, 1733938903u, 1658948056u, 2192440210u, 1717829063u, 849763004u, 2334162093u, 3715296533u, 1757279167u, 3270001477u, 2677428083u, 4197601814u),
    SC(2911676146u, 4069956071u, 3299890629u, 3133371278u, 3551760603u, 558967408u, 205243474u, 237180706u, 4227661901u, 390685951u, 658498389u, 225847327u, 3028263358u, 3941067795u, 1850521034u, 1584413524u),
    SC(304549398u, 3089811378u, 549382137u, 2353383127u, 2278640956u, 781853185u, 1734676013u, 3311472816u, 957105351u, 1291924767u, 2025324585u, 3897237789u, 80455313u, 302089802u, 3496158310u, 4000611245u),
    SC(1221283087u, 3865703766u, 1551786763u, 3208862988u, 2964616465u, 1429406173u, 2847895093u, 3047143885u, 3187847794u, 3875229246u, 2044093786u, 2855772466u, 2252977997u, 1253496627u, 1824313803u, 3492626272u),
    SC(1435191953u, 2954553263u, 3689501374u, 3761866706u, 3160683386u, 2172174457u, 4033800334u, 2293562561u, 500568896u, 2877151546u, 112648553u, 2760351679u, 1976713840u, 2960166087u, 1364536484u, 4127293522u),
    SC(2942286091u, 3570696800u, 2680748212u, 879905933u, 371824626u, 2796545677u, 2544287558u, 1654320774u, 3724452395u, 1875952433u, 1755420330u, 700510406u, 2122483560u, 357724466u, 2579725929u, 4152935597u),
    SC(732269412u, 3045632405u, 947036931u, 2403831527u, 2919479301u, 2947112020u, 1653738112u, 2316444303u, 3103978479u, 2856978461u, 308282125u, 1154683958u, 2086296447u, 1288456128u, 528614237u, 2945631134u)
},
{
    SC(3751554592u, 1759634227u, 4138518211u, 3130599659u, 3881948336u, 669688286u, 3672211577u, 695226401u, 1226786139u, 1855160209u, 905875552u, 2831529665u, 1625185017u, 3130043300u, 3227522138u, 3659203373u),
    SC(399372699u, 529779700u, 1206056828u, 1867177702u, 196488961u, 2148657353u, 2522788662u, 2308787051u, 1566407979u, 857878238u, 2852634973u, 2131204123u, 2812808340u, 3651465982u, 1947448513u, 3757182587u),
    SC(3732610632u, 1025396308u, 60450219u, 3075208965u, 2460440177u, 301478800u, 2020185415u, 2910424285u, 1627945543u, 473410099u, 4114096970u, 2440686062u, 3031404169u, 2099206907u, 1232790956u, 2248800462u),
    SC(2343232878u, 1198836246u, 1270188071u, 2305538045u, 1841160260u, 1049160535u, 2935147928u, 3818293360u, 2128394208u, 692132409u, 3183837651u, 981952986u, 3501941431u, 1239605342u, 1265208179u, 225920797u),
    SC(1958540456u, 418545838u, 1645667403u, 4203505141u, 81660142u, 351421726u, 2877676470u, 871152679u, 2804776066u, 431108218u, 927442607u, 3782508732u, 318483929u, 4079394971u, 1143889788u, 4195920424u),
    SC(2351179626u, 1598459225u, 3579449038u, 4292231882u, 2911534527u, 3174868713u, 2883217980u, 1046921244u, 3074833211u, 117299980u, 3425406982u, 2813303717u, 879305153u, 3439142119u, 1270010014u, 2633468950u),
    SC(3394012837u, 1133386629u, 2931266329u, 2512080059u, 3268046571u, 585832644u, 1151303760u, 4164956195u, 1787214290u, 3523549326u, 4139598868u, 530139359u, 2107355242u, 1401770006u, 4264627539u, 3014221080u),
    SC(1988836761u, 3474599222u, 2535855552u, 3118306895u, 1953046625u, 30632894u, 8987922u, 1482010220u, 1585584845u, 441041520u, 3045700482u, 362734762u, 3723600227u, 1056985402u, 2472480517u, 3558297033u),
    SC(4137318322u, 915055827u, 1432589840u, 3550795442u, 1919127293u, 1256417138u, 946345068u, 1353195020u, 2948635882u, 3916808200u, 3223857138u, 2259986522u, 636089773u, 2116476405u, 266813303u, 3992924481u),
    SC(1294364269u, 2282087282u, 719947200u, 1065389577u, 67185303u, 600695627u, 3423704882u, 507439949u, 1464333499u, 954935833u, 1949391476u, 2146234814u, 640934838u, 2477152026u, 3767255766u, 2397668523u),
    SC(1825548026u, 2780595753u, 282065873u, 3347141416u, 3152283414u, 1656153711u, 1047376382u, 3616949007u, 464657631u, 3299783240u, 1162505878u, 3862534742u, 3899846651u, 3980167606u, 2513773976u, 1803555687u),
    SC(734708953u, 181663675u, 2018505992u, 1055015000u, 2266993350u, 3679506170u, 1032089726u, 2239152753u, 3271229362u, 257492591u, 519168390u, 890304984u, 594386284u, 933877218u, 2646719799u, 439652468u),
    SC(1253204385u, 2215899770u, 848155650u, 1305331452u, 1831981169u, 4101626048u, 253253616u, 718148001u, 3846087699u, 2362703617u, 564971301u, 878503956u, 2792594154u, 3831500219u, 630060686u, 2654848235u),
    SC(2082956373u, 965635733u, 1172460454u, 3057130868u, 485386699u, 558270142u, 2819896785u, 247008390u, 1884023798u, 3291747866u, 1725636793u, 1552257124u, 171155452u, 894504521u, 3157754944u, 4135144713u),
    SC(3013624247u, 3479051648u, 3976465681u, 139584997u, 690715168u, 2972053528u, 2543659091u, 81834710u, 261064551u, 1476481099u, 2550215537u, 1381589752u, 3557508349u, 3578290922u, 1272133161u, 3008228265u),
    SC(3507369103u, 1077600519u, 1522596015u, 3088783267u, 2852999673u, 751358577u, 733140212u, 3467225217u, 100497019u, 50410977u, 68742811u, 3090618848u, 1603912616u, 2272476179u, 1767751118u, 3249696448u)
},
{
    SC(2950670644u, 1870384244u, 3964091927u, 4110714448u, 298132763u, 3177974896u, 3260855649u, 1258311080u, 2976836646u, 3581267654u, 3094482836u, 80535005u, 2024129606u, 168620678u, 4254285674u, 2577025593u),
    SC(1515179601u, 3578614970u, 3088354879u, 797813018u, 1355130048u, 1083957563u, 119796717u, 2021253602u, 1525138732u, 4127381203u, 3062851977u, 4142386071u, 1213064952u, 3609844670u, 1484215992u, 3431673114u),
    SC(1401099367u, 3953214819u, 830584870u, 2207781603u, 918659453u, 4293181358u, 4072336467u, 4282551694u, 262435288u, 1941569548u, 147995405u, 1811389750u, 4118444114u, 1252574507u, 578798636u, 1074483177u),
    SC(2872591360u, 1058667772u, 16799222u, 688522560u, 3475129040u, 3433794124u, 1076991040u, 1425059515u, 2939587530u, 236447274u, 3960100164u, 1298525395u, 2761371754u, 4025787449u, 2464666072u, 3981743594u),
    SC(3976786453u, 1358319886u, 3905641993u, 1405765539u, 2585003073u, 3447572652u, 741448872u, 3444688769u, 971292808u, 1486657617u, 3079335839u, 862424956u, 248802634u, 1703726921u, 2982469234u, 2682500687u),
    SC(4273605693u, 2467118193u, 3538801384u, 3862847335u, 1065478730u, 1602785515u, 1071410798u, 2624755760u, 2768741032u, 2700950902u, 558848464u, 3400938789u, 1410632048u, 2094050860u, 1686695852u, 2101955993u),
    SC(4124709913u, 3191744141u, 3038636619u, 2944952304u, 2687117769u, 1502766822u, 14738299u, 223780235u, 32298390u, 1195949618u, 1154476371u, 1873391152u, 273358443u, 2362272244u, 509120994u, 606974408u),
    SC(3937286725u, 1520668653u, 941545039u, 3056942351u, 574018151u, 2549472282u, 82289937u, 374652507u, 619831005u, 2134744303u, 1462663193u, 2963006112u, 3726585674u, 1797461239u, 1470634776u, 3441417480u),
    SC(2845288945u, 3925574221u, 1989126288u, 3105801567u, 210047271u, 1545005898u, 2572648420u, 2278643173u, 2633053858u, 3288168184u, 3566345146u, 165026071u, 191806458u, 4116335861u, 1768316231u, 3169297484u),
    SC(253765755u, 2509241970u, 1926513613u, 3735004917u, 4188741775u, 2806800711u, 281300019u, 3635185u, 3462483807u, 2277745510u, 1708651892u, 1413928970u, 56262931u, 531946794u, 2864634184u, 3118504241u),
    SC(4194010611u, 4232988065u, 1802432341u, 3448133339u, 3732370320u, 253801846u, 2726367450u, 3905836819u, 1373544282u, 2066678017u, 3439519431u, 3381452691u, 2754663978u, 535580478u, 2512241599u, 2720083475u),
    SC(3589933363u, 4047249230u, 2311777188u, 270484672u, 1108190662u, 2080251561u, 1724842405u, 4014518744u, 1593608472u, 2342434397u, 4205240553u, 2166622191u, 3528923u, 1996089122u, 4284726332u, 989608730u),
    SC(2475269743u, 4230552139u, 3917936952u, 3098769598u, 3209444661u, 4188126675u, 3974782724u, 3639917274u, 2711234947u, 1439392508u, 1127433801u, 478802541u, 4223040451u, 2268034322u, 2452212595u, 3508939070u),
    SC(2413851784u, 190519100u, 3576747926u, 2710481928u, 2148944938u, 3984096005u, 2427227598u, 1001464024u, 2191178977u, 1139441673u, 3841324161u, 308061908u, 3976150834u, 1467800561u, 3226772030u, 1743883019u),
    SC(281260179u, 1415659644u, 915707047u, 1662956706u, 911938094u, 3456789397u, 2082200558u, 947098788u, 4036848108u, 2455542339u, 1466205449u, 4158358953u, 586549709u, 850657486u, 61343079u, 2292663847u),
    SC(3487862268u, 4116082621u, 1969417576u, 1466595601u, 3136251120u, 3697533272u, 438943523u, 1041892750u, 1141661777u, 435333448u, 3031876514u, 2121342186u, 209290199u, 256519609u, 1400190683u, 4260080502u)
},
{
    SC(1406628534u, 2978091511u, 343468499u, 973866526u, 757277528u, 1142388839u, 2945536141u, 3759469101u, 3001571847u, 2170606364u, 1017327004u, 3120716036u, 468321128u, 3656061918u, 2331571461u, 1930702552u),
    SC(3117811324u, 4230396490u, 526101390u, 3589443580u, 12282838u, 3055128772u, 453582536u, 750425919u, 87216299u, 1999749165u, 2446098001u, 1907762611u, 183870981u, 3643605669u, 4232900175u, 2946539195u),
    SC(3903405291u, 1034986659u, 2587588236u, 1880077572u, 1696686560u, 1243434386u, 3746745675u, 2212912696u, 2031851135u, 575946730u, 2663616094u, 2706019532u, 2635197066u, 1942621203u, 3760379195u, 4173271368u),
    SC(2892050679u, 1105289247u, 1519565685u, 2426902952u, 65580444u, 3373395323u, 2112756687u, 3658806066u, 2548718870u, 3586646888u, 3350821933u, 1921239811u, 4061525916u, 3520594550u, 1872307168u, 3464547908u),
    SC(2889143489u, 489507550u, 788811400u, 1800916293u, 3249681744u, 1400920516u, 3917828215u, 1093821500u, 1905385813u, 2931012984u, 1800788801u, 1697549042u, 3133274419u, 3606456099u, 2156683634u, 3205410986u),
    SC(2814687995u, 4053305746u, 484530004u, 410862009u, 246830045u, 3164065541u, 3723774424u, 3388961612u, 3438413619u, 3662326637u, 2178649434u, 3555798301u, 164350275u, 2341607004u, 3896269562u, 1591806179u),
    SC(3226183767u, 3881369008u, 700458770u, 376569395u, 2607908019u, 1353553198u, 2636334721u, 1140283021u, 2632309194u, 1710844790u, 3031461719u, 4081969123u, 3326745889u, 4034909949u, 3950856167u, 3153389256u),
    SC(2184243175u, 2166726232u, 3921103433u, 872887260u, 623636347u, 95935618u, 2766774027u, 697875047u, 164043041u, 993154257u, 4114304816u, 3500729957u, 409872172u, 3504722710u, 2806324915u, 717798207u),
    SC(1913401183u, 1684394893u, 957780895u, 2366199383u, 3846687839u, 2225031745u, 50628017u, 764720583u, 2251658783u, 1601491318u, 3836612294u, 3836982164u, 1834686310u, 4239983357u, 2677791106u, 718595268u),
    SC(641418698u, 3008658673u, 1590313857u, 1025261614u, 1545641278u, 883067087u, 405447843u, 251932751u, 890679795u, 1380695500u, 4259157180u, 4219905082u, 665298826u, 4240175069u, 1720908833u, 2268480568u),
    SC(1323007329u, 2757671761u, 531677728u, 1863777888u, 1512057206u, 2416428007u, 297355401u, 2843988168u, 3028483811u, 4269951770u, 844221740u, 1060678479u, 2913804270u, 3550002834u, 1490208797u, 2041637686u),
    SC(4098631786u, 3088674341u, 2277647863u, 546429701u, 239595915u, 96051385u, 2043858235u, 356783975u, 3081379864u, 1495630942u, 1713035648u, 2797737429u, 4252005067u, 1174473008u, 182861961u, 1284115192u),
    SC(1497340893u, 2990980382u, 435071738u, 25048206u, 1369038540u, 2388914024u, 3985375113u, 3187649864u, 1375850783u, 2762762203u, 3714513839u, 1546363407u, 2343675571u, 416152492u, 1797618344u, 3540898582u),
    SC(2184924310u, 2347360549u, 640504537u, 1253044800u, 1440674061u, 1666425671u, 3827600864u, 2022304946u, 2918906490u, 263308814u, 3892002350u, 1942380643u, 1520343008u, 1245225248u, 3081248535u, 2098883649u),
    SC(2377054091u, 3295547231u, 2240796492u, 1757295037u, 62158041u, 1809272299u, 4005194159u, 1592984938u, 366675588u, 3144502911u, 2973082795u, 4105706826u, 2851896979u, 3262002710u, 3082369242u, 634669574u),
    SC(729159370u, 3948971047u, 1511320403u, 3061460707u, 3090283349u, 1868816562u, 3759558902u, 3868199437u, 2438888892u, 1660478281u, 2415784493u, 3546303863u, 3144683831u, 3066258755u, 2228021651u, 3294706852u)
}
};
#undef SC
#endif
///end of [autogenerated]ecmult_static_context.h







///From secp256k1/src/ecmult_gen_impl.h
/**********************************************************************
 * Copyright (c) 2013, 2014, 2015 Pieter Wuille, Gregory Maxwell      *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/
void secp256k1_ecmult_gen_context_init(secp256k1_ecmult_gen_context *ctx) {
    ctx->prec = NULL;
}

void secp256k1_ecmult_gen_context_build(secp256k1_ecmult_gen_context *ctx, const secp256k1_callback* cb) {
#ifndef USE_ECMULT_STATIC_PRECOMPUTATION
    secp256k1_ge prec[1024];
    secp256k1_gej gj;
    secp256k1_gej nums_gej;
    int i, j;
#endif

    if (ctx->prec != NULL) {
        return;
    }
#ifndef USE_ECMULT_STATIC_PRECOMPUTATION
    ctx->prec = (secp256k1_ge_storage (*)[64][16])checked_malloc(cb, sizeof(*ctx->prec));

    /* get the generator */
    secp256k1_gej_set_ge(&gj, &secp256k1_ge_const_g);

    /* Construct a group element with no known corresponding scalar (nothing up my sleeve). */
    {
        static const unsigned char nums_b32[33] = "The scalar for this x is unknown";
        secp256k1_fe nums_x;
        secp256k1_ge nums_ge;
        int r;
        r = secp256k1_fe_set_b32(&nums_x, nums_b32);
        (void)r;
        VERIFY_CHECK(r);
        r = secp256k1_ge_set_xo_var(&nums_ge, &nums_x, 0);
        (void)r;
        VERIFY_CHECK(r);
        secp256k1_gej_set_ge(&nums_gej, &nums_ge);
        /* Add G to make the bits in x uniformly distributed. */
        secp256k1_gej_add_ge_var(&nums_gej, &nums_gej, &secp256k1_ge_const_g, NULL);
    }

    /* compute prec. */
    {
        secp256k1_gej precj[1024]; /* Jacobian versions of prec. */
        secp256k1_gej gbase;
        secp256k1_gej numsbase;
        gbase = gj; /* 16^j * G */
        numsbase = nums_gej; /* 2^j * nums. */
        for (j = 0; j < 64; j++) {
            /* Set precj[j*16 .. j*16+15] to (numsbase, numsbase + gbase, ..., numsbase + 15*gbase). */
            precj[j*16] = numsbase;
            for (i = 1; i < 16; i++) {
                secp256k1_gej_add_var(&precj[j*16 + i], &precj[j*16 + i - 1], &gbase, NULL);
            }
            /* Multiply gbase by 16. */
            for (i = 0; i < 4; i++) {
                secp256k1_gej_double_var(&gbase, &gbase, NULL);
            }
            /* Multiply numbase by 2. */
            secp256k1_gej_double_var(&numsbase, &numsbase, NULL);
            if (j == 62) {
                /* In the last iteration, numsbase is (1 - 2^j) * nums instead. */
                secp256k1_gej_neg(&numsbase, &numsbase);
                secp256k1_gej_add_var(&numsbase, &numsbase, &nums_gej, NULL);
            }
        }
        secp256k1_ge_set_all_gej_var(prec, precj, 1024, cb);
    }
    for (j = 0; j < 64; j++) {
        for (i = 0; i < 16; i++) {
            secp256k1_ge_to_storage(&(*ctx->prec)[j][i], &prec[j*16 + i]);
        }
    }
#else
    (void)cb;
    ctx->prec = (secp256k1_ge_storage (*)[64][16])secp256k1_ecmult_static_context;
#endif
    secp256k1_ecmult_gen_blind(ctx, NULL);
}

int secp256k1_ecmult_gen_context_is_built(const secp256k1_ecmult_gen_context* ctx) {
    return ctx->prec != NULL;
}

void secp256k1_ecmult_gen_context_clone(secp256k1_ecmult_gen_context *dst,
                                               const secp256k1_ecmult_gen_context *src, const secp256k1_callback* cb) {
    if (src->prec == NULL) {
        dst->prec = NULL;
    } else {
#ifndef USE_ECMULT_STATIC_PRECOMPUTATION
        dst->prec = (secp256k1_ge_storage (*)[64][16])checked_malloc(cb, sizeof(*dst->prec));
        memcpy(dst->prec, src->prec, sizeof(*dst->prec));
#else
        (void)cb;
        dst->prec = src->prec;
#endif
        dst->initial = src->initial;
        dst->blind = src->blind;
    }
}

void secp256k1_ecmult_gen_context_clear(secp256k1_ecmult_gen_context *ctx) {
#ifndef USE_ECMULT_STATIC_PRECOMPUTATION
    free(ctx->prec);
#endif
    secp256k1_scalar_clear(&ctx->blind);
    secp256k1_gej_clear(&ctx->initial);
    ctx->prec = NULL;
}

void secp256k1_ecmult_gen(const secp256k1_ecmult_gen_context *ctx, secp256k1_gej *r, const secp256k1_scalar *gn) {
    secp256k1_ge add;
    secp256k1_ge_storage adds;
    secp256k1_scalar gnb;
    int bits;
    int i, j;
    memset(&adds, 0, sizeof(adds));
    *r = ctx->initial;
    /* Blind scalar/point multiplication by computing (n-b)G + bG instead of nG. */
    secp256k1_scalar_add(&gnb, gn, &ctx->blind);
    add.infinity = 0;
    for (j = 0; j < 64; j++) {
        bits = secp256k1_scalar_get_bits(&gnb, j * 4, 4);
        for (i = 0; i < 16; i++) {
            /** This uses a conditional move to avoid any secret data in array indexes.
             *   _Any_ use of secret indexes has been demonstrated to result in timing
             *   sidechannels, even when the cache-line access patterns are uniform.
             *  See also:
             *   "A word of warning", CHES 2013 Rump Session, by Daniel J. Bernstein and Peter Schwabe
             *    (https://cryptojedi.org/peter/data/chesrump-20130822.pdf) and
             *   "Cache Attacks and Countermeasures: the Case of AES", RSA 2006,
             *    by Dag Arne Osvik, Adi Shamir, and Eran Tromer
             *    (http://www.tau.ac.il/~tromer/papers/cache.pdf)
             */
            secp256k1_ge_storage_cmov(&adds, &(*ctx->prec)[j][i], i == bits);
        }
        secp256k1_ge_from_storage(&add, &adds);
        secp256k1_gej_add_ge(r, r, &add);
    }
    bits = 0;
    secp256k1_ge_clear(&add);
    secp256k1_scalar_clear(&gnb);
}

/* Setup blinding values for secp256k1_ecmult_gen. */
void secp256k1_ecmult_gen_blind(secp256k1_ecmult_gen_context *ctx, const unsigned char *seed32) {
    secp256k1_scalar b;
    secp256k1_gej gb;
    secp256k1_fe s;
    unsigned char nonce32[32];
    secp256k1_rfc6979_hmac_sha256_t rng;
    int retry;
    unsigned char keydata[64] = {0};
    if (seed32 == NULL) {
        /* When seed is NULL, reset the initial point and blinding value. */
        secp256k1_gej_set_ge(&ctx->initial, &secp256k1_ge_const_g);
        secp256k1_gej_neg(&ctx->initial, &ctx->initial);
        secp256k1_scalar_set_int(&ctx->blind, 1);
    }
    /* The prior blinding value (if not reset) is chained forward by including it in the hash. */
    secp256k1_scalar_get_b32(nonce32, &ctx->blind);
    /** Using a CSPRNG allows a failure free interface, avoids needing large amounts of random data,
     *   and guards against weak or adversarial seeds.  This is a simpler and safer interface than
     *   asking the caller for blinding values directly and expecting them to retry on failure.
     */
    memcpy(keydata, nonce32, 32);
    if (seed32 != NULL) {
        memcpy(keydata + 32, seed32, 32);
    }
    secp256k1_rfc6979_hmac_sha256_initialize(&rng, keydata, seed32 ? 64 : 32);
    memset(keydata, 0, sizeof(keydata));
    /* Retry for out of range results to achieve uniformity. */
    do {
        secp256k1_rfc6979_hmac_sha256_generate(&rng, nonce32, 32);
        retry = !secp256k1_fe_set_b32(&s, nonce32);
        retry |= secp256k1_fe_is_zero(&s);
    } while (retry); /* This branch true is cryptographically unreachable. Requires sha256_hmac output > Fp. */
    /* Randomize the projection to defend against multiplier sidechannels. */
    secp256k1_gej_rescale(&ctx->initial, &s);
    secp256k1_fe_clear(&s);
    do {
        secp256k1_rfc6979_hmac_sha256_generate(&rng, nonce32, 32);
        secp256k1_scalar_set_b32(&b, nonce32, &retry);
        /* A blinding value of 0 works, but would undermine the projection hardening. */
        retry |= secp256k1_scalar_is_zero(&b);
    } while (retry); /* This branch true is cryptographically unreachable. Requires sha256_hmac output > order. */
    secp256k1_rfc6979_hmac_sha256_finalize(&rng);
    memset(nonce32, 0, 32);
    secp256k1_ecmult_gen(ctx, &gb, &b);
    secp256k1_scalar_negate(&b, &b);
    ctx->blind = b;
    ctx->initial = gb;
    secp256k1_scalar_clear(&b);
    secp256k1_gej_clear(&gb);
}
///end of secp256k1/src/ecmult_gen_impl.h

///From secp256k1/src/ecdsa_impl.h
int secp256k1_eckey_pubkey_serialize(secp256k1_ge *elem, unsigned char *pub, size_t *size, int compressed) {
    if (secp256k1_ge_is_infinity(elem)) {
        return 0;
    }
    secp256k1_fe_normalize_var(&elem->x);
    secp256k1_fe_normalize_var(&elem->y);
    secp256k1_fe_get_b32(&pub[1], &elem->x);
    if (compressed) {
        *size = 33;
        pub[0] = 0x02 | (secp256k1_fe_is_odd(&elem->y) ? 0x01 : 0x00);
    } else {
        *size = 65;
        pub[0] = 0x04;
        secp256k1_fe_get_b32(&pub[33], &elem->y);
    }
    return 1;
}

int secp256k1_eckey_privkey_tweak_add(secp256k1_scalar *key, const secp256k1_scalar *tweak) {
    secp256k1_scalar_add(key, key, tweak);
    if (secp256k1_scalar_is_zero(key)) {
        return 0;
    }
    return 1;
}

int secp256k1_eckey_pubkey_tweak_add(const secp256k1_ecmult_context *ctx, secp256k1_ge *key, const secp256k1_scalar *tweak) {
    secp256k1_gej pt;
    secp256k1_scalar one;
    secp256k1_gej_set_ge(&pt, key);
    secp256k1_scalar_set_int(&one, 1);
    secp256k1_ecmult(ctx, &pt, &pt, &one, tweak);

    if (secp256k1_gej_is_infinity(&pt)) {
        return 0;
    }
    secp256k1_ge_set_gej(key, &pt);
    return 1;
}

int secp256k1_eckey_privkey_tweak_mul(secp256k1_scalar *key, const secp256k1_scalar *tweak) {
    if (secp256k1_scalar_is_zero(tweak)) {
        return 0;
    }

    secp256k1_scalar_mul(key, key, tweak);
    return 1;
}

int secp256k1_eckey_pubkey_tweak_mul(const secp256k1_ecmult_context *ctx, secp256k1_ge *key, const secp256k1_scalar *tweak) {
    secp256k1_scalar zero;
    secp256k1_gej pt;
    if (secp256k1_scalar_is_zero(tweak)) {
        return 0;
    }

    secp256k1_scalar_set_int(&zero, 0);
    secp256k1_gej_set_ge(&pt, key);
    secp256k1_ecmult(ctx, &pt, &pt, tweak, &zero);
    secp256k1_ge_set_gej(key, &pt);
    return 1;
}

/** Group order for secp256k1 defined as 'n' in "Standards for Efficient Cryptography" (SEC2) 2.7.1
 *  sage: for t in xrange(1023, -1, -1):
 *     ..   p = 2**256 - 2**32 - t
 *     ..   if p.is_prime():
 *     ..     print '%x'%p
 *     ..     break
 *   'fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f'
 *  sage: a = 0
 *  sage: b = 7
 *  sage: F = FiniteField (p)
 *  sage: '%x' % (EllipticCurve ([F (a), F (b)]).order())
 *   'fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141'
 */
static const secp256k1_fe secp256k1_ecdsa_const_order_as_fe = SECP256K1_FE_CONST(
    0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFEUL,
    0xBAAEDCE6UL, 0xAF48A03BUL, 0xBFD25E8CUL, 0xD0364141UL
);

/** Difference between field and order, values 'p' and 'n' values defined in
 *  "Standards for Efficient Cryptography" (SEC2) 2.7.1.
 *  sage: p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
 *  sage: a = 0
 *  sage: b = 7
 *  sage: F = FiniteField (p)
 *  sage: '%x' % (p - EllipticCurve ([F (a), F (b)]).order())
 *   '14551231950b75fc4402da1722fc9baee'
 */
static const secp256k1_fe secp256k1_ecdsa_const_p_minus_order = SECP256K1_FE_CONST(
    0, 0, 0, 1, 0x45512319UL, 0x50B75FC4UL, 0x402DA172UL, 0x2FC9BAEEUL
);

static int secp256k1_der_read_len(const unsigned char **sigp, const unsigned char *sigend) {
    int lenleft, b1;
    size_t ret = 0;
    if (*sigp >= sigend) {
        return -1;
    }
    b1 = *((*sigp)++);
    if (b1 == 0xFF) {
        /* X.690-0207 8.1.3.5.c the value 0xFF shall not be used. */
        return -1;
    }
    if ((b1 & 0x80) == 0) {
        /* X.690-0207 8.1.3.4 short form length octets */
        return b1;
    }
    if (b1 == 0x80) {
        /* Indefinite length is not allowed in DER. */
        return -1;
    }
    /* X.690-207 8.1.3.5 long form length octets */
    lenleft = b1 & 0x7F;
    if (lenleft > sigend - *sigp) {
        return -1;
    }
    if (**sigp == 0) {
        /* Not the shortest possible length encoding. */
        return -1;
    }
    if ((size_t)lenleft > sizeof(size_t)) {
        /* The resulting length would exceed the range of a size_t, so
         * certainly longer than the passed array size.
         */
        return -1;
    }
    while (lenleft > 0) {
        if ((ret >> ((sizeof(size_t) - 1) * 8)) != 0) {
        }
        ret = (ret << 8) | **sigp;
        if (ret + lenleft > (size_t)(sigend - *sigp)) {
            /* Result exceeds the length of the passed array. */
            return -1;
        }
        (*sigp)++;
        lenleft--;
    }
    if (ret < 128) {
        /* Not the shortest possible length encoding. */
        return -1;
    }
    return ret;
}

static int secp256k1_der_parse_integer(secp256k1_scalar *r, const unsigned char **sig, const unsigned char *sigend) {
    int overflow = 0;
    unsigned char ra[32] = {0};
    int rlen;

    if (*sig == sigend || **sig != 0x02) {
        /* Not a primitive integer (X.690-0207 8.3.1). */
        return 0;
    }
    (*sig)++;
    rlen = secp256k1_der_read_len(sig, sigend);
    if (rlen <= 0 || (*sig) + rlen > sigend) {
        /* Exceeds bounds or not at least length 1 (X.690-0207 8.3.1).  */
        return 0;
    }
    if (**sig == 0x00 && rlen > 1 && (((*sig)[1]) & 0x80) == 0x00) {
        /* Excessive 0x00 padding. */
        return 0;
    }
    if (**sig == 0xFF && rlen > 1 && (((*sig)[1]) & 0x80) == 0x80) {
        /* Excessive 0xFF padding. */
        return 0;
    }
    if ((**sig & 0x80) == 0x80) {
        /* Negative. */
        overflow = 1;
    }
    while (rlen > 0 && **sig == 0) {
        /* Skip leading zero bytes */
        rlen--;
        (*sig)++;
    }
    if (rlen > 32) {
        overflow = 1;
    }
    if (!overflow) {
        memcpy(ra + 32 - rlen, *sig, rlen);
        secp256k1_scalar_set_b32(r, ra, &overflow);
    }
    if (overflow) {
        secp256k1_scalar_set_int(r, 0);
    }
    (*sig) += rlen;
    return 1;
}

#include <sstream>
#include <iostream>
int secp256k1_ecdsa_sig_parse(secp256k1_scalar *rr, secp256k1_scalar *rs, const unsigned char *sig, size_t size) {
    const unsigned char *sigend = sig + size;
    int rlen;
    std::cout << "DEBUG: inside parsing of signatures" << std::endl;
    if (sig == sigend || *(sig++) != 0x30) {
        std::cout << "DEBUG: Bad signature start" << std::endl;
        /* The encoding doesn't start with a constructed sequence (X.690-0207 8.9.1). */
        return 0;
    }
    rlen = secp256k1_der_read_len(&sig, sigend);
    if (rlen < 0 || sig + rlen > sigend) {
        std::cout << "DEBUG: Bad signature start" << std::endl;
        /* Tuple exceeds bounds */
        return 0;
    }
    if (sig + rlen != sigend) {
        std::cout << "DEBUG: Garbage after tuple. rlen: " << rlen << ", sigend: " << sigend << " sig + rlen: " << (sig + rlen) << std::endl;
        /* Garbage after tuple. */
        return 0;
    }

    if (!secp256k1_der_parse_integer(rr, &sig, sigend)) {
        return 0;
    }
    if (!secp256k1_der_parse_integer(rs, &sig, sigend)) {
        return 0;
    }

    if (sig != sigend) {
        std::cout << "DEBUG: Garbage inside tuple" << std::endl;
        /* Trailing garbage inside tuple. */
        return 0;
    }
    std::cout << "DEBUG: success output" << std::endl;

    return 1;
}

int secp256k1_ecdsa_sig_serialize(unsigned char *sig, size_t *size, const secp256k1_scalar* ar, const secp256k1_scalar* as) {
    unsigned char r[33] = {0}, s[33] = {0};
    unsigned char *rp = r, *sp = s;
    size_t lenR = 33, lenS = 33;
    secp256k1_scalar_get_b32(&r[1], ar);
    secp256k1_scalar_get_b32(&s[1], as);
    while (lenR > 1 && rp[0] == 0 && rp[1] < 0x80) { lenR--; rp++; }
    while (lenS > 1 && sp[0] == 0 && sp[1] < 0x80) { lenS--; sp++; }
    if (*size < 6+lenS+lenR) {
        *size = 6 + lenS + lenR;
        return 0;
    }
    *size = 6 + lenS + lenR;
    sig[0] = 0x30;
    sig[1] = 4 + lenS + lenR;
    sig[2] = 0x02;
    sig[3] = lenR;
    memcpy(sig+4, rp, lenR);
    sig[4+lenR] = 0x02;
    sig[5+lenR] = lenS;
    memcpy(sig+lenR+6, sp, lenS);
    return 1;
}

int secp256k1_ecdsa_sig_verify(const secp256k1_ecmult_context *ctx, const secp256k1_scalar *sigr, const secp256k1_scalar *sigs, const secp256k1_ge *pubkey, const secp256k1_scalar *message) {
    unsigned char c[32];
    secp256k1_scalar sn, u1, u2;
#if !defined(EXHAUSTIVE_TEST_ORDER)
    secp256k1_fe xr;
#endif
    secp256k1_gej pubkeyj;
    secp256k1_gej pr;

    if (secp256k1_scalar_is_zero(sigr) || secp256k1_scalar_is_zero(sigs)) {
        return 0;
    }

    secp256k1_scalar_inverse_var(&sn, sigs);
    secp256k1_scalar_mul(&u1, &sn, message);
    secp256k1_scalar_mul(&u2, &sn, sigr);
    secp256k1_gej_set_ge(&pubkeyj, pubkey);
    secp256k1_ecmult(ctx, &pr, &pubkeyj, &u2, &u1);
    if (secp256k1_gej_is_infinity(&pr)) {
        return 0;
    }

#if defined(EXHAUSTIVE_TEST_ORDER)
{
    secp256k1_scalar computed_r;
    secp256k1_ge pr_ge;
    secp256k1_ge_set_gej(&pr_ge, &pr);
    secp256k1_fe_normalize(&pr_ge.x);

    secp256k1_fe_get_b32(c, &pr_ge.x);
    secp256k1_scalar_set_b32(&computed_r, c, NULL);
    return secp256k1_scalar_eq(sigr, &computed_r);
}
#else
    secp256k1_scalar_get_b32(c, sigr);
    secp256k1_fe_set_b32(&xr, c);

    /** We now have the recomputed R point in pr, and its claimed x coordinate (modulo n)
     *  in xr. Naively, we would extract the x coordinate from pr (requiring a inversion modulo p),
     *  compute the remainder modulo n, and compare it to xr. However:
     *
     *        xr == X(pr) mod n
     *    <=> exists h. (xr + h * n < p && xr + h * n == X(pr))
     *    [Since 2 * n > p, h can only be 0 or 1]
     *    <=> (xr == X(pr)) || (xr + n < p && xr + n == X(pr))
     *    [In Jacobian coordinates, X(pr) is pr.x / pr.z^2 mod p]
     *    <=> (xr == pr.x / pr.z^2 mod p) || (xr + n < p && xr + n == pr.x / pr.z^2 mod p)
     *    [Multiplying both sides of the equations by pr.z^2 mod p]
     *    <=> (xr * pr.z^2 mod p == pr.x) || (xr + n < p && (xr + n) * pr.z^2 mod p == pr.x)
     *
     *  Thus, we can avoid the inversion, but we have to check both cases separately.
     *  secp256k1_gej_eq_x implements the (xr * pr.z^2 mod p == pr.x) test.
     */
    if (secp256k1_gej_eq_x_var(&xr, &pr)) {
        /* xr * pr.z^2 mod p == pr.x, so the signature is valid. */
        return 1;
    }
    if (secp256k1_fe_cmp_var(&xr, &secp256k1_ecdsa_const_p_minus_order) >= 0) {
        /* xr + n >= p, so we can skip testing the second case. */
        return 0;
    }
    secp256k1_fe_add(&xr, &secp256k1_ecdsa_const_order_as_fe);
    if (secp256k1_gej_eq_x_var(&xr, &pr)) {
        /* (xr + n) * pr.z^2 mod p == pr.x, so the signature is valid. */
        return 1;
    }
    return 0;
#endif
}

int secp256k1_ecdsa_sig_sign(const secp256k1_ecmult_gen_context *ctx, secp256k1_scalar *sigr, secp256k1_scalar *sigs, const secp256k1_scalar *seckey, const secp256k1_scalar *message, const secp256k1_scalar *nonce, int *recid) {
    unsigned char b[32];
    secp256k1_gej rp;
    secp256k1_ge r;
    secp256k1_scalar n;
    int overflow = 0;

    secp256k1_ecmult_gen(ctx, &rp, nonce);
    secp256k1_ge_set_gej(&r, &rp);
    secp256k1_fe_normalize(&r.x);
    secp256k1_fe_normalize(&r.y);
    secp256k1_fe_get_b32(b, &r.x);
    secp256k1_scalar_set_b32(sigr, b, &overflow);
    /* These two conditions should be checked before calling */
    VERIFY_CHECK(!secp256k1_scalar_is_zero(sigr));
    VERIFY_CHECK(overflow == 0);

    if (recid) {
        /* The overflow condition is cryptographically unreachable as hitting it requires finding the discrete log
         * of some P where P.x >= order, and only 1 in about 2^127 points meet this criteria.
         */
        *recid = (overflow ? 2 : 0) | (secp256k1_fe_is_odd(&r.y) ? 1 : 0);
    }
    secp256k1_scalar_mul(&n, sigr, seckey);
    secp256k1_scalar_add(&n, &n, message);
    secp256k1_scalar_inverse(sigs, nonce);
    secp256k1_scalar_mul(sigs, sigs, &n);
    secp256k1_scalar_clear(&n);
    secp256k1_gej_clear(&rp);
    secp256k1_ge_clear(&r);
    if (secp256k1_scalar_is_zero(sigs)) {
        return 0;
    }
    if (secp256k1_scalar_is_high(sigs)) {
        secp256k1_scalar_negate(sigs, sigs);
        if (recid) {
            *recid ^= 1;
        }
    }
    return 1;
}
///end of secp256k1/src/ecdsa_impl.h

///From secp256k1/src/eckey_impl.h
int secp256k1_eckey_pubkey_parse(secp256k1_ge *elem, const unsigned char *pub, size_t size) {
    if (size == 33 && (pub[0] == 0x02 || pub[0] == 0x03)) {
        secp256k1_fe x;
        return secp256k1_fe_set_b32(&x, pub+1) && secp256k1_ge_set_xo_var(elem, &x, pub[0] == 0x03);
    } else if (size == 65 && (pub[0] == 0x04 || pub[0] == 0x06 || pub[0] == 0x07)) {
        secp256k1_fe x, y;
        if (!secp256k1_fe_set_b32(&x, pub+1) || !secp256k1_fe_set_b32(&y, pub+33)) {
            return 0;
        }
        secp256k1_ge_set_xy(elem, &x, &y);
        if ((pub[0] == 0x06 || pub[0] == 0x07) && secp256k1_fe_is_odd(&y) != (pub[0] == 0x07)) {
            return 0;
        }
        return secp256k1_ge_is_valid_var(elem);
    } else {
        return 0;
    }
}
///end of secp256k1/src/eckey_impl.h

































///From secp256k1.c
#define ARG_CHECK(cond) do { \
    if (EXPECT(!(cond), 0)) { \
        secp256k1_callback_call(&ctx->illegal_callback, #cond); \
        return 0; \
    } \
} while(0)

void default_illegal_callback_fn(const char* str, void* data) {
    (void)data;
    fprintf(stderr, "[libsecp256k1] illegal argument: %s\n", str);
    abort();
}

static const secp256k1_callback default_illegal_callback = {
    default_illegal_callback_fn,
    NULL
};

void default_error_callback_fn(const char* str, void* data) {
    (void)data;
    fprintf(stderr, "[libsecp256k1] internal consistency check failed: %s\n", str);
    abort();
}

static const secp256k1_callback default_error_callback = {
    default_error_callback_fn,
    NULL
};

secp256k1_context* secp256k1_context_create(unsigned int flags) {
    secp256k1_context* ret = (secp256k1_context*)checked_malloc(&default_error_callback, sizeof(secp256k1_context));
    ret->illegal_callback = default_illegal_callback;
    ret->error_callback = default_error_callback;

    if (EXPECT((flags & SECP256K1_FLAGS_TYPE_MASK) != SECP256K1_FLAGS_TYPE_CONTEXT, 0)) {
            secp256k1_callback_call(&ret->illegal_callback,
                                    "Invalid flags");
            free(ret);
            return NULL;
    }

    secp256k1_ecmult_context_init(&ret->ecmult_ctx);
    secp256k1_ecmult_gen_context_init(&ret->ecmult_gen_ctx);

    if (flags & SECP256K1_FLAGS_BIT_CONTEXT_SIGN) {
        secp256k1_ecmult_gen_context_build(&ret->ecmult_gen_ctx, &ret->error_callback);
    }
    if (flags & SECP256K1_FLAGS_BIT_CONTEXT_VERIFY) {
        secp256k1_ecmult_context_build(&ret->ecmult_ctx, &ret->error_callback);
    }

    return ret;
}

secp256k1_context* secp256k1_context_clone(const secp256k1_context* ctx) {
    secp256k1_context* ret = (secp256k1_context*)checked_malloc(&ctx->error_callback, sizeof(secp256k1_context));
    ret->illegal_callback = ctx->illegal_callback;
    ret->error_callback = ctx->error_callback;
    secp256k1_ecmult_context_clone(&ret->ecmult_ctx, &ctx->ecmult_ctx, &ctx->error_callback);
    secp256k1_ecmult_gen_context_clone(&ret->ecmult_gen_ctx, &ctx->ecmult_gen_ctx, &ctx->error_callback);
    return ret;
}

void secp256k1_context_destroy(secp256k1_context* ctx) {
    if (ctx != NULL) {
        secp256k1_ecmult_context_clear(&ctx->ecmult_ctx);
        secp256k1_ecmult_gen_context_clear(&ctx->ecmult_gen_ctx);

        free(ctx);
    }
}

void secp256k1_context_set_illegal_callback(secp256k1_context* ctx, void (*fun)(const char* message, void* data), const void* data) {
    if (fun == NULL) {
        fun = default_illegal_callback_fn;
    }
    ctx->illegal_callback.fn = fun;
    ctx->illegal_callback.data = data;
}

void secp256k1_context_set_error_callback(secp256k1_context* ctx, void (*fun)(const char* message, void* data), const void* data) {
    if (fun == NULL) {
        fun = default_error_callback_fn;
    }
    ctx->error_callback.fn = fun;
    ctx->error_callback.data = data;
}

int secp256k1_pubkey_load(const secp256k1_context* ctx, secp256k1_ge* ge, const secp256k1_pubkey* pubkey) {
    if (sizeof(secp256k1_ge_storage) == 64) {
        /* When the secp256k1_ge_storage type is exactly 64 byte, use its
         * representation inside secp256k1_pubkey, as conversion is very fast.
         * Note that secp256k1_pubkey_save must use the same representation. */
        secp256k1_ge_storage s;
        memcpy(&s, &pubkey->data[0], 64);
        secp256k1_ge_from_storage(ge, &s);
    } else {
        /* Otherwise, fall back to 32-byte big endian for X and Y. */
        secp256k1_fe x, y;
        secp256k1_fe_set_b32(&x, pubkey->data);
        secp256k1_fe_set_b32(&y, pubkey->data + 32);
        secp256k1_ge_set_xy(ge, &x, &y);
    }
    ARG_CHECK(!secp256k1_fe_is_zero(&ge->x));
    return 1;
}

void secp256k1_pubkey_save(secp256k1_pubkey* pubkey, secp256k1_ge* ge) {
    if (sizeof(secp256k1_ge_storage) == 64) {
        secp256k1_ge_storage s;
        secp256k1_ge_to_storage(&s, ge);
        memcpy(&pubkey->data[0], &s, 64);
    } else {
        VERIFY_CHECK(!secp256k1_ge_is_infinity(ge));
        secp256k1_fe_normalize_var(&ge->x);
        secp256k1_fe_normalize_var(&ge->y);
        secp256k1_fe_get_b32(pubkey->data, &ge->x);
        secp256k1_fe_get_b32(pubkey->data + 32, &ge->y);
    }
}

int secp256k1_ec_pubkey_parse(const secp256k1_context* ctx, secp256k1_pubkey* pubkey, const unsigned char *input, size_t inputlen) {
    secp256k1_ge Q;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(pubkey != NULL);
    memset(pubkey, 0, sizeof(*pubkey));
    ARG_CHECK(input != NULL);
    if (!secp256k1_eckey_pubkey_parse(&Q, input, inputlen)) {
        return 0;
    }
    secp256k1_pubkey_save(pubkey, &Q);
    secp256k1_ge_clear(&Q);
    return 1;
}

int secp256k1_ec_pubkey_serialize(const secp256k1_context* ctx, unsigned char *output, size_t *outputlen, const secp256k1_pubkey* pubkey, unsigned int flags) {
    secp256k1_ge Q;
    size_t len;
    int ret = 0;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(outputlen != NULL);
    ARG_CHECK(*outputlen >= ((flags & SECP256K1_FLAGS_BIT_COMPRESSION) ? 33 : 65));
    len = *outputlen;
    *outputlen = 0;
    ARG_CHECK(output != NULL);
    memset(output, 0, len);
    ARG_CHECK(pubkey != NULL);
    ARG_CHECK((flags & SECP256K1_FLAGS_TYPE_MASK) == SECP256K1_FLAGS_TYPE_COMPRESSION);
    if (secp256k1_pubkey_load(ctx, &Q, pubkey)) {
        ret = secp256k1_eckey_pubkey_serialize(&Q, output, &len, flags & SECP256K1_FLAGS_BIT_COMPRESSION);
        if (ret) {
            *outputlen = len;
        }
    }
    return ret;
}

void secp256k1_ecdsa_signature_load(const secp256k1_context* ctx, secp256k1_scalar* r, secp256k1_scalar* s, const secp256k1_ecdsa_signature* sig) {
    (void)ctx;
    if (sizeof(secp256k1_scalar) == 32) {
        /* When the secp256k1_scalar type is exactly 32 byte, use its
         * representation inside secp256k1_ecdsa_signature, as conversion is very fast.
         * Note that secp256k1_ecdsa_signature_save must use the same representation. */
        memcpy(r, &sig->data[0], 32);
        memcpy(s, &sig->data[32], 32);
    } else {
        secp256k1_scalar_set_b32(r, &sig->data[0], NULL);
        secp256k1_scalar_set_b32(s, &sig->data[32], NULL);
    }
}

void secp256k1_ecdsa_signature_save(secp256k1_ecdsa_signature* sig, const secp256k1_scalar* r, const secp256k1_scalar* s) {
    if (sizeof(secp256k1_scalar) == 32) {
        memcpy(&sig->data[0], r, 32);
        memcpy(&sig->data[32], s, 32);
    } else {
        secp256k1_scalar_get_b32(&sig->data[0], r);
        secp256k1_scalar_get_b32(&sig->data[32], s);
    }
}

int secp256k1_ecdsa_signature_parse_der(const secp256k1_context* ctx, secp256k1_ecdsa_signature* sig, const unsigned char *input, size_t inputlen) {
    secp256k1_scalar r, s;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(sig != NULL);
    ARG_CHECK(input != NULL);

    if (secp256k1_ecdsa_sig_parse(&r, &s, input, inputlen)) {
        secp256k1_ecdsa_signature_save(sig, &r, &s);
        return 1;
    } else {
        memset(sig, 0, sizeof(*sig));
        return 0;
    }
}

int secp256k1_ecdsa_signature_parse_compact(const secp256k1_context* ctx, secp256k1_ecdsa_signature* sig, const unsigned char *input64) {
    secp256k1_scalar r, s;
    int ret = 1;
    int overflow = 0;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(sig != NULL);
    ARG_CHECK(input64 != NULL);

    secp256k1_scalar_set_b32(&r, &input64[0], &overflow);
    ret &= !overflow;
    secp256k1_scalar_set_b32(&s, &input64[32], &overflow);
    ret &= !overflow;
    if (ret) {
        secp256k1_ecdsa_signature_save(sig, &r, &s);
    } else {
        memset(sig, 0, sizeof(*sig));
    }
    return ret;
}

int secp256k1_ecdsa_signature_serialize_der(const secp256k1_context* ctx, unsigned char *output, size_t *outputlen, const secp256k1_ecdsa_signature* sig) {
    secp256k1_scalar r, s;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(output != NULL);
    ARG_CHECK(outputlen != NULL);
    ARG_CHECK(sig != NULL);

    secp256k1_ecdsa_signature_load(ctx, &r, &s, sig);
    return secp256k1_ecdsa_sig_serialize(output, outputlen, &r, &s);
}

int secp256k1_ecdsa_signature_serialize_compact(const secp256k1_context* ctx, unsigned char *output64, const secp256k1_ecdsa_signature* sig) {
    secp256k1_scalar r, s;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(output64 != NULL);
    ARG_CHECK(sig != NULL);

    secp256k1_ecdsa_signature_load(ctx, &r, &s, sig);
    secp256k1_scalar_get_b32(&output64[0], &r);
    secp256k1_scalar_get_b32(&output64[32], &s);
    return 1;
}

int secp256k1_ecdsa_signature_normalize(const secp256k1_context* ctx, secp256k1_ecdsa_signature *sigout, const secp256k1_ecdsa_signature *sigin) {
    secp256k1_scalar r, s;
    int ret = 0;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(sigin != NULL);

    secp256k1_ecdsa_signature_load(ctx, &r, &s, sigin);
    ret = secp256k1_scalar_is_high(&s);
    if (sigout != NULL) {
        if (ret) {
            secp256k1_scalar_negate(&s, &s);
        }
        secp256k1_ecdsa_signature_save(sigout, &r, &s);
    }

    return ret;
}

int secp256k1_ecdsa_verify(const secp256k1_context* ctx, const secp256k1_ecdsa_signature *sig, const unsigned char *msg32, const secp256k1_pubkey *pubkey) {
    secp256k1_ge q;
    secp256k1_scalar r, s;
    secp256k1_scalar m;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(secp256k1_ecmult_context_is_built(&ctx->ecmult_ctx));
    ARG_CHECK(msg32 != NULL);
    ARG_CHECK(sig != NULL);
    ARG_CHECK(pubkey != NULL);

    secp256k1_scalar_set_b32(&m, msg32, NULL);
    secp256k1_ecdsa_signature_load(ctx, &r, &s, sig);
    return (!secp256k1_scalar_is_high(&s) &&
            secp256k1_pubkey_load(ctx, &q, pubkey) &&
            secp256k1_ecdsa_sig_verify(&ctx->ecmult_ctx, &r, &s, &q, &m));
}

int nonce_function_rfc6979(unsigned char *nonce32, const unsigned char *msg32, const unsigned char *key32, const unsigned char *algo16, void *data, unsigned int counter) {
   unsigned char keydata[112];
   int keylen = 64;
   secp256k1_rfc6979_hmac_sha256_t rng;
   unsigned int i;
   /* We feed a byte array to the PRNG as input, consisting of:
    * - the private key (32 bytes) and message (32 bytes), see RFC 6979 3.2d.
    * - optionally 32 extra bytes of data, see RFC 6979 3.6 Additional Data.
    * - optionally 16 extra bytes with the algorithm name.
    * Because the arguments have distinct fixed lengths it is not possible for
    *  different argument mixtures to emulate each other and result in the same
    *  nonces.
    */
   memcpy(keydata, key32, 32);
   memcpy(keydata + 32, msg32, 32);
   if (data != NULL) {
       memcpy(keydata + 64, data, 32);
       keylen = 96;
   }
   if (algo16 != NULL) {
       memcpy(keydata + keylen, algo16, 16);
       keylen += 16;
   }
   secp256k1_rfc6979_hmac_sha256_initialize(&rng, keydata, keylen);
   memset(keydata, 0, sizeof(keydata));
   for (i = 0; i <= counter; i++) {
       secp256k1_rfc6979_hmac_sha256_generate(&rng, nonce32, 32);
   }
   secp256k1_rfc6979_hmac_sha256_finalize(&rng);
   return 1;
}

int secp256k1_ecdsa_sign(const secp256k1_context* ctx, secp256k1_ecdsa_signature *signature, const unsigned char *msg32, const unsigned char *seckey, secp256k1_nonce_function noncefp, const void* noncedata) {
    secp256k1_scalar r, s;
    secp256k1_scalar sec, non, msg;
    int ret = 0;
    int overflow = 0;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(secp256k1_ecmult_gen_context_is_built(&ctx->ecmult_gen_ctx));
    ARG_CHECK(msg32 != NULL);
    ARG_CHECK(signature != NULL);
    ARG_CHECK(seckey != NULL);
    if (noncefp == NULL) {
        noncefp = secp256k1_nonce_function_default;
    }

    secp256k1_scalar_set_b32(&sec, seckey, &overflow);
    /* Fail if the secret key is invalid. */
    if (!overflow && !secp256k1_scalar_is_zero(&sec)) {
        unsigned char nonce32[32];
        unsigned int count = 0;
        secp256k1_scalar_set_b32(&msg, msg32, NULL);
        while (1) {
            ret = noncefp(nonce32, msg32, seckey, NULL, (void*)noncedata, count);
            if (!ret) {
                break;
            }
            secp256k1_scalar_set_b32(&non, nonce32, &overflow);
            if (!overflow && !secp256k1_scalar_is_zero(&non)) {
                if (secp256k1_ecdsa_sig_sign(&ctx->ecmult_gen_ctx, &r, &s, &sec, &msg, &non, NULL)) {
                    break;
                }
            }
            count++;
        }
        memset(nonce32, 0, 32);
        secp256k1_scalar_clear(&msg);
        secp256k1_scalar_clear(&non);
        secp256k1_scalar_clear(&sec);
    }
    if (ret) {
        secp256k1_ecdsa_signature_save(signature, &r, &s);
    } else {
        memset(signature, 0, sizeof(*signature));
    }
    return ret;
}

int secp256k1_ec_seckey_verify(const secp256k1_context* ctx, const unsigned char *seckey) {
    secp256k1_scalar sec;
    int ret;
    int overflow;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(seckey != NULL);

    secp256k1_scalar_set_b32(&sec, seckey, &overflow);
    ret = !overflow && !secp256k1_scalar_is_zero(&sec);
    secp256k1_scalar_clear(&sec);
    return ret;
}

int secp256k1_ec_pubkey_create(const secp256k1_context* ctx, secp256k1_pubkey *pubkey, const unsigned char *seckey) {
    secp256k1_gej pj;
    secp256k1_ge p;
    secp256k1_scalar sec;
    int overflow;
    int ret = 0;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(pubkey != NULL);
    memset(pubkey, 0, sizeof(*pubkey));
    ARG_CHECK(secp256k1_ecmult_gen_context_is_built(&ctx->ecmult_gen_ctx));
    ARG_CHECK(seckey != NULL);

    secp256k1_scalar_set_b32(&sec, seckey, &overflow);
    ret = (!overflow) & (!secp256k1_scalar_is_zero(&sec));
    if (ret) {
        secp256k1_ecmult_gen(&ctx->ecmult_gen_ctx, &pj, &sec);
        secp256k1_ge_set_gej(&p, &pj);
        secp256k1_pubkey_save(pubkey, &p);
    }
    secp256k1_scalar_clear(&sec);
    return ret;
}

int secp256k1_ec_privkey_negate(const secp256k1_context* ctx, unsigned char *seckey) {
    secp256k1_scalar sec;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(seckey != NULL);

    secp256k1_scalar_set_b32(&sec, seckey, NULL);
    secp256k1_scalar_negate(&sec, &sec);
    secp256k1_scalar_get_b32(seckey, &sec);

    return 1;
}

int secp256k1_ec_pubkey_negate(const secp256k1_context* ctx, secp256k1_pubkey *pubkey) {
    int ret = 0;
    secp256k1_ge p;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(pubkey != NULL);

    ret = secp256k1_pubkey_load(ctx, &p, pubkey);
    memset(pubkey, 0, sizeof(*pubkey));
    if (ret) {
        secp256k1_ge_neg(&p, &p);
        secp256k1_pubkey_save(pubkey, &p);
    }
    return ret;
}

int secp256k1_ec_privkey_tweak_add(const secp256k1_context* ctx, unsigned char *seckey, const unsigned char *tweak) {
    secp256k1_scalar term;
    secp256k1_scalar sec;
    int ret = 0;
    int overflow = 0;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(seckey != NULL);
    ARG_CHECK(tweak != NULL);

    secp256k1_scalar_set_b32(&term, tweak, &overflow);
    secp256k1_scalar_set_b32(&sec, seckey, NULL);

    ret = !overflow && secp256k1_eckey_privkey_tweak_add(&sec, &term);
    memset(seckey, 0, 32);
    if (ret) {
        secp256k1_scalar_get_b32(seckey, &sec);
    }

    secp256k1_scalar_clear(&sec);
    secp256k1_scalar_clear(&term);
    return ret;
}

int secp256k1_ec_pubkey_tweak_add(const secp256k1_context* ctx, secp256k1_pubkey *pubkey, const unsigned char *tweak) {
    secp256k1_ge p;
    secp256k1_scalar term;
    int ret = 0;
    int overflow = 0;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(secp256k1_ecmult_context_is_built(&ctx->ecmult_ctx));
    ARG_CHECK(pubkey != NULL);
    ARG_CHECK(tweak != NULL);

    secp256k1_scalar_set_b32(&term, tweak, &overflow);
    ret = !overflow && secp256k1_pubkey_load(ctx, &p, pubkey);
    memset(pubkey, 0, sizeof(*pubkey));
    if (ret) {
        if (secp256k1_eckey_pubkey_tweak_add(&ctx->ecmult_ctx, &p, &term)) {
            secp256k1_pubkey_save(pubkey, &p);
        } else {
            ret = 0;
        }
    }

    return ret;
}

int secp256k1_ec_privkey_tweak_mul(const secp256k1_context* ctx, unsigned char *seckey, const unsigned char *tweak) {
    secp256k1_scalar factor;
    secp256k1_scalar sec;
    int ret = 0;
    int overflow = 0;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(seckey != NULL);
    ARG_CHECK(tweak != NULL);

    secp256k1_scalar_set_b32(&factor, tweak, &overflow);
    secp256k1_scalar_set_b32(&sec, seckey, NULL);
    ret = !overflow && secp256k1_eckey_privkey_tweak_mul(&sec, &factor);
    memset(seckey, 0, 32);
    if (ret) {
        secp256k1_scalar_get_b32(seckey, &sec);
    }

    secp256k1_scalar_clear(&sec);
    secp256k1_scalar_clear(&factor);
    return ret;
}

int secp256k1_ec_pubkey_tweak_mul(const secp256k1_context* ctx, secp256k1_pubkey *pubkey, const unsigned char *tweak) {
    secp256k1_ge p;
    secp256k1_scalar factor;
    int ret = 0;
    int overflow = 0;
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(secp256k1_ecmult_context_is_built(&ctx->ecmult_ctx));
    ARG_CHECK(pubkey != NULL);
    ARG_CHECK(tweak != NULL);

    secp256k1_scalar_set_b32(&factor, tweak, &overflow);
    ret = !overflow && secp256k1_pubkey_load(ctx, &p, pubkey);
    memset(pubkey, 0, sizeof(*pubkey));
    if (ret) {
        if (secp256k1_eckey_pubkey_tweak_mul(&ctx->ecmult_ctx, &p, &factor)) {
            secp256k1_pubkey_save(pubkey, &p);
        } else {
            ret = 0;
        }
    }

    return ret;
}

int secp256k1_context_randomize(secp256k1_context* ctx, const unsigned char *seed32) {
    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(secp256k1_ecmult_gen_context_is_built(&ctx->ecmult_gen_ctx));
    secp256k1_ecmult_gen_blind(&ctx->ecmult_gen_ctx, seed32);
    return 1;
}

int secp256k1_ec_pubkey_combine(const secp256k1_context* ctx, secp256k1_pubkey *pubnonce, const secp256k1_pubkey * const *pubnonces, size_t n) {
    size_t i;
    secp256k1_gej Qj;
    secp256k1_ge Q;

    ARG_CHECK(pubnonce != NULL);
    memset(pubnonce, 0, sizeof(*pubnonce));
    ARG_CHECK(n >= 1);
    ARG_CHECK(pubnonces != NULL);

    secp256k1_gej_set_infinity(&Qj);

    for (i = 0; i < n; i++) {
        secp256k1_pubkey_load(ctx, &Q, pubnonces[i]);
        secp256k1_gej_add_ge(&Qj, &Qj, &Q);
    }
    if (secp256k1_gej_is_infinity(&Qj)) {
        return 0;
    }
    secp256k1_ge_set_gej(&Q, &Qj);
    secp256k1_pubkey_save(pubnonce, &Q);
    return 1;
}
///End of secp256k1.c
