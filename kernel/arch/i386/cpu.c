#include <arch/i386/cpu.h>

#include <arch/i386/proc/task.h> // for struct tss_entry
#include <kernel/const.h>
#include <kernel/interrupt.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <string.h>
#include "isr.h"

// Generic selector access flags
#define SEL_PRESENT   0x80 // present
#define SEL_RING3     0x60 // ring 3 accessible

// Entry in the global descriptor table
struct gdt_entry
{
    uint16_t limit_low;      // lower 16 bits of limit
    uint32_t base_low : 24;  // lower 24 bits of base
    //uint8_t base_middle;     // next 8 bits of base
    uint8_t access;          // access flags
    uint8_t limit_high : 4;  // high 4 bits of limit
    uint8_t granularity : 4; // granularity flags
    uint8_t base_high;       // last 8 bits of base
} PACKED;

// GDT-specific access flags
#define GDT_NOT_TSS        0x10 // must always be set for non-TSS GDT entries
#define GDT_CODE           0x08 // code segment if set, data if not
#define GDT_CODE_CONFORM   0x04 // for code segments - conform privilege bit
#define GDT_DATA_GROW_DOWN 0x04 // for data segments - grow down if set, up if not
#define GDT_CODE_READABLE  0x02 // for code segments - readable if set
#define GDT_DATA_WRITABLE  0x02 // for data segments - writable if set
#define GDT_TSS            0x01 // specifies that this is a TSS segment
#define GDT_TSS_BUSY       0x02 // set if busy, unset otherwise
#define GDT_TSS_32BIT      0x08 // 32 bit if set, 16 bit otherwise

// GDT granularity flags
#define GDT_GRAN_32BIT     0x08 // 32 bit protected mode if set, 16 bit otherwise
#define GDT_GRAN_PAGE      0x04 // page granularity if set, byte granularity if not
#define GDT_GRAN_AVAILABLE 0x01 // this flag is free for use by OS (rename later)

// Precooked GDT access and granularity flags
#define GDT_KERN_CODE    (SEL_PRESENT | GDT_NOT_TSS | GDT_CODE | GDT_CODE_READABLE)
#define GDT_KERN_DATA    (SEL_PRESENT | GDT_NOT_TSS | GDT_DATA_WRITABLE)
#define GDT_USER_CODE    (GDT_KERN_CODE | SEL_RING3)
#define GDT_USER_DATA    (GDT_KERN_DATA | SEL_RING3)
#define GDT_KERN_TSS     (SEL_PRESENT | GDT_TSS | GDT_TSS_32BIT)
#define GDT_USER_TSS     (GDT_KERN_TSS | SEL_RING3)
#define GDT_DEFAULT_GRAN (GDT_GRAN_PAGE | GDT_GRAN_32BIT)
#define GDT_TSS_GRAN     (GDT_GRAN_AVAILABLE)

// Null entry + kernel data/code + user data/code + TSS
#define NUM_GDT_ENTRIES 6

// Entry in the interrupt descriptor table
struct idt_entry
{
    uint16_t base_low;      // lower 16 bits of address to jump to when the
                            //   interrupt fires
    uint16_t selector, : 8; // kernel segment selector (and 8 excess zero bits)
    uint8_t access;         // access flags
    uint16_t base_high;     // upper 16 bits of address to jump to
} PACKED;

// IDT-specific access/type flags
#define IDT_INTERRUPT 0x0E
#define IDT_TRAP      0x0F

// Precooked IDT access bytes
#define IDT_KERN_INT  (SEL_PRESENT | IDT_INTERRUPT)
#define IDT_KERN_TRAP (SEL_PRESENT | IDT_TRAP)
#define IDT_USER_INT  (IDT_KERN_INT | SEL_RING3)
#define IDT_USER_TRAP (IDT_KERN_TRAP | SEL_RING3)

// Limit and base structure, used for both GDT and IDT work
struct limit_and_base
{
    uint16_t limit;
    uint32_t base;
} PACKED;

static struct gdt_entry gdt_entries[NUM_GDT_ENTRIES];
static struct idt_entry idt_entries[INT_NUM_INTERRUPTS];
//static struct tss_entry tss;

static void set_gdt_gate(uint8_t num,
                         uint32_t base,
                         uint32_t limit,
                         uint8_t access,
                         uint8_t gran);
static void set_idt_gate(uint8_t num,
                         uint32_t base,
                         uint16_t selector,
                         uint8_t access);
static void fill_idt(void);

void gdt_init(void)
{
    struct limit_and_base gdt_ptr;
    gdt_ptr.limit = sizeof(struct gdt_entry) * NUM_GDT_ENTRIES - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    memset(gdt_entries, 0, sizeof(struct gdt_entry) * NUM_GDT_ENTRIES);

    // Null segment
    set_gdt_gate(0, 0, 0, 0, 0);

    // Kernel code
    set_gdt_gate(1, 0, 0xFFFFF, GDT_KERN_CODE, GDT_DEFAULT_GRAN);

    // Kernel data
    set_gdt_gate(2, 0, 0xFFFFF, GDT_KERN_DATA, GDT_DEFAULT_GRAN);

    // User code
    set_gdt_gate(3, 0, 0xFFFFF, GDT_USER_CODE, GDT_DEFAULT_GRAN);

    // User data
    set_gdt_gate(4, 0, 0xFFFFF, GDT_USER_DATA, GDT_DEFAULT_GRAN);

    // Set up TSS
    extern struct tss_entry kernel_tss;
    set_gdt_gate(5, (uint32_t)&kernel_tss, sizeof(kernel_tss),
                 GDT_USER_TSS, GDT_TSS_GRAN);

    write_gdt((uint32_t)&gdt_ptr);

    printf("[cpu] initialized GDT\n");
}

void idt_init(void)
{
    struct limit_and_base idt_ptr;
    idt_ptr.limit = sizeof(struct idt_entry) * INT_NUM_INTERRUPTS;
    idt_ptr.base = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * INT_NUM_INTERRUPTS);

    fill_idt();

    // Finally, load/flush the IDT
    write_idt((uint32_t)&idt_ptr);

    printf("[cpu] initialized IDT (%d entries)\n", INT_NUM_INTERRUPTS);
}

void set_gdt_gate(uint8_t num,
                  uint32_t base,
                  uint32_t limit,
                  uint8_t access,
                  uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFFFF);
    //gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    // Limit can only be 20 bits
    PANIC_IF(limit & 0xFFF00000, "limit out of range");
    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].limit_high  = (limit >> 16) & 0x0F;

    // Granularity can only be 4 bits (use the low bits)
    PANIC_IF(gran & 0xF0, "granularity out of range");
    gdt_entries[num].granularity = gran & 0x0F;
    gdt_entries[num].access      = access;
}

void set_idt_gate(uint8_t num,
                  uint32_t base,
                  uint16_t selector,
                  uint8_t access)
{
    idt_entries[num].base_low  = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

    idt_entries[num].selector  = selector;
    idt_entries[num].access    = access;
}

void fill_idt(void)
{
    // Initialize gates 0-31
    // NOTE: This can probably be done more elegantly, but this works fine
    set_idt_gate(0,  (uint32_t)isr0,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(1,  (uint32_t)isr1,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(2,  (uint32_t)isr2,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(3,  (uint32_t)isr3,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(4,  (uint32_t)isr4,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(5,  (uint32_t)isr5,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(6,  (uint32_t)isr6,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(7,  (uint32_t)isr7,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(8,  (uint32_t)isr8,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(9,  (uint32_t)isr9,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(10, (uint32_t)isr10, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(11, (uint32_t)isr11, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(12, (uint32_t)isr12, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(13, (uint32_t)isr13, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(14, (uint32_t)isr14, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(15, (uint32_t)isr15, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(16, (uint32_t)isr16, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(17, (uint32_t)isr17, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(18, (uint32_t)isr18, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(19, (uint32_t)isr19, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(20, (uint32_t)isr20, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(21, (uint32_t)isr21, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(22, (uint32_t)isr22, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(23, (uint32_t)isr23, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(24, (uint32_t)isr24, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(25, (uint32_t)isr25, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(26, (uint32_t)isr26, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(27, (uint32_t)isr27, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(28, (uint32_t)isr28, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(29, (uint32_t)isr29, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(30, (uint32_t)isr30, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(31, (uint32_t)isr31, KERN_CODE_SELECTOR, IDT_KERN_INT);

    // ... and gates 32-47 (our IRQs)
    set_idt_gate(32, (uint32_t)irq0,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(33, (uint32_t)irq1,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(34, (uint32_t)irq2,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(35, (uint32_t)irq3,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(36, (uint32_t)irq4,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(37, (uint32_t)irq5,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(38, (uint32_t)irq6,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(39, (uint32_t)irq7,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(40, (uint32_t)irq8,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(41, (uint32_t)irq9,  KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(42, (uint32_t)irq10, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(43, (uint32_t)irq11, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(44, (uint32_t)irq12, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(45, (uint32_t)irq13, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(46, (uint32_t)irq14, KERN_CODE_SELECTOR, IDT_KERN_INT);
    set_idt_gate(47, (uint32_t)irq15, KERN_CODE_SELECTOR, IDT_KERN_INT);

    // ... and the syscall gate (48), with ring 3 privilege
    set_idt_gate(48, (uint32_t)isr48, KERN_CODE_SELECTOR, IDT_USER_INT);
}
