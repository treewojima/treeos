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

#include <kernel/elf/elf32.h>
#include <kernel/debug.h>
#include <string.h>

static bool validate_magic(struct Elf32_Ehdr *hdr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool elf32_validate(struct Elf32_Ehdr *hdr)
{
    KASSERT(hdr);

    // Valid ELF header?
    if (!validate_magic(hdr))
    {
        worry("invalid ELF32 file");
        return false;
    }

    // 32 bit?
    if (hdr->e_ident[EI_CLASS] != ELFCLASS32)
    {
        worry("unsupported ELF file class");
        return false;
    }

    // Little endian?
    if (hdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        worry("unsupported ELF file byte order");
        return false;
    }

    // Valid platform?
    if (hdr->e_machine != EM_386)
    {
        worry("unsupported ELF file target");
        return false;
    }

    // Correct ELF specification version?
    if (hdr->e_ident[EI_VERSION] != EV_CURRENT)
    {
        worry("unsupported ELF file version");
        return false;
    }

    // Executable type? (subject to change later as more types are supported)
    if (hdr->e_type != ET_EXEC)
    {
        worry("unsupported ELF file type");
        return false;
    }

    return true;
}

bool elf32_load(struct Elf32_Ehdr *hdr, struct process *proc)
{
    KASSERT(hdr);
    KASSERT(proc);
    KASSERT(proc->pgdir);

    // First, loop through the program headers to validate and calculate
    // the base address
    Elf32_Word base = -1;
    for (uint32_t i = 0; i < hdr->e_phnum; i++)
    {
        struct Elf32_Phdr *prog_hdr = (struct Elf32_Phdr *)
                ((uint32_t)hdr + hdr->e_phoff + i * hdr->e_phentsize);

        if (prog_hdr->p_type != PT_LOAD) continue;

        // Base address is the smallest p_vaddr entry
        if (prog_hdr->p_vaddr < base) base = prog_hdr->p_vaddr;
    }

    // Loop through them again to calculate the amount of memory required
    Elf32_Word size = 0;
    for (uint32_t i = 0; i < hdr->e_phnum; i++)
    {
        struct Elf32_Phdr *prog_hdr = (struct Elf32_Phdr *)
                ((uint32_t)hdr + hdr->e_phoff + i * hdr->e_phentsize);

        if (prog_hdr->p_type != PT_LOAD) continue;

        // Size is the largest p_vaddr entry plus p_memsz
        Elf32_Word s = prog_hdr->p_paddr - base + prog_hdr->p_memsz;
        if (s > size) size = s;
    }

    //printf("init base: %x\n", base);
    //printf("init size: %x\n", size);

    // TODO: finish this function
    return true;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static bool validate_magic(struct Elf32_Ehdr *hdr)
{
    KASSERT(hdr);
#if 0
    KASSERT(hdr->e_ident[EI_MAG0] == ELFMAG0);
    KASSERT(hdr->e_ident[EI_MAG1] == ELFMAG1);
    KASSERT(hdr->e_ident[EI_MAG2] == ELFMAG2);
    KASSERT(hdr->e_ident[EI_MAG3] == ELFMAG3);
#else
    static const uint8_t elf_magic[EI_NMAGIC] =
        { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3 };

    if (memcmp(hdr->e_ident, elf_magic, EI_NMAGIC))
    {
        worry("invalid ELF magic for file");
        return false;
    }
#endif

    return true;
}
