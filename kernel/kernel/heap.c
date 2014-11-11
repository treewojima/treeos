#include <kernel/heap.h>
#include <kernel/multiboot.h>
#include <stdint.h>

uint32_t g_placement_address;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the kernel heap
 * Parameters:
 *     boot_record - pointer to multiboot info record
 *
 * Returns:
 *     void
 */
void kheap_init(struct multiboot_info *boot_record)
{
    // Start off with the heap at the end of the .bss section
    extern const uint8_t g_bss_end;
    g_placement_address = (uint32_t)&g_bss_end;

    // If there are multiboot modules present, take them into account. Otherwise
    // they'll be counted as free heap space, which would lead to... undefined
    // behavior.
    if (boot_record->mods_count > 0)
    {
        // NOTE: Right now, this code only looks for ONE module. If multiple
        // modules are loaded further on down the line in kernel development,
        // this next part will be bad fucking news.

        struct multiboot_module *module = MB_GET_MODULES(boot_record);
        g_placement_address += module->mod_end;
    }
}
