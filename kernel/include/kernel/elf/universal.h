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

#ifndef __TREEOS_KERNEL_ELF_UNIVERSAL_H__
#define __TREEOS_KERNEL_ELF_UNIVERSAL_H__

// This file contains architecture-agnostic ELF constants and functions

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#define EI_NIDENT 16 // Size of e_ident
#define EI_NMAGIC  4 // Size of magic within e_ident

// Indices into the e_ident member of the ELF header
enum Elf_Ident
{
    EI_MAG0       = 0, // 0x7F
    EI_MAG1       = 1, // 'E'
    EI_MAG2       = 2, // 'L'
    EI_MAG3       = 3, // 'F'
    EI_CLASS      = 4, // Architecture (32/64)
    EI_DATA       = 5, // Byte Order
    EI_VERSION    = 6, // ELF Version
    EI_OSABI      = 7, // OS Specific
    EI_ABIVERSION = 8, // OS Specific
    EI_PAD        = 9  // Padding
};

// Desired magic values for e_ident
enum Elf_Magic
{
    ELFMAG0 = 0x7F, // e_ident[EI_MAG0]
    ELFMAG1 = 'E',  // e_ident[EI_MAG1]
    ELFMAG2 = 'L',  // e_ident[EI_MAG2]
    ELFMAG3 = 'F'   // e_ident[EI_MAG3]
};

// Class values
enum Elf_Class
{
    ELFCLASSNONE = 0, // No class specified
    ELFCLASS32   = 1, // 32 bit
    ELFCLASS64   = 2  // 64 bit
};

// Data byte order values
enum Elf_Data
{
    ELFDATANONE = 0, // No ordering specified/invalid
    ELFDATA2LSB = 1, // 2's complement, little endian (x86 is this)
    ELFDATA2MSB = 2, // 2's complement, big endian
};

// ELF specification version
enum Elf_Version
{
    EV_NONE    = 0, // Unknown/invalid version
    EV_CURRENT = 1  // Current (only) version
};

// Object file type
enum Elf_Type
{
    ET_NONE   = 0,      // Unknown/no file type
    ET_REL    = 1,      // Relocatable
    ET_EXEC   = 2,      // Executable
    ET_DYN    = 3,      // Shared object
    ET_CORE   = 4,      // Core
    //ET_LOPROC = 0xFF00, // Processor-specific
    //ET_HIPROC = 0xFFFF  // Processor-specific
};

// Machine architecture type
enum Elf_Machine
{
    EM_NONE  = 0, // No architecture specified
    EM_386   = 3, // Intel x86 (the only one we care about)
};

// Segment types
enum Elf_Segment
{
    PT_NULL    = 0,          // Unused (ignored)
    PT_LOAD    = 1,          // Loadable
    PT_DYNAMIC = 2,          // Dynamic linking information
    PT_INTERP  = 3,          // Interpreter pathname
    PT_NOTE    = 4,          // Auxilliary information
    PT_SHLIB   = 5,          // Reserved
    PT_PHDR    = 6,          // Program header table
    //PT_LOOS    = 0x60000000, // OS specific
    //PT_HIOS    = 0x6FFFFFFF, // OS specific
    //PT_LOPROC  = 0x70000000, // Processor specific
    //PT_HIPROC  = 0x7FFFFFFF  // Processor specific
};

// Segment flag bits
enum Elf_Segment_Flag
{
    PF_X        = 1,          // Execute
    PF_W        = 2,          // Write
    PF_R        = 4,          // Read
    //PF_MASKOS   = 0x0FF00000, // Unspecified
    //PF_MASKPROC = 0xF0000000  // Unspecified
};

#endif
