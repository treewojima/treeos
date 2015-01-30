#include <kernel/multiboot.h>

// Global storage for the multiboot information structure, to be copied from
// whatever location GRUB sticks it in and stored within kernel data space
// NOTE: This is defined as const to enforce the C compiler preventing changes
//       to it, but it is initialized by casting away the const-ness in kinit().
//       A bit of an icky hack, but it works.
const struct multiboot_info g_multiboot_info;
