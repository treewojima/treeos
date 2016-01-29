#include <arch/i386/proc/task.h>

#include <arch/i386/cpu.h>
#include <stdio.h>
#include <string.h>

struct tss_entry kernel_tss;

void tss_init(void)
{
    memset(&kernel_tss, 0, sizeof(kernel_tss));

    // Set the default kernel stack pointer
    // NOTE: once this is no longer the bootstrap stack, change this!
    extern uint8_t g_stack_top;
    kernel_tss.esp0 = (uint32_t)&g_stack_top;

    // Set the default kernel stack segment
    kernel_tss.ss0 = 0x10;

    // Load the TSS using the offset in the GDT (0x28), plus some control bits
    write_tss(0x2B);

    printf("[cpu] initialized task register\n");
}
