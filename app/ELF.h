#pragma once

#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdio>

#include "utils.h"

// struct Output {
// 	std::string raw_output;
// 	// std::string json_output;
// 	// std::string yaml_output;
// };

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

class ELF {
public:
	ELF(const std::string& fPath);
	~ELF();
	
	bool valid() const { return this->validELF; };

	void displayHeader() const;

private:
	void* map_file(const std::string& file);

private:
	bool validELF;
	void* mappedFile;
	off_t fileSize;

#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
	uint32_t elf_magic = 0x464c457f;
#else
	uint32_t elf_magic = 0x7f454c46;
#endif 

	Elf64_Ehdr* elfHeader;	
};
