#include "kernel_test.h"

#include <kernel/panic.h>
#include <kernel/pmm.h>
#include <kernel/vmm/addr.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_TASKS 2

extern void sys_puts(char *);

uint32_t g_usermode_esp UNUSED;

struct task
{
    uint32_t esp;
    uint32_t eip;
};

struct task tasks[2];
bool current_task;

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

void test_usermode_stage1(void)
{
    uint32_t vaddr = (768 * 1024 * 1024) - (PAGE_SIZE * NUM_TASKS);
    for (int i = 0; i < NUM_TASKS; i++)
    {
        struct page_table_entry pte = { 0 };
        PANIC_IF(!page_alloc(&pte), "out of memory");
        page_map(pte.address << 12, vaddr, true, true);
        vaddr += PAGE_SIZE;
        tasks[i].esp = vaddr;
    }

    tasks[0].eip = (uint32_t)&task1;
    tasks[1].eip = (uint32_t)&task2;

    //test_usermode_stage2();
}

void test_usermode_stage3(void)
{
    sys_puts("hello, ring 3!");
    sys_puts("goodbye, ring 3!");
    for (;;);
}

void task1(void)
{

}

void task2(void)
{

}
