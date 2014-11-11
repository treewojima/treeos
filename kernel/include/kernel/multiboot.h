#ifndef __TREEOS_KERNEL_MULTIBOOT_H__
#define __TREEOS_KERNEL_MULTIBOOT_H__

#include <stdint.h>

#define MB_BOOT_MAGIC  0x2BADB002
#define MB_FLAG_MOD    (1 << 2)

struct multiboot_aout
{
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr, :32;
};

struct multiboot_elf
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
};

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
};

struct multiboot_module
{
    // module goes from mod_start to mod_end - 1
    uint32_t mod_start;
    uint32_t mod_end;
    char string[8];//, :32;
};

#define MB_GET_MODULES(info) ((struct multiboot_module *)info->mods_addr)

#endif
