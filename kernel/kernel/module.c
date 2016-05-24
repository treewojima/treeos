/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kernel/module.h>

#include <kernel/debug.h>
#include <kernel/multiboot.h>
#include <kernel/pmm.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Default location for modules
#define MODULES_START_ADDR 0xE0000000

struct module
{
    char name[8];
    uint32_t addr;
    uint32_t size;
};

static struct module *module_table;
static uint32_t module_table_size;

void module_table_init(void)
{
    const struct multiboot_info *const mbi = &g_multiboot_info;

    printf("%u Multiboot module%s found\n",
           mbi->mods_count,
           mbi->mods_count == 1 ? "" : "s");

    module_table_size = mbi->mods_count;
    if (!module_table_size) return; // nothing to do

    module_table = kcalloc(module_table_size, sizeof(*module_table));

    struct multiboot_mod_list *mod =
            (struct multiboot_mod_list *)g_multiboot_info.mods_addr;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < module_table_size; i++, mod++)
    {
        KASSERT(mod);

        printf("   module %u (%p):\n", i, mod);
        printf("      mod_start = %p\n", mod->mod_start);
        printf("      mod_end   = %p\n", mod->mod_end);
        printf("      cmdline   = %p (\"%s\")\n",
               mod->cmdline,
               mod->cmdline ? (const char *)mod->cmdline : "<null>");

        // Calculate page aligned values for use in page allocation
        uint32_t mod_start = PAGE_ALIGN_DOWN(mod->mod_start);
        uint32_t mod_end = PAGE_ALIGN_UP(mod->mod_end);
        uint32_t mod_size = mod_end - mod_start;
        uint32_t pages = mod_size >> PAGE_SIZE_SHIFT;

        // Map however many pages are required
        for (uint32_t k = 0; k < pages; k++)
        {
            page_map(mod_start + (k << PAGE_SIZE_SHIFT),
                     (MODULES_START_ADDR + offset) + (k << PAGE_SIZE_SHIFT),
                     false, false);
        }

        // Fill in the module table entry, making sure to use the actual
        // virtual address values rather than page-aligned or physical ones
        module_table[i].addr =
                (MODULES_START_ADDR + offset) + (mod->mod_start % PAGE_SIZE);
        module_table[i].size = mod->mod_end - mod->mod_start;
        if (mod->cmdline)
        {
            const char *cmdline = (const char *)mod->cmdline;
            strncpy(module_table[i].name, cmdline + 2, 7);
        }

        // Allow for a blank page between modules as a canary
        offset += mod_size + PAGE_SIZE;
    }
}

void *get_module_by_name(const char *name)
{
    for (uint32_t i = 0; i < module_table_size; i++)
    {
        if (!strcmp(name, module_table[i].name))
        {
            return (void *)module_table[i].addr;
        }
    }

    return NULL;
}
