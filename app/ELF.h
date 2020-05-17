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


// struct Output {
// 	std::string raw_output;
// 	// std::string json_output;
// 	// std::string yaml_output;
// };


static const std::unordered_map<uint64_t, std::unordered_map<uint64_t, const char*>> HEADER_MAP_VALUES = {
	{EI_CLASS,
		{
			{ELFCLASSNONE,	"Invalid ELF class"},
			{ELFCLASS32,	"ELF32"},
			{ELFCLASS64,	"ELF64"}
		}
	}
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
