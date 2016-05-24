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

#include <arch/i386/pmm/allocator.h>

#include <arch/i386/pmm/paging.h>
#include <kernel/const.h>
#include <kernel/debug.h>
#include <string.h>

static uint32_t placement_heap;

void paging_allocator_init(void)
{
    // Set the placement heap to begin at the next page after the kernel tail
    placement_heap = (KERN_END & 0xFFFFF000) + PAGE_SIZE;
}

void *paging_structure_alloc(void)
{
    KASSERT(IS_PAGE_ALIGNED(placement_heap));

    // First, allocate a free page
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");

    // Map it to the next "slot" in the placement heap
    page_map(pte.address << 12,
             placement_heap,
             true, false);

    // Zero the entire page
    memset((void *)placement_heap, 0, PAGE_SIZE);

    uint32_t tmp = placement_heap;
    placement_heap += PAGE_SIZE;
    return (void *)tmp;
}
