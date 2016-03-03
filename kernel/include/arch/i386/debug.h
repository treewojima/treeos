#ifndef __TREEOS_ARCH_I386_DEBUG_H__
#define __TREEOS_ARCH_I386_DEBUG_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (arch/i386/debug.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifndef TREEOS_DEBUG
#   define KASSERT(cond)
#   define KASSERT_MSG(cond, msg)
#   define KASSERT_WORRY(cond)
#   define KASSERT_WORRY_MSG(cond, msg)
#else

#include <arch/i386/cpu.h>
#include <stdio.h>

#define dbg_print_esp() printf("esp = %p\n", read_esp())

#endif

#endif
