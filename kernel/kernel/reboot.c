#include <kernel/reboot.h>

void reboot(void)
{
    // Just call halt() for now
    halt();
}

void halt(void)
{
    //int_disable();
    __asm__ __volatile__("cli");
    __asm__ __volatile__("hlt");
    while (1) ;
    __builtin_unreachable();
}
