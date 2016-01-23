#ifndef __TREEOS_KERNEL_TIMER_H__
#define __TREEOS_KERNEL_TIMER_H__

#include "defines.h"

#include <stdint.h>

#define DEFAULT_SYSTEM_TIMER_FREQ 100

// Initialize system timer
void timer_init(uint32_t frequency);

// Get system tick count
uint32_t timer_getticks(void);

// Sleep for specified number of ticks
void timer_sleep(uint32_t ticks);

#endif
