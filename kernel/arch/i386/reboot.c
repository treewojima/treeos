#include <kernel/interrupt.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>

extern void liballoc_dump(void);

void reboot(void)
{
    tty_puts_color("rebooting", TTY_COLOR_PANIC, false);

    // Force a triple fault by loading an empty IDT and triggering an interrupt
    uint64_t fake_idt = 0;
    write_idt((uint32_t)&fake_idt);

    int_enable();
    asm volatile("int $3": : : "memory");

    for (;;);
    __builtin_unreachable();
}

void halt(void)
{
    tty_puts_color("halted", TTY_COLOR_PANIC, false);

    liballoc_dump();

    asm volatile("cli; hlt": : : "memory");

    for (;;);
    __builtin_unreachable();
}
