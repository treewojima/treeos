/* Source: http://wiki.osdev.org/User:Mduft/HigherHalf_Kernel_with_32-bit_Paging
 * TODO: comment the code! */

OUTPUT_FORMAT(elf32-i386)
ENTRY(main)
KERNEL_BOOT_VMA = 0x00100000;

/* Tell where the various sections of the object files will be put in the final
 * kernel image */
SECTIONS
{
    . = KERNEL_BOOT_VMA;

    /* Multiboot header and .text */
    .text : ALIGN(0x1000)
    {
        *(.text)
        *(.rodata*)
    }

    /* Read-write data (initialized) */
    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    /*.bss BLOCK(0x1000) : ALIGN(0x1000)*/
    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }
}
