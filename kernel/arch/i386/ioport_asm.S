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

// Read a byte in from an I/O port
// Parameters:
//     uint16_t - port
//
// Returns:
//     uint8_t
//
.global asm_ioport_inb
.type asm_ioport_inb, @function
asm_ioport_inb:
    // Get the port parameter to read from
    movw 4(%esp), %dx

    // Read
    xorl %eax, %eax
    inb  %dx , %al

    ret

// Write a byte to an I/O port
// Parameters:
//     uint16_t - port
//     uint8_t - data
//
// Returns:
//     void
//
.global asm_ioport_outb
.type asm_ioport_outb, @function
asm_ioport_outb:
    // Get port and data parameters
    movw  4(%esp), %dx
    movb  8(%esp), %al

    // Write
    outb %al, %dx

    ret

// Read a word in from an I/O port
// Parameters:
//     uint16_t - port
//
// Returns:
//     uint16_t
//
.global asm_ioport_inw
.type asm_ioport_inw, @function
asm_ioport_inw:
    // Get the port parameter to read from
    movw 4(%esp), %dx

    // Read
    xorl %eax, %eax
    inw  %dx , %ax

    ret

// Write a word to an I/O port
// Parameters:
//     uint16_t - port
//     uint16_t - data
//
// Returns:
//     void
//
.global asm_ioport_outw
.type asm_ioport_outw, @function
asm_ioport_outw:
    // Get port and data parameters
    movw  4(%esp), %dx
    movw  8(%esp), %ax

    // Write
    outw %ax, %dx

    ret

// Read a long in from an I/O port
// Parameters:
//     uint16_t - port
//
// Returns:
//     uint32_t
//
.global asm_ioport_inl
.type asm_ioport_inl, @function
asm_ioport_inl:
    // Get the port parameter to read from
    movw 4(%esp), %dx

    // Read
    //xorl %eax, %eax
    inl  %dx , %eax

    ret

// Write a long to an I/O port
// Parameters:
//     uint16_t - port
//     uint32_t - data
//
// Returns:
//     void
//
.global asm_ioport_outl
.type asm_ioport_outl, @function
asm_ioport_outl:
    // Get port and data parameters
    movw  4(%esp), %dx
    movl  8(%esp), %eax

    // Write
    outl %eax, %dx

    ret
