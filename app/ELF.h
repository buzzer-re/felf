#pragma once

#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <cstdio>

class ELF {
public:
	ELF(const std::string& fPath);
	~ELF();
	
	bool valid() const { return this->validELF; };

private:
	void* map_file(const std::string& file);

private:
	bool validELF;
	void* mappedFile;
	off_t fileSize;


	Elf64_Ehdr* elfHeader;
};
