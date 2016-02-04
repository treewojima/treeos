#ifndef __TREEOS_KERNEL_KBD_H__
#define __TREEOS_KERNEL_KBD_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Initialize the keyboard driver
void kbd_init(void);

#ifdef TREEOS_I386
#   include <arch/i386/kbd.h>
#endif

#endif
