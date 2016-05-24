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

#include <arch/i386/vmm/addr.h>

#include <arch/i386/pmm/paging.h>
#include <kernel/panic.h>
#include <stdint.h>
#include <stdio.h>

void *virt_to_phys(void *vaddr)
{
    uint32_t pgdir_index = (uint32_t)vaddr >> 22;
    uint32_t pgtbl_index = (uint32_t)vaddr >> 12 & 0x03FF;

    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    if (!pde[pgdir_index].present)
    {
        char msg[128];
        sprintf(msg,
                "virtual address %x references invalid page directory entry %u",
                (uint32_t)vaddr,
                pgdir_index);
        panic(msg);
    }

    struct page_table_entry *pte =
            ((struct page_table_entry *)0xFFC00000) + (0x400 * pgdir_index);
    if (!pte[pgtbl_index].present)
    {
        char msg[128];
        sprintf(msg,
                "virtual address %x references invalid page table entry %u in page directory %u",
                (uint32_t)vaddr,
                pgtbl_index,
                pgdir_index);
        panic(msg);
    }

    uint32_t *pte_int = (uint32_t *)&pte[pgtbl_index];
    return (void *)((*pte_int & ~0xFFF) + ((uint32_t)vaddr & 0xFFF));
}
