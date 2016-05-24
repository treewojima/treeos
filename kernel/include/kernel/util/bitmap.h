/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TREEOS_KERNEL_UTIL_BITMAP_H__
#define __TREEOS_KERNEL_UTIL_BITMAP_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <kernel/const.h>

// work in progress
//#define PLACEHOLDER_FOR_WORKING_MALLOC

#define BITMAP_FREE_STRUCT  (1 << 0)
#define BITMAP_FREE_NOTHING (1 << 1)

// Bitmap structure that spans multiple words, if necessary
struct bitmap
{
    unsigned word_count;
    unsigned *bits;
    unsigned char flags;
};

// Allocate a bitmap large enough to hold num_bits
struct bitmap *bitmap_alloc(struct bitmap *bitmap, unsigned num_bits);

void bitmap_free(struct bitmap *bitmap);

#define bitmap_set_bit(bitmap, bit)   ((bitmap)->bits[(bit) / BITS_PER_WORD] |= (1 << ((bit) % BITS_PER_WORD)))
#define bitmap_clear_bit(bitmap, bit) ((bitmap)->bits[(bit) / BITS_PER_WORD] &= ~(1 << ((bit) % BITS_PER_WORD)))
#define bitmap_test_bit(bitmap, bit)  (!!((bitmap)->bits[(bit) / BITS_PER_WORD] & (1 << ((bit) % BITS_PER_WORD))))

// Finds the first unset number of consecutive bits
//unsigned bitmap_first_free_block(struct bitmap *bitmap, unsigned size);
//#define bitmap_first_free(bitmap) bitmap_first_free_block((bitmap), 1)
unsigned bitmap_first_free(struct bitmap *bitmap);

#endif
