#include <arch/i386/cpu.h>
#include <arch/i386/interrupt.h>
#include <kernel/panic.h>
#include <kernel/syscall.h>
#include <stdio.h>
#include <string.h>

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void isr48(void);

// Entry in the global descriptor table
#if 1
struct gdt_entry
{
    uint16_t limit_low;   // lower 16 bits of limit
    uint16_t base_low;    // lower 16 bits of base
    uint8_t base_middle;  // next 8 bits of base
    uint8_t access;       // access flags
    uint8_t granularity;  // granularity byte
    uint8_t base_high;    // last 8 bits of base
} __attribute__((packed));
#else
struct gdt_entry
{
    uint16_t limit_low;
    uint32_t base_low : 24;

    // attribute byte
    uint8_t accessed : 1;
    uint8_t read_write : 1;             // readable for code, writeable for
                                        //     data
    uint8_t conforming_expand_down : 1; // conforming for code, expand down
                                        //     for data
    uint8_t code : 1;                   // 1 for code, 0 for data
    uint8_t always_one : 1;             // should always be 1 for everything
                                        //     but TSS and LDT
    uint8_t dpl : 2;                    // privilege level
    uint8_t present : 1;

    // granularity byte
    uint8_t limit_high : 4;
    uint8_t available : 1;
    uint8_t always_zero : 1; // should always be 0 no matter what
    uint8_t big : 1;         // 32bit opcodes for code, dword stack for data
    uint8_t granularity : 1; // 1 to use 4k page addressing, 0 for byte
                             //     addressing

    uint8_t base_high;
} __attribute__((packed));
#endif

// Entry in the interrupt descriptor table
struct idt_entry
{
    uint16_t base_low;      // lower 16 bits of address to jump to when the
                            //   interrupt fires
    uint16_t selector, : 8;  // kernel segment selector (and 8 excess zero bits)
    uint8_t flags;
    uint16_t base_high;     // upper 16 bits of address to jump to
} __attribute__((packed));

// Task selector segment descriptor
struct tss_entry
{
    uint32_t prev_tss;   // previous TSS - if using hardware task switching,
                         //     this acts as a linked list
    uint32_t esp0;       // stack pointer to load when changing to kernel mode
    uint32_t ss0;        // stack segment to load when changing to kernel mode

    // Unless implementing hardware task switching, the rest of this structure
    // is unused
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

// Limit and base structure, used for both GDT and IDT work
struct limit_and_base
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#define NUM_GDT_ENTRIES 6

static struct gdt_entry gdt_entries[NUM_GDT_ENTRIES];
static struct idt_entry idt_entries[INT_NUM_INTERRUPTS];
static struct tss_entry tss;

static void gdt_set_gate(uint8_t num,
                         uint32_t base,
                         uint32_t limit,
                         uint8_t access,
                         uint8_t gran);
static void idt_set_gate(uint8_t num,
                         uint32_t base,
                         uint16_t selector,
                         uint8_t flags);

void gdt_init(void)
{
    struct limit_and_base gdt_ptr;
    gdt_ptr.limit = sizeof(struct gdt_entry) * NUM_GDT_ENTRIES - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    memset(gdt_entries, 0, sizeof(struct gdt_entry) * NUM_GDT_ENTRIES);

    // Set up ring segments
    gdt_set_gate(0, 0, 0, 0, 0);                // null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user mode data segment

    // Set up TSS
    gdt_set_gate(5, (uint32_t)&tss, sizeof(struct tss_entry), 0x89, 0x40);

    load_gdt((uint32_t)&gdt_ptr);

    printf("[cpu] initialized GDT\n");
}

void idt_init(void)
{
    struct limit_and_base idt_ptr;
    idt_ptr.limit = sizeof(struct idt_entry) * INT_NUM_INTERRUPTS;
    idt_ptr.base = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * INT_NUM_INTERRUPTS);

    // Initialize gates 0-31
    // NOTE: This can probably be done more elegantly, but this works fine
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    // ... and gates 32-47 (our IRQs)
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    // ... and the syscall gate (48)
    idt_set_gate(48, (uint32_t)isr48, 0x08, 0x8E);

    // Finally, load/flush the IDT
    load_idt((uint32_t)&idt_ptr);

    printf("[cpu] initialized IDT (%d entries)\n", INT_NUM_INTERRUPTS);
}

void tss_init(void)
{
    memset(&tss, 0, sizeof(struct tss_entry));

    // Set the default kernel stack pointer
    // NOTE: once this is no longer the bootstrap stack, change this!
    extern uint8_t g_stack_top;
    tss.esp0 = (uint32_t)&g_stack_top;

    // Set the default kernel stack segment
    tss.ss0 = 0x10;

    load_tss();

    printf("[cpu] initialized task register\n");
}

static void gdt_set_gate(uint8_t num,
                         uint32_t base,
                         uint32_t limit,
                         uint8_t access,
                         uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

static void idt_set_gate(uint8_t num,
                         uint32_t base,
                         uint16_t selector,
                         uint8_t flags)
{
    idt_entries[num].base_low  = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

    idt_entries[num].selector  = selector;
    // Uncomment the OR portion when user-mode functionality is enabled
    idt_entries[num].flags     = flags | 0x60;
}

uint32_t get_eflags(void)
{
    uint32_t eflags;
    __asm__ __volatile__("pushfl; popl %0" : "=a"(eflags));
    return eflags;
}

char *strcat_registers(char *buf, const struct registers *const registers)
{
    int pos = 0;
    while (*(buf + pos) != '\0')
        pos++;

    pos += sprintf(buf + pos, "  eax=%x ebx=%x ecx=%x edx=%x\n",
                   registers->eax, registers->ebx, registers->ecx,
                   registers->edx);
    pos += sprintf(buf + pos, "  ebp=%x esi=%x edi=%x\n",
                   registers->ebp, registers->esi, registers->edi);
    pos += sprintf(buf + pos, "  ds=%x es=%x fs=%x gs=%x\n",
                   registers->ds, registers->es, registers->fs,
                   registers->gs);
    pos += sprintf(buf + pos, "  cs:eip=%x:%x\n",
                   registers->eip, registers->cs);
    pos += sprintf(buf + pos, "  eflags=%x\n",
                   registers->eflags);
    pos += sprintf(buf + pos, "  int=%d err=%x\n",
                   registers->int_num, registers->error_code);
    pos += sprintf(buf + pos, "  esp=%x ss=%x\n",
                   registers->esp, registers->ss);

    return buf;
}

void test_usermode(void)
{
    //syscall(666);
    while (true);
}
