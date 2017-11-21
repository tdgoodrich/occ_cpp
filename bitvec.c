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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitvec.h"

struct bitvec* bitvec_make(size_t num_bits)
{
    struct bitvec *v = calloc(sizeof (struct bitvec) + bitvec_bytes(num_bits), 1);
    //struct bitvec *v = (struct bitvec *) malloc(sizeof (struct bitvec) + bitvec_bytes(num_bits));
    //memset(v, 1, sizeof (struct bitvec) + bitvec_bytes(num_bits));

    v->num_bits = num_bits;
    return v;
}

void bitvec_copy(struct bitvec *d, const struct bitvec *s)
{
    memcpy(d, s, sizeof (struct bitvec) + bitvec_bytes(s->num_bits));
}

struct bitvec *bitvec_clone(const struct bitvec *s)
{
    struct bitvec *d = bitvec_make(bitvec_size(s));
    bitvec_copy(d, s);
    return d;
}

size_t bitvec_find(const struct bitvec *v, size_t n)
{
    if (n >= v->num_bits)
    {
        return BITVEC_NOT_FOUND;
    }

    unsigned long w = n / BITS_PER_WORD, words = bitvec_words(v->num_bits);
    unsigned long word = v->data[w];
    word &= ~0UL << (n % BITS_PER_WORD);

    while (true)
    {
        if (word)
        {
            n = w * BITS_PER_WORD + ctzl(word);
            if (n >= v->num_bits)
            {
                break;
            }
            else
            {
                return n;
            }
        }

        if (++w >= words)
        {
            break;
        }
        word = v->data[w];
    }

    return BITVEC_NOT_FOUND;
}

size_t bitvec_count(const struct bitvec *v)
{
    size_t count = 0;
    for (size_t w = 0; w < bitvec_words(v->num_bits); ++w)
    {
        count += popcountl(v->data[w]);
    }

    return count;
}

static inline size_t min(size_t x, size_t y)
{
    return x < y ? x : y;
}

void bitvec_setminus(struct bitvec *d, const struct bitvec *s)
{
    size_t words = min(bitvec_words(s->num_bits), bitvec_words(d->num_bits));
    for (size_t w = 0; w < words; ++w)
    {
	    d->data[w] &= ~s->data[w];
    }
}

void bitvec_join(struct bitvec *d, const struct bitvec *s)
{
    size_t words = min(bitvec_words(s->num_bits), bitvec_words(d->num_bits));
    for (size_t w = 0; w < words; ++w)
    {
	    d->data[w] |= s->data[w];
    }
}

void bitvec_output(const struct bitvec *v, FILE *stream)
{
    fprintf(stream, "[%lu/%lu:", (unsigned long) bitvec_count(v), v->num_bits);
    for (size_t i = bitvec_find(v, 0); i != BITVEC_NOT_FOUND; i = bitvec_find(v, i + 1))
    {
	    fprintf(stream, " %lu", (unsigned long) i);
    }
    fprintf(stream, "]");
}

void bitvec_clear(struct bitvec *v)
{
    memset(v->data, 0, bitvec_bytes(v->num_bits));
}

void bitvec_fill(struct bitvec *v)
{
    memset(v->data, (unsigned char) -1, bitvec_bytes(v->num_bits));
    size_t words = bitvec_words(v->num_bits);
    unsigned long padbits = BITS_PER_WORD * words - v->num_bits;
    if (padbits)
    {
	    v->data[words - 1] &= ~0UL >> padbits;
    }
}

void bitvec_invert(struct bitvec *v)
{
    size_t words = bitvec_words(v->num_bits);

    for (size_t w = 0; w < words; ++w) {
	    v->data[w] = ~v->data[w];
    }

    unsigned long padbits = BITS_PER_WORD * words - v->num_bits;
    if (padbits)
    {
	    v->data[words - 1] &= ~0UL >> padbits;
    }
}
