#include <arch/i386/ioport.h>

uint8_t ioport_inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

uint16_t ioport_inw(uint16_t port)
{
    uint16_t ret;
    __asm__ __volatile__ ("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

uint32_t ioport_inl(uint16_t port)
{
    uint32_t ret;
    __asm__ __volatile__ ("inl %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

void ioport_outb(uint16_t port, uint8_t value)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN"(port), "a"(value));
}

void ioport_outw(uint16_t port, uint16_t value)
{
    __asm__ __volatile__ ("outw %1, %0" : : "dN"(port), "a"(value));
}

void ioport_outl(uint16_t port, uint32_t value)
{
    __asm__ __volatile__ ("outl %1, %0" : : "dN"(port), "a"(value));
}

void ioport_delay(void)
{
    ioport_outb(0x80, 0);
}

