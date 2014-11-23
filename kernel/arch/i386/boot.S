# Multiboot header section
.section .multiboot

# Declare constants used for creating a multiboot header
.set ALIGN,    (1 << 0)          # align loaded modules on page boundaries
.set MEMINFO,  (1 << 1)          # provide memory map
.set FLAGS,    (ALIGN | MEMINFO) # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002        # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)  # checksum of above, to prove we are multiboot

.align 0x100
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

# Data section - includes the initial page directories/tables
.section .data

# Reserve space for the initial kernel page directory and the low/high tables
.align 0x1000
.global g_kernel_page_dir
g_kernel_page_dir:
    .space 0x1000, 0x00
.global g_kernel_page_table
g_kernel_page_table:
    .space 0x1000, 0x00

# The kernel bootstrap stack
.section .bootstrap_stack, "aw", @nobits
.global g_stack_bottom
g_stack_bottom:
    .skip 0x4000 # 16 KiB
.global g_stack_top
g_stack_top:

# Regular code section
.section .text

# Kernel entry point; called after control is handed over from the bootloader
.global _start
.type _start, @function
_start:
    # Set the stack pointer
    movl $g_stack_top, %esp
    movl %esp        , %ebp

    # Initialize core kernel functionality and push the multiboot arguments
    pushl %ebx
    pushl %eax
    call kinit

    # Call global constructors
    call _init

    # Call the main kernel function
    call kmain

    # If we get here, then the main kernel function returned and there's
    # nowhere else to go. Halt w/ an infinite loop for good measure
    cli
    hlt
.Lhang:
    jmp .Lhang

.size _start, . - _start
