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

.section .text

// Flushes/updates a page in the TLB
// Parameters:
//     uint32_t - pointer to virtual address
//
// Returns:
//     void
//
.global flush_tlb
.type flush_tlb, @function
flush_tlb:
    movl   4(%esp), %eax
    invlpg (%eax)
    ret

// Flushes/updates the entire TLB
//Parameters:
//     none
//
// Returns:
//     void
//
.global flush_tlb_full
.type flush_tlb_full, @function
flush_tlb_full:
    movl %cr3, %eax
    movl %eax, %cr3
    ret
