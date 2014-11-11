#ifndef __TREEOS_KERNEL_TIMER_H__
#define __TREEOS_KERNEL_TIMER_H__

#include <stdint.h>

#define DEFAULT_SYSTEM_TIMER_FREQ 100

// Initialize system timer
void timer_init(uint32_t frequency);

#endif
