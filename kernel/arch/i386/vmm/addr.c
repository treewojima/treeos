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
