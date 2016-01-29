#ifndef __TREEOS_KERNEL_ARCH_IOPORT_H__
#define __TREEOS_KERNEL_ARCH_IOPORT_H__

#include <defines.h>
#include <stdint.h>

#ifndef TREEOS_I386
#   error included i386-specific header (ioport.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#define USE_ASM_IOPORT

#ifdef USE_ASM_IOPORT
#   define ioport_inb  asm_ioport_inb
#   define ioport_outb asm_ioport_outb
#   define ioport_inw  asm_ioport_inw
#   define ioport_outw asm_ioport_outw
#   define ioport_inl  asm_ioport_inl
#   define ioport_outl asm_ioport_outl
#else
#   define ioport_inb  c_ioport_inb
#   define ioport_outb c_ioport_outb
#   define ioport_inw  c_ioport_inw
#   define ioport_outw c_ioport_outw
#   define ioport_inl  c_ioport_inl
#   define ioport_outl c_ioport_outl
#endif

uint8_t ioport_inb(uint16_t port);
void ioport_outb(uint16_t port, uint8_t value);

uint16_t ioport_inw(uint16_t port);
void ioport_outw(uint16_t port, uint16_t value);

uint32_t ioport_inl(uint16_t port);
void ioport_outl(uint16_t port, uint32_t value);

#endif
