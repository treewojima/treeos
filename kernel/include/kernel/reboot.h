#ifndef __TREEOS_KERNEL_REBOOT_H__
#define __TREEOS_KERNEL_REBOOT_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

void reboot(void) NORETURN;
void halt(void) NORETURN;

#endif
