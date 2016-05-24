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

    // x86?
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
