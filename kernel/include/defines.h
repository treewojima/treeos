#ifndef __TREEOS_KERNEL_DEFINES_H__
#define __TREEOS_KERNEL_DEFINES_H__

// Awful hack to get Qt Creator's code model to handle certain defines
#ifndef NOT_IN_QT_CREATOR
#   define __TREEOS_I386
#endif

#ifndef NULL
#   define NULL ((void *)0)
#endif

//#define KBD_DEBUG_KEYPRESSES

#endif
