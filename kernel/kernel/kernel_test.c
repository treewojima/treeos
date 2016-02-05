#include "kernel_test.h"

#include <kernel/panic.h>
#include <kernel/pmm.h>
#include <kernel/vmm/addr.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void test_map_page(void)
{
    printf("testing map_page\n");

    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");

    const uint32_t vaddr = 768 * 1024 * 1024;

    uint32_t paddr = pte.address << 12;
    page_map(paddr, vaddr, true, true);

    int *test = (int *)vaddr;
    *test = 69;
    printf("*test      = %d\n", *test);
    printf("test_vaddr = %p\n", test);
    printf("test_paddr = %p\n", virt_to_phys(test));

    page_unmap(vaddr);
    page_free(&pte);

    printf("done testing map_page\n");
}

void test_malloc(void)
{
    printf("testing malloc\n");

    const int ARRAY_SIZE = 5;
    int *foo = kmalloc(sizeof(*foo) * ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        foo[i] = i * 2;
    }

    for (int i = ARRAY_SIZE - 1; i >= 0; i--)
    {
        printf("foo[%d] = %d\n", i, foo[i]);
    }

    char *bar = kcalloc(ARRAY_SIZE, 1);
    sprintf(bar, "hai2u");
    printf("bar = %s\n", bar);

    kfree(foo);
    kfree(bar);

    printf("done testing malloc\n");
}

void test_usermode_c(void)
{
    extern void sys_puts(char *);
    sys_puts("hello, ring 3!");
    for (;;);
}
