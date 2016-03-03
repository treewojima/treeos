#ifndef __TREEOS_KERNEL_DEFINES_H__
#define __TREEOS_KERNEL_DEFINES_H__

// Awful hack to get Qt Creator's code model to handle certain global defines
#ifndef TREEOS_CMDLINE_DEFINES
#   define TREEOS_I386
#   define TREEOS_DEBUG
#endif

#define TREEOS_GLOBAL_MULTIBOOT_INFO

#ifdef TREEOS_EXPORT_ASM
#   define ASM_FILE // for multiboot.h
#else
#   ifndef NULL
#      define NULL ((void *)0)
#   endif // defined(NULL)
//#   define CHECK_FORMAT_STRING
#   ifdef TREEOS_DEBUG
//#      define DEBUG_FORCE_INLINE
#   endif // defined(TREEOS_DEBUG)
#   ifdef __GNUC__
#      define ALIGNED(n) __attribute__((aligned((n))))
#      define PACKED     __attribute__((packed))
#      define NORETURN   __attribute__((noreturn))
#      define UNUSED     __attribute__((unused))
#      ifdef CHECK_FORMAT_STRING
#         define FORMAT(str, vararg) \
              __attribute__((format(printf, (str), (vararg))))
#      else
#         define FORMAT(str, vararg)
#      endif
#      ifdef DEBUG_FORCE_INLINE
#         define FORCE_INLINE __attribute__((always_inline))
#      else
#         define FORCE_INLINE
#      endif
#   else
#      error "compilers other than GCC are not supported"
#   endif // defined(__GNUC__)
#endif // defined(TREEOS_EXPORT_ASM)

#endif
