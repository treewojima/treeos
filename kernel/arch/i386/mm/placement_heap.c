#include <kernel/mm.h>

#include <kernel/debug.h>
#include <kernel/multiboot.h>
#include <string.h>

#define DEFAULT_PLACEMENT_HEAP_SIZE 0x400000

// Place the default kernel heap at 3 GiB + 16 MiB in memory and 4 MiB in size
// NOTE: This is just temporary
extern uint8_t KERNEL_HIGH_VMA;
#define DEFAULT_KERNEL_HEAP_LOC  ((uint32_t)&KERNEL_HIGH_VMA + 0x1000000)
#define DEFAULT_KERNEL_HEAP_SIZE 0x400000

static uint32_t placement_address;

void mm_init_placement_heap(void)
{
    // Start off with the kernel heap at the end of the kernel image
    extern const uint8_t g_placement_heap_start;
    placement_address = (uint32_t)&g_placement_heap_start;

    //g_placement_address = (uint32_t)&placement_heap;

    // If there are multiboot modules present, take them into account. Otherwise
    // they'll be counted as free heap space, which would lead to... undefined
    // behavior.
    // TODO: Add module support
    extern const struct multiboot_info g_multiboot_info;
    KASSERT(g_multiboot_info.mods_count == 0);
}

void *mm_placement_alloc(uint32_t size, bool zero, bool align)
{
    // Align the placement address if it's not already
    if (align && (placement_address & 0xFFFFF000))
    {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    uint32_t tmp = placement_address;
    placement_address += size;
    //PANIC_IF(g_placement_address - (uint32_t)&placement_heap >= DEFAULT_PLACEMENT_HEAP_SIZE,
    //         "out of memory in kernel placement heap");

    if (zero) memset((void *)tmp, 0, size);

    return (void *)tmp;
}
