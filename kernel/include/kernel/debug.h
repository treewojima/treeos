#ifndef __TREEOS_KERNEL_DEBUG_H__
#define __TREEOS_KERNEL_DEBUG_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifndef TREEOS_DEBUG
#   define KASSERT(cond)
#   define KASSERT_MSG(cond, msg)
#   define KASSERT_WORRY(cond)
#   define KASSERT_WORRY_MSG(cond, msg)
#else

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

#endif
