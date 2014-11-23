#if 0

#include <kernel/mm.h>
#include <kernel/panic.h>

#define INDEX_FROM_BIT(b)  (b / 32)
#define OFFSET_FROM_BIT(b) (b % 32)

extern struct page_directory g_kernel_page_dir;

uint32_t *g_pages, g_num_pages;

static void set_page_bit(uint32_t page_addr);
static void clear_page_bit(uint32_t page_addr);
static bool test_page_bit(uint32_t page_addr);
static uint32_t find_first_free_page(void);

void mm_alloc_page(struct page *page, bool kernel, bool writeable)
{
    if (!page->frame)
    {
        uint32_t index = find_first_free_page();

        if (index == (uint32_t)-1)
        {
            panic("no free pages");
        }

        set_page_bit(index * 0x1000);
        page->present = 1;
        page->rw = writeable;
        page->user = !kernel;
        page->frame = index;
    }
}

void mm_free_page(struct page *page)
{
    if (page->frame)
    {
        clear_page_bit(page->frame);
        page->frame = 0;
    }
}

void mm_alloc_reserved_pages(void)
{
    // Update the bitmap to reflect pages already allocated
    extern uint8_t g_asm_kernel_page_table, g_asm_kernel_page_table_low;
    struct page_table *kernel_page_table[2];
    kernel_page_table[0] = (struct kernel_page_table *)&g_asm_kernel_page_table;
    kernel_page_table[1] = (struct kernel_page_table *)&g_asm_kernel_page_table_low;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 1024; j++)
        {
            if (kernel_page_table[i]->pages[j].present)
            {
                set_page_bit(kernel_page_table[i]->pages[j].frame);
            }
        }
    }
}

static void set_page_bit(uint32_t page_addr)
{
    uint32_t page_bit = page_addr / 0x1000;
    uint32_t index = INDEX_FROM_BIT(page_bit);
    uint32_t offset = OFFSET_FROM_BIT(page_bit);
    pages[index] |= (1 << offset);
}

static void clear_page_bit(uint32_t page_addr)
{
    uint32_t page_bit = page_addr / 0x1000;
    uint32_t index = INDEX_FROM_BIT(page_bit);
    uint32_t offset = OFFSET_FROM_BIT(page_bit);
    pages[index] &= ~(1 << offset);
}

static bool test_page_bit(uint32_t page_addr)
{
    uint32_t page_bit = page_addr / 0x1000;
    uint32_t index = INDEX_FROM_BIT(page_bit);
    uint32_t offset = OFFSET_FROM_BIT(page_bit);
    return (pages[index] & (1 << offset));
}

static uint32_t find_first_free_page(void)
{
    for (int i = 0; i < INDEX_FROM_BIT(num_pages); i++)
    {
        // Check for free bits in this frame
        if (pages[i] != (uint32_t)-1)
        {
            for (int j = 0; j < 32; j++)
            {
                uint32_t test_bit = 1 << j;
                if (!(pages[i] & test_bit))
                {
                    return i * 32 + j;
                }
            }
        }
    }

    return (uint32_t)-1;
}

#endif
