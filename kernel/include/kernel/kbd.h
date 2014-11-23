#ifndef __TREEOS_KERNEL_KBD_H__
#define __TREEOS_KERNEL_KBD_H__

#include "defines.h"

#include <stdint.h>

// Initialize the keyboard driver
void kbd_init(void);

#ifdef __TREEOS_I386
#   include <arch/i386/kbd.h>
#endif

#endif
