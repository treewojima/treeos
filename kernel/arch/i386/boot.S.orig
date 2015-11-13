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

# Reserve space for the kernel bootstrap page directory and table
.align 0x1000
bootstrap_page_dir:
    .space 0x1000, 0x00
bootstrap_page_table:
    .space 0x1000, 0x00
bootstrap_page_table_low:
    .space 0x1000, 0x00

# Reserve space for the kernel page directory and table
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

.section .text

.global _gdb_start
.set _gdb_start, (_start - 0xC0000000)

# Kernel entry point; called after control is handed over from the bootloader
.global _start
.type _start, @function
_start:
    # Set the stack/base pointers to account for the offset into the higher half
    movl $g_stack_top       , %esp
    subl $KERNEL_VIRT_OFFSET, %esp
    #movl %esp               , %ebp

    # Initialize the bootstrap paging mechanism
    call init_bootstrap_paging

    # Adjust stack/base pointers to the proper virtual address
    addl $KERNEL_VIRT_OFFSET, %esp
    movl %esp               , %ebp

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

# Initialize bootstrap paging
.type init_bootstrap_paging, @function
init_bootstrap_paging:
    # Preserve some registers that this function will clobber
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx

    # Load the page directory and low page table addresses
    movl $bootstrap_page_dir      , %eax
    subl $KERNEL_VIRT_OFFSET      , %eax
    movl $bootstrap_page_table_low, %ebx
    subl $KERNEL_VIRT_OFFSET      , %ebx

    # Set the "present" bit of the page table
    orl $0x1, %ebx

    # Load the low page table into the page directory
    movl %ebx, (%eax)

    # Calculate the offset of the high page table in the page directory
    pushl %eax
    movl  $KERNEL_VIRT_OFFSET, %eax  # get virtual address offset
    shrl  $22                , %eax  # calculate index in the page dir
    movl  $4                 , %ecx
    mull  %ecx                       # calculate byte offset (4 bytes per entry)
    movl  %eax               , %edx
    popl  %eax

    # Move the pointer to the page directory to the correct entry
    pushl %eax        # for use in ebx down the road
    addl  %edx, %eax

    # Load the adjusted high page table into the page directory
    movl $bootstrap_page_table, %ebx
    subl $KERNEL_VIRT_OFFSET  , %ebx
    orl  $0x1                 , %ebx
    movl %ebx                 , (%eax)

    popl %ebx  # retrieve the value stored from eax earlier

    # Start mapping the lower 1 MiB of memory
    movl $0x100000, %ecx
.Lmap_first_mib_loop:
    movl %ecx   , %edx         # physical == virtual (identity mapping)
    call map_bootstrap_page
    subl $0x1000, %ecx         # advance by one page
    jnz  .Lmap_first_mib_loop  # loop (but quit when we reach zero)

    # Map the low (identity) and high (adjusted) pages
    movl $KERNEL_PHYS_ADDR  , %ecx
    movl $g_kernel_end      , %eax  # this starts out as virtual, so...
    subl $KERNEL_VIRT_OFFSET, %eax  # adjust for physical
.Lmap_kernel_loop:
    # First map the low page
    movl %ecx, %edx          # physical == virtual (identity mapping)
    call map_bootstrap_page

    # Now the high page
    pushl %ecx
    addl  $KERNEL_VIRT_OFFSET, %ecx  # map the high virtual address to the same
                                     # physical address as in the low page
    call  map_bootstrap_page
    popl  %ecx

    # Advance by one page and quit when the kernel is mapped
    addl $0x1000, %ecx
    cmp  %eax   , %ecx
    jle  .Lmap_kernel_loop

    # Set the last entry of the page directory to itself
    movl %ebx       , %eax
    addl $4092      , %eax   # page dir index 1023 * 4 bytes in size
    movl %ebx       , %ecx
    andl $0xFFFFF000, %ecx   # strip off the last 12 flag bits
    orl  $0x3       , %ecx
    movl %ecx       , (%eax)

    #Enable paging
    movl %ebx     , %cr3  # insert the kernel page directory into cr3
    movl %cr0     , %eax  # fetch the current cr0 value
    orl $(1 << 31), %eax  # enable the paging bit
    movl %eax     , %cr0  # write the new cr0 value

    # Clean up by restoring registers from the stack and return
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    ret

# Takes arguments in registers:
#    ebx - physical address of kernel page directory
#    ecx - virtual address to map
#    edx - physical address to map to
.type map_bootstrap_page, @function
map_bootstrap_page:
    # Preserve some registers that this function will clobber
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx

    # Preserve some of the arguments (for use in calculating offsets)
    pushl %edx
    pushl %ecx

    # Fetch the page table from the page directory
    movl %ecx  , %eax
    shrl $22   , %eax
    movl $4    , %ecx
    mull %ecx          # offset is now stored in eax
    addl %eax  , %ebx  # set ebx to point to the phys addr of a page table
    movl (%ebx), %eax

    # Double-check the present flag for the calculated page directory entry
    movl %eax, %ecx
    andl $0x1, %ecx
    cmpl $0  , %ecx
    je bootstrap_panic  # if flag is zero, panic

    # Extract the physical page table address from the page directory entry by
    # stripping off the last 12 flag bits
    andl $0xFFFFF000, %eax

    # Fetch the virtual address argument that was stashed earlier
    popl %edx

    # Calculate the index and store in ebx
    pushl %eax
    movl  %edx  , %eax
    shrl  $0xC  , %eax  # discard non-significant bits
    andl  $0x3FF, %eax  # discart non-relevant bits on the left side
    movl  $4    , %ecx  # each entry is 4 bytes
    mull  %ecx
    movl  %eax  , %ebx  # ebx now contains the offset into the page table
                        # for the entry
    popl  %eax

    # Fetch the target physical address that was stashed earlier
    popl %edx

    # Calculate the final location of the page table entry and insert it
    # into the page table
    addl %ebx, %eax
    orl  $0x1, %edx
    movl %edx, (%eax)

    # Clean up clobbered registers and return
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    ret

bootstrap_panic:
    cli
    hlt
