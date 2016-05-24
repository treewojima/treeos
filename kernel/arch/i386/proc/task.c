#include <arch/i386/proc/task.h>

#include <arch/i386/cpu.h>
#include <kernel/const.h>
#include <stdio.h>
#include <string.h>

struct tss_entry g_kernel_tss;

void tss_init(void)
{
    memset(&g_kernel_tss, 0, sizeof(g_kernel_tss));

    // Set the default kernel stack pointer
    g_kernel_tss.esp0 = KERN_STACK_TOP;

    // Set the default kernel stack segment
    g_kernel_tss.ss0 = KERN_DATA_SELECTOR;

    // Load the TSS using the offset in the GDT (0x28), plus some control bits
    write_tss(0x2B);

    printf("[cpu] initialized task register\n");
}
