/* Source: http://wiki.osdev.org/User:Mduft/HigherHalf_Kernel_with_32-bit_Paging
 * TODO: comment the code! */

OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
KERNEL_BOOT_VMA = 0x00100000;
KERNEL_HIGH_VMA = 0xC0000000;

/*KERNEL_HEAP_SIZE = 0x200000;*/

/* Tell where the various sections of the object files will be put in the final
 * kernel image */
SECTIONS
{
    . = KERNEL_BOOT_VMA;

    /* Multiboot header and bootstrap .text */
    .boot :
    {
        *(.multiboot)
        */boot.o(.text)
    }

    /* Skip ahead to higher half address */
    . += KERNEL_HIGH_VMA;
    g_kernel_start = .;

    /* Multiboot header and .text */
    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_HIGH_VMA)
    {
        g_kernel_code = .;
        *(EXCLUDE_FILE(*/boot.o) .text)

        *(.rodata*)
    }

    /* Read-write data (initialized) */
    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_HIGH_VMA)
    {
        g_kernel_data = .;
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    /*.bss BLOCK(0x1000) : ALIGN(0x1000)*/
    .bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_HIGH_VMA)
    {
        g_bss_start = . ;
        
        *(COMMON)
        *(.bss)

        /* Align/pad to the nearest 4K page boundary */
        /*. = ALIGN(0x1000);*/
        g_bss_end = . ;
    }

    /* Page alignment is guaranteed for this symbol */
    /*g_placement_heap_start = .;*/

    . += KERNEL_HEAP_SIZE; /* DEBUG - to add more leeway for initial kernel stuff */

    /*g_kernel_end = . ;*/
}
