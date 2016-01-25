#ifndef __TREEOS_KERNEL_DEFINES_H__
#define __TREEOS_KERNEL_DEFINES_H__

// Awful hack to get Qt Creator's code model to handle certain defines
#ifndef NOT_IN_QT_CREATOR
#   define TREEOS_I386
#endif

#ifndef TREEOS_EXPORT_ASM

#ifndef NULL
#   define NULL ((void *)0)
#endif

//#define CHECK_FORMAT_STRING

#ifdef __GNUC__
#   define ALIGNED(n) __attribute__((aligned((n))))
#   define PACKED     __attribute__((packed))
#   define NORETURN   __attribute__((noreturn))
#   ifdef CHECK_FORMAT_STRING
#      define FORMAT(str, vararg) \
           __attribute__((format(printf, (str), (vararg))))
#   else
#      define FORMAT(str, vararg)
#   endif
#else
#   error "compilers other than GCC are not supported"
#endif

//#define KBD_DEBUG_KEYPRESSES

#endif

#endif
