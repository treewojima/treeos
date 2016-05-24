/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
