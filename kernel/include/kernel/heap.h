#ifndef __TREEOS_KERNEL_HEAP_H__
#define __TREEOS_KERNEL_HEAP_H__

#include <stddef.h>
#include <stdint.h>

#define KHEAP_START        0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000

struct multiboot_info;

void kheap_init(struct multiboot_info *boot_record);

#endif
