#include <kernel/mm.h>

#include <arch/i386/cpu.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern uint32_t g_placement_address;

extern struct page_dir_entry g_kernel_page_dir[1024];
extern struct page_table_entry g_kernel_page_table[1024];

// Page fault handler
static void page_fault_int_handler(struct registers *registers);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize paging and virtual memory
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
#if 0
void mm_init(void)
{
    // Register the page fault interrupt handler
    int_register_handler(14, page_fault_int_handler);

    // Import the existing page directory
    memset(&g_kernel_page_dir, 0, sizeof(g_kernel_page_dir));
    extern uint8_t g_asm_kernel_page_dir;
    g_kernel_page_dir.tables_phys = &g_asm_kernel_page_dir;

    // Flag existing or reserved pages as in-use
    mm_alloc_reserved_pages();

    //printf("kernel page dir = %p\n", kernel_page_dir.tables_phys);
    //printf("kernel page table (low) = %p\n", kernel_page_table);
    //printf("kernel page table (high) = %p\n", kernel_page_table_low);
}
#endif

void mm_init(void)
{
    // Map the first 4MB of memory in the page table (expand this later)
    struct page_table_entry pte;
    uint32_t address = 0;
    for (int i = 0; i < 1024; i++, address += 4096)
    {
        memset(&pte, 0, sizeof(pte));
        pte.present = 1;
        pte.rw = 1;
        pte.address = address >> 12;

        g_kernel_page_table[i] = pte;
    }

    // Since we're only using 4MB, we only need to mark the first entry of the
    // page directory as "present"
    g_kernel_page_dir[0].present = 1;
    g_kernel_page_dir[0].rw = 1;
    g_kernel_page_dir[0].address = (uint32_t)g_kernel_page_table >> 12;

    // Enable paging
    write_cr3((uint32_t)g_kernel_page_dir);
    write_cr0(read_cr0() | 0x80000000);

    // Set up our page fault handler
    int_register_handler(14, page_fault_int_handler);

    printf("[mm] paging enabled\n");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void page_fault_int_handler(struct registers *registers)
{
    char msg_buf[MAX_PANIC_BUF];
    sprintf(msg_buf, "page fault at address %p", read_cr2());
    panic_r(msg_buf, registers);
}
