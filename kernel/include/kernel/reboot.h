#ifndef __TREEOS_KERNEL_REBOOT_H__
#define __TREEOS_KERNEL_REBOOT_H__

#include "defines.h"

void reboot(void) __attribute__((noreturn));
void halt(void) __attribute__((noreturn));

#endif
