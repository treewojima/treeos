#ifndef __TREEOS_KERNEL_ARCH_SERIO_H__
#define __TREEOS_KERNEL_ARCH_SERIO_H__

#include "defines.h"
#ifndef __TREEOS_I386
#   error included i386-specific header (serio.h) in non-i386 build
#endif

#define SERIO_COM1 0x3F8
#define SERIO_COM2 0x2F8
#define SERIO_COM3 0x3E8
#define SERIO_COM4 0x2E8

#ifndef __TREEOS_EXPORT_ASM

#include <stdbool.h>

void serio_init(int com);
bool serio_data_incoming(int com);
char serio_readchar(int com);
bool serio_data_outgoing(int com);
void serio_writechar(int com, char c);

int serio_puts(int com, const char *str, bool newline);

#endif

#endif
