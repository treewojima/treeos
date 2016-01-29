.section .text

# Load the global descriptor table
# Parameters:
#     uint32_t - pointer to GDT records
#
# Returns:
#     void
#
.global write_gdt
.type write_gdt, @function
write_gdt:
    pushl %ebp
    movl  %esp, %ebp

    # Install the new GDT
    movl 8(%ebp), %eax
    lgdt (%eax)

    # Reload segment registers
    movw $0x10, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs
    movw %ax  , %ss

    # Longjump to code segment
    ljmp $0x08, $.Lhere
.Lhere:
    movl %ebp, %esp
    popl %ebp
    ret

# Load the interrupt descriptor table
# Parameters:
#     uint32_t - pointer to IDT records
#
# Returns:
#     void
#
.global write_idt
.type write_idt, @function
write_idt:
    pushl %ebp
    movl  %esp, %ebp

    # This code should be fairly self-explanatory
    movl 8(%ebp), %eax
    lidt (%eax)

    movl %ebp, %esp
    popl %ebp
    ret

# Load the specified TSS into the task register
# Parameters:
#     uint16_t - index/offset of TSS entry in GDT
#
# Returns:
#     void
#
.global write_tss
.type write_tss, @function
write_tss:
    pushl %ebp
    movl  %esp, %ebp

    #movw $0x2B, %ax
    movw 8(%ebp), %ax
    ltr %ax

    movl %ebp, %esp
    popl %ebp
    ret

# Read the value of the cr0 register
# Parameters:
#     none
#
# Returns:
#     uint32_t
#
.global read_cr0
.type read_cr0, @function
read_cr0:
    movl %cr0, %eax
    ret

# Write a value to the cr0 register
# Parameters:
#     uint32_t - new value
#
# Returns:
#     void
#
.global write_cr0
.type write_cr0, @function
write_cr0:
    pushl %ebp
    movl  %esp, %ebp

    movl 8(%ebp), %eax
    movl %eax, %cr0

    movl %ebp, %esp
    popl %ebp
    ret

# Read the value of the cr2 register
# Parameters:
#     none
#
# Returns:
#     uint32_t
#
.global read_cr2
.type read_cr2, @function
read_cr2:
    movl %cr2, %eax
    ret

# Read the value of the cr3 register
# Parameters:
#     none
#
# Returns:
#     uint32_t
#
.global read_cr3
.type read_cr3, @function
read_cr3:
    movl %cr3, %eax
    ret

# Write a value to the cr3 register
# Parameters:
#     uint32_t - new value
#
# Returns:
#     void
#
.global write_cr3
.type write_cr3, @function
write_cr3:
    pushl %ebp
    movl  %esp, %ebp

    movl 8(%ebp), %eax
    movl %eax, %cr3

    movl %ebp, %esp
    popl %ebp
    ret

# Read the value of the eflags register
# Parameters:
#     none
#
# Returns:
#     uint32_t
#
.global read_eflags
.type read_eflags, @function
read_eflags:
    pushfl
    popl %eax
    ret

# Old debug code
.global jump_usermode
.type jump_usermode, @function
.extern test_usermode
jump_usermode:
    #pushl %ebp
    #movl  %esp, %ebp

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

    #popl %ebp
    iret
