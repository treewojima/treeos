#include <kernel/mm.h>

#include <kernel/debug.h>
#include <kernel/multiboot.h>
#include <stddef.h>

uint32_t g_placement_address;

void mm_init_placement_heap(void)
{
    // Start off with the kernel heap at the end of the kernel image
    extern const uint8_t g_kernel_end;
    g_placement_address = (uint32_t)&g_kernel_end;

    // If there are multiboot modules present, take them into account. Otherwise
    // they'll be counted as free heap space, which would lead to... undefined
    // behavior.
    // TODO: Add module support
    extern const struct multiboot_info g_multiboot_info;
    KASSERT(g_multiboot_info.mods_count == 0);
}

void *mm_placement_alloc(uint32_t size, bool align)
{
    // Align the placement address if it's not already
    if (align && (g_placement_address & 0xFFFFF000))
    {
        g_placement_address &= 0xFFFFF000;
        g_placement_address += 0x1000;
    }

    uint32_t tmp = g_placement_address;
    g_placement_address += size;
    return (void *)tmp;
}


