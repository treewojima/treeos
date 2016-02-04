#ifndef __TREEOS_KERNEL_UTIL_BITMAP_H__
#define __TREEOS_KERNEL_UTIL_BITMAP_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

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
struct bitmap *bitmap_alloc(struct bitmap *bitmap, unsigned num_bits);

#ifdef PLACEHOLDER_FOR_WORKING_MALLOC
void bitmap_free(struct bitmap *bitmap);
#else
#   define bitmap_free(bitmap)
#endif

#define bitmap_set_bit(bitmap, bit)   ((bitmap)->bits[(bit) / BITS_PER_WORD] |= (1 << ((bit) % BITS_PER_WORD)))
#define bitmap_clear_bit(bitmap, bit) ((bitmap)->bits[(bit) / BITS_PER_WORD] &= ~(1 << ((bit) % BITS_PER_WORD)))
#define bitmap_test_bit(bitmap, bit)  (!!((bitmap)->bits[(bit) / BITS_PER_WORD] & (1 << ((bit) % BITS_PER_WORD))))

// Finds the first unset number of consecutive bits
//unsigned bitmap_first_free_block(struct bitmap *bitmap, unsigned size);
//#define bitmap_first_free(bitmap) bitmap_first_free_block((bitmap), 1)
unsigned bitmap_first_free(struct bitmap *bitmap);

#endif
