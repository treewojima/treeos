#include <kernel/util/bitmap.h>

#include <kernel/debug.h>
#include <kernel/mm.h>
#include <string.h>

#define MUST_FREE (1 << 0)

struct bitmap *bitmap_alloc(unsigned num_bits)
{
    unsigned num_words = num_bits / BITS_PER_WORD;
    if (num_bits % BITS_PER_WORD) num_words++;

#ifdef PLACEHOLDER_FOR_WORKING_MALLOC
    struct bitmap *bitmap = malloc(sizeof(struct bitmap));
    memset(bitmap, 0, sizeof(*bitmap));
    bitmap->bits = malloc(sizeof(*bitmap->bits) * num_words);
    memset(bitmap->bits, 0, sizeof(*bitmap->bits) * num_words);
    bitmap->flags |= MUST_FREE;
#else
    struct bitmap *bitmap = mm_placement_alloc(sizeof(struct bitmap), true, false);
    bitmap->bits = mm_placement_alloc(sizeof(*bitmap->bits) * num_words, true, false);
#endif

    bitmap->word_count = num_words;

    return bitmap;
}

#ifdef PLACEHOLDER_FOR_WORKING_MALLOC
void bitmap_free(struct bitmap *bitmap)
{
    if (!bitmap)
    {
        worry("freeing NULL bitmap");
        return;
    }

    WORRY_IF(!bitmap->bits, "freeing bitmap with null bits");
    KASSERT(bitmap->flags & MUST_FREE);

    free(bitmap->bits);
    free(bitmap);
}
#endif

unsigned bitmap_first_free(struct bitmap *bitmap)
{
    for (unsigned word = 0; word < bitmap->word_count; word++)
    {
        if (bitmap->bits[word] == (unsigned)-1)
            continue;

        for (unsigned bit = 0; bit < BITS_PER_WORD; bit++)
        {
            if (!(bitmap->bits[word] & (1 << bit)))
            {
                return word * BITS_PER_WORD + bit;
            }
        }
    }

    return (unsigned)-1;
}
