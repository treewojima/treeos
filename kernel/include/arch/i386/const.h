// Architecture-specific common constants

#ifndef __TREEOS_KERNEL_ARCH_I386_CONST_H__
#define __TREEOS_KERNEL_ARCH_I386_CONST_H__

#include <defines.h>
#ifndef TREEOS_I386
#   error included i386-specific header (cpu.h) in non-i386 build
#endif

// Only because <limits.h> is included
#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <limits.h>

// Used primarily for extending bitmaps across multiple words
#define BITS_PER_WORD (CHAR_BIT * sizeof(int))

// Segment selectors
#define KERN_CODE_SELECTOR 0x08
#define KERN_DATA_SELECTOR 0x10
#define USER_CODE_SELECTOR 0x18
#define USER_DATA_SELECTOR 0x20

#endif
