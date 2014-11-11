# Declare constants used for creating a multiboot header
.set ALIGN,    (1 << 0)          # align loaded modules on page boundaries
.set MEMINFO,  (1 << 1)          # provide memory map
.set FLAGS,    (ALIGN | MEMINFO) # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002        # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)  # checksum of above, to prove we are multiboot

# Declare other constants
#.set KERNEL_STACK, 0x100000
#.set PAGE_POWER, 12
#.set PAGE_SIZE, (1 << PAGE_POWER)

# Multiboot header section
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
.global g_stack_bottom
g_stack_bottom:
    .skip 16384 # 16KiB
.global g_stack_top
g_stack_top:

# Kernel entry point. This function is called after control is handed over from
# the bootloader (probably GRUB).
.section .text
.global _start
.type _start, @function
_start:
    movl $g_stack_top, %esp

    # Preserve some multiboot register values set by the bootloader
    pushl %ebx
    pushl %eax

    # Initialize core kernel functionality
    call kinit

    # Call global constructors
    call _init

    # Call kmain, which takes two arguments - the multiboot boot magic and the
    # pointer to the multiboot info record itself
    # NOTE: assumes that the stack isn't clobbered by kinit and _init
    call kmain

    # If we get here, then the main kernel function returned and we have
    # nowhere else to go. Halt w/ an infinite loop for backup (though it
    # should never be needed... right?)
    cli
    hlt
.Lhang:
    jmp .Lhang

.size _start, . - _start
