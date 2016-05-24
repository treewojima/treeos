/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TREEOS_KERNEL_PANIC_H__
#define __TREEOS_KERNEL_PANIC_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#define MAX_PANIC_BUF 512

#define panic(msg)                                           \
    do                                                       \
    {                                                        \
        extern void _panic(const char *, int, const char *)  \
                           NORETURN;                         \
        _panic(__FILE__, __LINE__, (msg));                   \
    } while (0)

#define worry(msg)                                            \
    do                                                        \
    {                                                         \
        extern void _worry(const char *, int, const char *);  \
        _worry(__FILE__, __LINE__, (msg));                    \
    } while (0)

#define panic_r(msg, context)                                    \
    do                                                           \
    {                                                            \
        struct thread_context;                                   \
        extern void _panic_r(const char *, int, const char *,    \
                             const struct thread_context *const) \
                             NORETURN;                           \
        _panic_r(__FILE__, __LINE__, (msg),                      \
                 (const struct thread_context *const)(context)); \
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
