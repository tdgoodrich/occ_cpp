/* occ -- find minimum odd cycle covers (Graph Bipartization)
   Copyright (C) 2006 Falk Hueffner

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef BIT_TWIDDLING_H
#define BIT_TWIDDLING_H

#include <limits.h>

#if   LONG_MAX == 2147483647
# define LONG_BITS 32
#elif LONG_MAX == 9223372036854775807
# define LONG_BITS 64
#else
# error "weird long size"
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
# define HAVE_BUILTIN_BITOPS
#endif

#ifdef HAVE_BUILTIN_BITOPS
# define ctzl(x) __builtin_ctzl(x)
#elif defined(__i386__)
static inline unsigned long ctzl(unsigned long x) {
    unsigned count;
    __asm__ ("bsfl %1,%0" : "=r" (count) : "rm" (x));
    return count;
}
#else
static inline unsigned ctzl(unsigned long x) {
   unsigned n = 1;
#if LONG_BITS == 64
   if ((x & 0xffffffff) == 0) n += 32, x >>= 32;
#endif
   if ((x & 0x0000ffff) == 0) n += 16, x >>= 16;
   if ((x & 0x000000ff) == 0) n +=  8, x >>=  8;
   if ((x & 0x0000000f) == 0) n +=  4, x >>=  4;
   if ((x & 0x00000003) == 0) n +=  2, x >>=  2;
   return n - (x & 1);
}
#endif

#ifdef HAVE_BUILTIN_BITOPS
# define popcountl(x) __builtin_popcountl(x)
#else
static inline unsigned popcountl(unsigned long x) {
    static const unsigned char table[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
    };

    return table[ x        & 0xff]
	     + table[(x >>  8) & 0xff]
	     + table[(x >> 16) & 0xff]
	     + table[(x >> 24) & 0xff]
#if LONG_BITS == 64
	 + table[(x >> 32) & 0xff]
	 + table[(x >> 40) & 0xff]
	 + table[(x >> 48) & 0xff]
	 + table[(x >> 56) & 0xff]
#endif
	;
}

#endif // HAVE_BUILTIN_BITOPS

#endif // BIT_TWIDDLING_H
