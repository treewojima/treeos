#ifndef __TREEOS_KERNEL_MULTIBOOT_H__
#define __TREEOS_KERNEL_MULTIBOOT_H__

#include "defines.h"

#include <stdint.h>

#define MB_BOOT_MAGIC  0x2BADB002
#define MB_FLAG_MEM    (1 << 0)
#define MB_FLAG_MOD    (1 << 3)
#define MB_FLAG_MMAP   (1 << 6)

struct multiboot_aout
{
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr, :32;
} __attribute__((packed));

struct multiboot_elf
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} __attribute__((packed));

struct multiboot_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    union
    {
        struct multiboot_aout aout;
        struct multiboot_elf elf;
    } syms;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
} __attribute__((packed));

struct multiboot_module
{
    // module goes from mod_start to mod_end - 1
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t string, :32;
} __attribute__((packed));

struct multiboot_memory_map
{
    uint32_t size;
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} __attribute__((packed));

//#define MB_GET_MODULES(info) ((struct multiboot_module *)info->mods_addr)

#endif
