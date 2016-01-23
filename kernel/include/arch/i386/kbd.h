#ifndef __TREEOS_KERNEL_ARCH_KBD_H__
#define __TREEOS_KERNEL_ARCH_KBD_H__

#include "defines.h"
#ifndef TREEOS_I386
#   error included i386-specific header (kbd.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Map of scancodes to keycodes
extern const char scancode_to_ascii[];

// Read the last detected scan code
uint8_t kbd_read_scancode(void);

#endif
