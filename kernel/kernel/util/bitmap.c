#include <kernel/util/bitmap.h>

#include <kernel/debug.h>
#include <stdlib.h>
#include <string.h>

struct bitmap *bitmap_alloc(struct bitmap *bitmap, unsigned num_bits)
{   
    unsigned num_words = num_bits / BITS_PER_WORD;
    if (num_bits % BITS_PER_WORD) num_words++;

    if (!bitmap)
    {
        bitmap = kcalloc(1, sizeof(struct bitmap));
        bitmap->flags |= BITMAP_FREE_STRUCT;
    }
    bitmap->word_count = num_words;
    bitmap->bits = kcalloc(num_words, sizeof(*bitmap->bits));

    return bitmap;
}

void bitmap_free(struct bitmap *bitmap)
{
    if (!bitmap)
    {
        worry("freeing NULL bitmap");
        return;
    }

    if (bitmap->flags & BITMAP_FREE_NOTHING)
    {
        worry("tried to free un-freeable bitmap");
        return;
    }

    WORRY_IF(!bitmap->bits, "freeing bitmap with null bits");
    kfree(bitmap->bits);

    if (bitmap->flags & BITMAP_FREE_STRUCT)
    {
        kfree(bitmap);
    }
    else
    {
        memset(bitmap, 0, sizeof(*bitmap));
    }
}

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
