#include <kernel/panic.h>

#include <arch/i386/proc/thread.h> // For struct thread_context and strcat_registers
#include <arch/i386/serio.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <string.h>

static char msg_buf[MAX_PANIC_BUF];

void _panic(const char *file, int line, const char *msg)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "panic (%s:%d): %s", file, line, msg);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_PANIC, true);
    halt();
}

void _worry(const char *file, int line, const char *msg)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "worry (%s:%d): %s\n", file, line, msg);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_WORRY, true);
}

void _panic_r(const char *file,
              int line,
              const char *msg,
              const struct thread_context *const context)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "panic (%s:%d): %s\n", file, line, msg);
    strcat_registers(msg_buf, context);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_PANIC, true);
    halt();
}
