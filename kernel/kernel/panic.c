#include <kernel/panic.h>

#include <arch/i386/cpu.h> // For struct registers and strcat_registers
#include <arch/i386/serio.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <string.h>

static char msg_buf[MAX_PANIC_BUF];

__attribute__((__noreturn__))
void __panic(const char *file, int line, const char *msg)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "panic (%s:%d): %s", file, line, msg);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_PANIC, false);
    halt();
}

void __worry(const char *file, int line, const char *msg)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "worry (%s:%d): %s\n", file, line, msg);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_WORRY, false);
}

__attribute__((__noreturn__))
void __panic_r(const char *file,
              int line,
              const char *msg,
              const struct registers *const registers)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "panic (%s:%d): %s\n", file, line, msg);
    strcat_registers(msg_buf, registers);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_PANIC, false);
    halt();
}
