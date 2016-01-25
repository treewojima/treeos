#include <kernel/interrupt.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>

void reboot(void)
{
    // Just call halt() for now
    halt();
    __builtin_unreachable();
}

void halt(void)
{
    tty_puts_color("halted", TTY_COLOR_PANIC, false);
    int_disable();
    __asm__ __volatile__("hlt");
    for (;;) ;
    __builtin_unreachable();
}
