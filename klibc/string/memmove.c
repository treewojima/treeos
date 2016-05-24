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

#include <stdint.h>
#include <string.h>

void *memmove(void *dest, const void *src, size_t size)
{
    uint8_t *destc = (uint8_t *)dest;
    const uint8_t *srcc = (const uint8_t *)src;

    if (destc < srcc)
    {
        for (size_t i = 0; i < size; i++)
        {
            destc[i] = srcc[i];
        }
    }
    else
    {
        for (size_t i = 0; i != 0; i++)
        {
            destc[i - 1] = srcc[i - 1];
        }
    }

    return dest;
}
