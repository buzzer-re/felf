#pragma once

#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <unordered_map>
#include <utility>
#include <string>
#include <cstdio>

#include "utils.h"


typedef std::unordered_map<uint64_t, const char*> HEADER_MAP_VALUE_TO_STRING;
typedef std::unordered_map<uint64_t, HEADER_MAP_VALUE_TO_STRING> HEADER_MAP_BYTE_TO_MAP;

static const HEADER_MAP_BYTE_TO_MAP HEADER_MAP_VALUES = {
	{EI_CLASS,
		{
			{ELFCLASSNONE,	"Invalid ELF class"},
			{ELFCLASS32,	"ELF32"},
			{ELFCLASS64,	"ELF64"}
		}
	},
	{EI_DATA,
		{
			{ELFDATANONE,	"Unknown data format"},
			{ELFDATA2LSB,	"Two's complement, little-endian"},
			{ELFDATA2MSB,	"Two's complement, big-endian"}
		}
	},
	{EI_VERSION,
		{
			{EV_NONE,	"Invalid Version"},
			{EV_CURRENT,"Current Version"}
		}
	},
	{EI_OSABI,
		{
			{ELFOSABI_NONE,		"UNIX System V ABI"},
			{ELFOSABI_SYSV,		"Unix System V ABI"},
			{ELFOSABI_HPUX,		"HP-UX ABI"},
			{ELFOSABI_NETBSD,	"NetBSD ABI"},
			{ELFOSABI_LINUX,	"Linux ABI"},
			{ELFOSABI_SOLARIS,	"Solaris ABI"},
			{ELFOSABI_IRIX,		"IRIX ABI"},
			{ELFOSABI_FREEBSD,	"FreeBSD ABI"},
			{ELFOSABI_TRU64,	"TRU64 UNIX ABI"},
			{ELFOSABI_ARM,		"ARM architecture ABI"},
			{ELFOSABI_STANDALONE,"Stand-alone (embedded) ABI"}
		}
	},
};


struct SectionHeaderTable {
	Elf64_Shdr* section_head;
	uint16_t length;
	uint16_t size;
	std::unordered_map<std::string , Elf64_Shdr*> sectionsMapped;
	std::unordered_map<std::string , Elf64_Shdr*>::const_iterator sectionsMappedIter;
};

struct SymbolTable {
	Elf64_Sym* symbol_head;
	Elf64_Shdr* section;
	uint16_t size;
	uint16_t length;
	std::unordered_map<std::string , Elf64_Sym*> symbolsMapped;
	std::unordered_map<std::string , Elf64_Sym*>::const_iterator symbolsMappedIter;
};

class ELF {
public:
	ELF(const std::string& fPath);
	~ELF();
	
	bool valid() const { return this->validELF; };

	void displayHeader() const;

private:
	void* map_file(const std::string& file);
	void build_elf();

	std::string getNameFromStringTable(uint64_t index) const;
	std::string getNameFromSymbolStringTable(uint64_t index) const;

private:
	bool validELF;
	void* mappedFile;
	off_t fileSize;

#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
	uint32_t elf_magic = 0x464c457f;
#else
	uint32_t elf_magic = 0x7f454c46;
#endif 

	// Elf structs
	Elf64_Ehdr* elfHeader;
	Elf64_Shdr* stringSectionHdr;
	Elf64_Shdr* stringSymbolTable;

	SectionHeaderTable elfSection;
	SymbolTable symbolTable;
	
};
