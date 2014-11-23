#ifndef __TREEOS_KERNEL_PANIC_H__
#define __TREEOS_KERNEL_PANIC_H__

#include "defines.h"

#define MAX_PANIC_BUF 512

struct registers;

#define panic(msg)                                           \
    do                                                       \
    {                                                        \
        extern void __panic(const char *, int, const char *) \
            __attribute__((noreturn));                       \
        __panic(__FILE__, __LINE__, (msg));                  \
    } while (0)

#define worry(msg)                                            \
    do                                                        \
    {                                                         \
        extern void __worry(const char *, int, const char *); \
        __worry(__FILE__, __LINE__, (msg));                   \
    } while (0)

#define panic_r(msg, reg)                                      \
    do                                                         \
    {                                                          \
        extern void __panic_r(const char *, int, const char *, \
                              const struct registers *const)   \
            __attribute__((noreturn));                         \
        __panic_r(__FILE__, __LINE__, (msg), (reg));           \
    } while (0)

#define PANIC_IF(cond, msg) \
        do                  \
        {                   \
            if (cond)       \
            {               \
                panic(msg); \
            }               \
        } while (0)

#define WORRY_IF(cond, msg) \
        do                  \
        {                   \
            if (cond)       \
            {               \
                worry(msg); \
            }               \
        } while (0)

#endif
