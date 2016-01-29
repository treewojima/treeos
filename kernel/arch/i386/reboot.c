#include <kernel/interrupt.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>

void reboot(void)
{
    tty_puts_color("rebooting", TTY_COLOR_PANIC, false);

    // Force a triple fault by loading an empty IDT and triggering an interrupt
    uint64_t fake_idt = 0;
    write_idt((uint32_t)&fake_idt);

    int_enable();
    asm volatile("int $48": : : "memory");

    for (;;);
    __builtin_unreachable();
}

void halt(void)
{
    tty_puts_color("halted", TTY_COLOR_PANIC, false);

    int_set_raw_mask(0xFFFF);
    int_disable();
    asm volatile("hlt": : : "memory");

    for (;;);
    __builtin_unreachable();
}
