#include <kernel/mm.h>

#include <kernel/debug.h>
#include <kernel/panic.h>
#include <string.h>

#define BITS_PER_BITMAP (8 * sizeof(*frame_bitmap))

static uint32_t frame_count, *frame_bitmap;

static void set_bit(uint32_t bit);
static void clear_bit(uint32_t bit);
static bool test_bit(uint32_t bit);
static uint32_t first_free(void);

uint32_t mm_init_frame_bitmap(uint32_t mem_size)
{    
    frame_count = mem_size / PAGE_SIZE;
    if (mem_size % PAGE_SIZE) frame_count++;

    uint32_t num_elements = frame_count / BITS_PER_BITMAP;
    if (frame_count % BITS_PER_BITMAP) num_elements++;

    frame_bitmap = mm_placement_alloc(num_elements, false);

    // Set all of memory as "in use" by default
    //uint32_t bytes_allocated =
    //        frame_count / (BITS_PER_BITMAP / sizeof(*frame_bitmap));
    uint32_t bytes_allocated = num_elements * sizeof(*frame_bitmap);
    memset(frame_bitmap, 0xFF, bytes_allocated);

    return bytes_allocated;
}

void mm_init_region(uint32_t base, uint32_t size)
{
    base /= PAGE_SIZE;
    size /= PAGE_SIZE;

    while ((int)size >= 0)
    {
        clear_bit(base++);
        size--;
    }

    // Ensure that allocations can't return zero
    set_bit(0);
}

void mm_deinit_region(uint32_t base, uint32_t size)
{
    base /= PAGE_SIZE;
    size /= PAGE_SIZE;

    while ((int)size >= 0)
    {
        set_bit(base++);
        size--;
    }
}

struct page_table_entry *mm_alloc_frame(struct page_table_entry *frame)
{
    KASSERT(frame);

    uint32_t address = first_free();
    if (address == (uint32_t)-1)
    {
        return NULL;
    }

    set_bit(address);
    address *= PAGE_SIZE;

    frame->address = address >> 12;
    return frame;
}

void mm_free_frame(struct page_table_entry *frame)
{
    KASSERT(frame);

    uint32_t bit = (frame->address << 12) / PAGE_SIZE;
    KASSERT_WORRY(!test_bit(bit));
    clear_bit(bit);
}

static void set_bit(uint32_t bit)
{
    frame_bitmap[bit / BITS_PER_BITMAP] |= (1 << (bit % BITS_PER_BITMAP));
}

static void clear_bit(uint32_t bit)
{
    frame_bitmap[bit / BITS_PER_BITMAP] &= ~(1 << (bit % BITS_PER_BITMAP));
}

static bool test_bit(uint32_t bit)
{
    return frame_bitmap[bit / BITS_PER_BITMAP] & (1 << (bit % BITS_PER_BITMAP));
}

static uint32_t first_free(void)
{
    for (uint32_t i = 0; i < frame_count / BITS_PER_BITMAP; i++)
    {
        if (frame_bitmap[i] != (uint32_t)-1)
        {
            for (uint32_t j = 0; j < BITS_PER_BITMAP; j++)
            {
                if (!(frame_bitmap[i] & (1 << j)))
                {
                    return i * BITS_PER_BITMAP + j;
                }
            }
        }
    }

    return (uint32_t)-1;
}
