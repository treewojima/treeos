.section .text

# Load the global descriptor table
# Parameters:
#     uint32_t - pointer to GDT records
#
# Returns:
#     void
#
.global load_gdt
.type load_gdt, @function
load_gdt:
    # Install the new GDT
    movl 4(%esp), %eax
    lgdt (%eax)

    # Reload segment registers
    movw $0x10, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs
    movw %ax  , %ss

    # Longjump to code segment
    ljmp $0x08, $.here
.here:
    ret

# Load the interrupt descriptor table
# Parameters:
#     uint32_t - pointer to IDT records
#
# Returns:
#     void
#
.global load_idt
.type load_idt, @function
load_idt:
    # This code should be fairly self-explanatory
    movl 4(%esp), %eax
    lidt (%eax)
    ret

# Load the default TSS into the task register
# Parameters:
#     none
#
# Returns:
#     void
#
.global load_tss
.type load_tss, @function
load_tss:
    movw $0x2B, %ax
    ltr %ax
    ret

.global jump_usermode
.type jump_usermode, @function
.extern test_usermode
jump_usermode:
    movw $0x23, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movl %esp, %eax
    pushl $0x23
    pushl %eax
    pushfl
    pushl $0x1B
    pushl $test_usermode
    iret
