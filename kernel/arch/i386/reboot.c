#include <kernel/reboot.h>
#include <kernel/tty.h>

void reboot(void)
{
    // Just call halt() for now
    halt();
}

void halt(void)
{
    tty_puts_color("halted", TTY_COLOR_PANIC, false);
    //int_disable();
    __asm__ __volatile__("cli");
    __asm__ __volatile__("hlt");
    while (1) ;
    __builtin_unreachable();
}
