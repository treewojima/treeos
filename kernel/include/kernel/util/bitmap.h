#ifndef __TREEOS_KERNEL_UTIL_BITMAP_H__
#define __TREEOS_KERNEL_UTIL_BITMAP_H__

#include <defines.h>
#include <kernel/const.h>

// work in progress
//#define PLACEHOLDER_FOR_WORKING_MALLOC

// Bitmap structure that spans multiple words, if necessary
struct bitmap
{
    unsigned word_count;
    unsigned *bits;
    unsigned char flags;
};

// Allocate a bitmap large enough to hold num_bits
struct bitmap *bitmap_alloc(unsigned num_bits);

#ifdef PLACEHOLDER_FOR_WORKING_MALLOC
void bitmap_free(struct bitmap *bitmap);
#else
#   define bitmap_free(bitmap)
#endif

#define bitmap_set_bit(bitmap, bit)   ((bitmap)->bits[(bit) / BITS_PER_WORD] |= (1 << ((bit) % BITS_PER_WORD)))
#define bitmap_clear_bit(bitmap, bit) ((bitmap)->bits[(bit) / BITS_PER_WORD] &= ~(1 << ((bit) % BITS_PER_WORD)))
#define bitmap_test_bit(bitmap, bit)  (!!((bitmap)->bits[(bit) / BITS_PER_WORD] & (1 << ((bit) % BITS_PER_WORD))))

// Finds the first unset bit
unsigned bitmap_first_free(struct bitmap *bitmap);

#endif
