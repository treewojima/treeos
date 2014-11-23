#ifndef __TREEOS_KERNEL_DEBUG_H__
#define __TREEOS_KERNEL_DEBUG_H__

#include "defines.h"

#include <kernel/panic.h>

#define KASSERT(cond) \
    PANIC_IF(!(cond), "KASSERT failed: " # cond)

#define KASSERT_MSG(cond, msg) \
    PANIC_IF(!(cond), "KASSERT failed: " # msg)

#define KASSERT_WORRY(cond) \
    WORRY_IF(!(cond), "KASSERT failed: " # cond)

#define KASSERT_WORRY_MSG(cond, msg) \
    WORRY_IF(!(cond), "KASSERT failed: " # msg)

#endif
