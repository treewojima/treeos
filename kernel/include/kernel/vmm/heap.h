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

#ifndef __TREEOS_KERNEL_VMM_HEAP_H__
#define __TREEOS_KERNEL_VMM_HEAP_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdbool.h>
#include <stdint.h>

// Kernel heap is placed at 3.25 GiB, with a maximum size of 1 MiB
#define KHEAP_START    0xD0000000
#define KHEAP_MAX_SIZE 0x100000

struct heap
{
    uint32_t base;
    uint32_t size;
    uint8_t flags;
};

// Kernel heap init
void kernel_heap_init(void);

// Heap manipulation
struct heap *heap_alloc(struct heap *heap, uint32_t base, uint32_t size);
void *heap_grow(struct heap *heap, uint32_t pages);
void heap_shrink(struct heap *heap, void *ptr, uint32_t pages);

#endif
