#include <arch/i386/serio.h>

#include <arch/i386/ioport.h>
#include <kernel/debug.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the driver for the specified serial port
 * Parameters:
 *     com - COM port to initialize
 *
 * Returns:
 *     void
 */
void serio_init(int com)
{
    ioport_outb(com + 1, 0x0);  // disable interrupts
    ioport_outb(com + 3, 0x80); // enable DLAB (set baud rate divisor)
    ioport_outb(com + 0, 0x3);  // set divisor to 3 (low byte) for 38400 baud
    ioport_outb(com + 1, 0x0);  //                  (high byte)
    ioport_outb(com + 3, 0x3);  // 8 bits, no parity, one stop bit
    ioport_outb(com + 2, 0xC7); // enable FIFO, clear with 14 byte threshold
    ioport_outb(com + 4, 0xB);  // enable IRQs, RTS/DSR set

    char port_str[5];
    switch (com)
    {
    case SERIO_COM1:
        strcpy(port_str, "COM1");
        break;

    case SERIO_COM2:
        strcpy(port_str, "COM2");
        break;

    case SERIO_COM3:
        strcpy(port_str, "COM3");
        break;

    case SERIO_COM4:
        strcpy(port_str, "COM4");
        break;

    default:
        strcpy(port_str, "COM?");
    }

    char msg_buf[32];
    sprintf(msg_buf, "serial port %s initialized", port_str);
    serio_puts(com, msg_buf, true);
}

/* Check for queued incoming data on a serial port
 * Parameters:
 *     com - COM port to check
 *
 * Returns:
 *     bool - true if data is waiting to be read
 */
bool serio_data_incoming(int com)
{
    return ioport_inb(com + 5) & 1;
}

/* Read a character from a serial port
 * Parameters:
 *     com - COM port to read from
 *
 * Returns:
 *     char - character that was read
 */
char serio_readchar(int com)
{
    while (!serio_data_incoming(com));

    return ioport_inb(com);
}

/* Check for queued outgoing data on a serial port
 * Parameters:
 *     com - COM port to check
 *
 * Returns:
 *     bool - true if data is waiting to be sent
 */
bool serio_data_outgoing(int com)
{
    return !(ioport_inb(com + 5) & 0x20);
}

/* Write a character to a serial port
 * Parameters:
 *     com - COM port to write to
 *     c   - character to write
 *
 * Returns:
 *     void
 */
void serio_writechar(int com, char c)
{
    while (serio_data_outgoing(com));

    ioport_outb(com, c);
}

/* Write a string to a serial port
 * Parameters:
 *     com     - COM port to write to
 *     str     - string to write
 *     newline - flag to append carriage return and newline
 *
 * Returns:
 *     int - number of characters written
 */
int serio_puts(int com, const char *str, bool newline)
{
    KASSERT_WORRY(str != NULL);

    int count = 0;

    while (*str != '\0')
    {
        serio_writechar(com, *str);
        str++; count++;
    }

    if (newline)
    {
        serio_writechar(com, '\r');
        serio_writechar(com, '\n');
        count += 2;
    }

    return count;
}
