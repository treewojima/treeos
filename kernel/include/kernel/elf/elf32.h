#ifndef __TREEOS_KERNEL_ELF_ELF32_H__
#define __TREEOS_KERNEL_ELF_ELF32_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <kernel/elf/universal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t Elf32_Half;	// Unsigned half int
typedef uint32_t Elf32_Off;	    // Unsigned offset
typedef uint32_t Elf32_Addr;	// Unsigned address
typedef uint32_t Elf32_Word;	// Unsigned int
typedef int32_t  Elf32_Sword;	// Signed int

// Main ELF header
struct Elf32_Ehdr
{
    uint8_t       e_ident[EI_NIDENT]; // Magic number and other info
    Elf32_Half    e_type;             // Object file type
    Elf32_Half    e_machine;          // Architecture
    Elf32_Word    e_version;          // Object file version
    Elf32_Addr    e_entry;            // Virtual address of entry point
    Elf32_Off     e_phoff;            // Offset of program header table file
    Elf32_Off     e_shoff;            // Offset of section header table file
    Elf32_Word    e_flags;            // Offset of processor-specific flags
    Elf32_Half    e_ehsize;           // ELF header size in bytes (should be 52)
    Elf32_Half    e_phentsize;        // Program header table entry size
    Elf32_Half    e_phnum;            // Program header table entry count
    Elf32_Half    e_shentsize;        // Section header table entry size
    Elf32_Half    e_shnum;            // Section header table entry count
    Elf32_Half    e_shstrndx;         // Section header string table index
};

// Program header
struct Elf32_Phdr
{
    Elf32_Word p_type;   // Type of segment
    Elf32_Off  p_offset; // Offset from beginning of file
    Elf32_Addr p_vaddr;  // Virtual address of segment
    Elf32_Addr p_paddr;  // Physical address of segment (unused for SysV ABI)
    Elf32_Word p_filesz; // Number of bytes in file image (allowed to be zero)
    Elf32_Word p_memsz;  // Number of bytes in memory image (allowed to be zero)
    Elf32_Word p_flags;  // Flags
    Elf32_Word p_align;  // Alignment (0/1 mean no alignment, otherwise power of 2)
};

// Expected size of ELF32 header
#define ELF32_HEADER_SIZE 52

bool elf32_validate(struct Elf32_Ehdr *hdr);

#endif
