#ifndef __TREEOS_KERNEL_ARCH_IOPORT_H__
#define __TREEOS_KERNEL_ARCH_IOPORT_H__

#include "defines.h"
#ifndef __TREEOS_I386
#   error included i386-specific header (ioport.h) in non-i386 build
#endif

#ifdef __TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

uint8_t ioport_inb(uint16_t port);
uint16_t ioport_inw(uint16_t port);
uint32_t ioport_inl(uint16_t port);

void ioport_outb(uint16_t port, uint8_t value);
void ioport_outw(uint16_t port, uint16_t value);
void ioport_outl(uint16_t port, uint32_t value);

void ioport_delay(void);

#endif
