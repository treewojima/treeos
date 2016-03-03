#ifndef __TREEOS_KERNEL_MODULE_H__
#define __TREEOS_KERNEL_MODULE_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

void module_table_init(void);
void *get_module_by_name(const char *name);

#endif
