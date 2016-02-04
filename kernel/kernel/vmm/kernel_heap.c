#include <kernel/vmm/heap.h>

#include <kernel/debug.h>
#include <kernel/multiboot.h>
#include <string.h>

// Kernel heap is placed at 3.25 GiB, with a maximum size of 1 MiB
#define KHEAP_START    0xD0000000
#define KHEAP_MAX_SIZE 0x100000

// Kernel heap structure
static struct heap kheap;

void kernel_heap_init(void)
{
    PANIC_IF(!heap_alloc(&kheap, DEFAULT_KHEAP_START, PAGE_SIZE),
             "failed to create kernel heap");
}

int liballoc_lock(void)
{
    // Just disable interrupts - this will need to be fixed with a proper
    // synchronization method later
    if (int_enabled())
    {
        enable_int = true;
        int_disable();
    }
    else
    {
        enable_int = false;
    }

    return 0;
}

int liballoc_unlock(void)
{
    // Just re-enable interrupts if necessary - again, needs to be fixed later
    if (enable_int)
    {
        int_enable();
    }

    return 0;
}

void *liballoc_alloc(size_t pages)
{
    // NOTE: This manually manipulates the heap for now, which is a bad idea

    if (!pages) return NULL;

    kheap.size += pages << PAGE_SIZE_SHIFT;
    PANIC_IF(kheap.base + kheap.size > KHEAP_MAX_SIZE,
             "maximum kernel heap size exceeded");

    // How does this work, if this function must return a void pointer to the
    // new address block, yet the heap doesn't keep track of the current address?
}
