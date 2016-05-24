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

#include <kernel/multiboot.h>

// Global storage for the multiboot information structure, to be copied from
// whatever location GRUB sticks it in and stored within kernel data space
// NOTE: This is defined as const to enforce the C compiler preventing changes
//       to it, but it is initialized by casting away the const-ness in kinit().
//       A bit of an icky hack, but it works.
const struct multiboot_info g_multiboot_info;
