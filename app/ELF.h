 #pragma once

#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <string>
#include <cstdio>

#include <errno.h>
#include <string.h>

#include "utils.h"

#define MAP_RW PROT_WRITE | PROT_READ
#define MAP_RO PROT_READ
#define MAP_EX PROT_READ | PROT_EXEC

struct SectionHeaderTable {
	Elf64_Shdr* section_head;
	uint16_t length;
	uint16_t size;
	std::vector<Elf64_Shdr*> sectionArray;
	std::unordered_map<std::string , Elf64_Shdr*> sectionsMapped;
	std::unordered_map<std::string , Elf64_Shdr*>::const_iterator sectionsMappedIter;
};


struct SymbolData {
	Elf64_Sym* symbol;
	unsigned char* data;
	uint64_t size;
};

struct SymbolTable {
	Elf64_Sym* symbol_head;
	Elf64_Shdr* section;
	uint16_t size;
	uint16_t length;
	std::unordered_map<std::string , Elf64_Sym*> symbolsMapped;
	std::unordered_map<std::string , Elf64_Sym*>::const_iterator symbolsMappedIter;

	std::unordered_map<std::string, SymbolData*> symbolDataMapped;
	std::unordered_map<std::string , SymbolData*>::const_iterator symbolDataMappedIter;
};


struct ProgramHeaderTable {
	Elf64_Phdr* program_head;
	uint16_t size;
	uint16_t length;
	std::vector<Elf64_Phdr*> phrVector;
};

class ELF {
public:
	ELF(const std::string& fPath, int mode = MAP_RO);
	~ELF();
	
	bool valid() const { return this->validELF; };

	void displayHeader() const;

	bool save(const std::string& output) const;

private:
	void* map_file(const std::string& file, int mode);
	void build_quick_elf();

	Elf64_Section* getSectionByIndex(int index) const;

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

public:
	// Elf structs
	Elf64_Ehdr* elfHeader;
	Elf64_Shdr* stringSectionHdr;
	Elf64_Shdr* stringSymbolTable;

	SectionHeaderTable elfSection;
	SymbolTable symbolTable;
	ProgramHeaderTable phrTable;
};
