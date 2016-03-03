/* Source: http://wiki.osdev.org/User:Mduft/HigherHalf_Kernel_with_32-bit_Paging
 * TODO: comment the code! */

OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
g_kernel_boot_addr   = 0x00100000;
g_kernel_high_offset = 0xC0000000;

/* Tell where the various sections of the object files will be put in the final
 * kernel image */
SECTIONS
{
    . = g_kernel_boot_addr;

    /* Multiboot header and bootstrap .text */
    .boot :
    {
        *(.multiboot)
        */boot.o(.text)
    }

    /* Skip ahead to higher half address */
    . += g_kernel_high_offset;
    g_kernel_start = .;

    /* Multiboot header and .text */
    .text ALIGN(0x1000) : AT(ADDR(.text) - g_kernel_high_offset)
    {
        g_kernel_code = .;
        *(EXCLUDE_FILE(*/boot.o) .text)

        *(.rodata*)
    }

    /* Read-write data (initialized) */
    .data ALIGN(0x1000) : AT(ADDR(.data) - g_kernel_high_offset)
    {
        g_kernel_data = .;
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    /*.bss BLOCK(0x1000) : ALIGN(0x1000)*/
    .bss ALIGN(0x1000) : AT(ADDR(.bss) - g_kernel_high_offset)
    {
        g_bss_start = . ;
        
        *(COMMON)
        *(.bss)

        /* Align/pad to the nearest 4K page boundary */
        /*. = ALIGN(0x1000);*/
        g_bss_end = . ;
    }

    g_kernel_end = . ;
}
