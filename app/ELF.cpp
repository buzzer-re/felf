#include "ELF.h"



ELF::ELF(const std::string& fPath)
{
	
	this->mappedFile = this->map_file(fPath);

	if (mappedFile == nullptr) {
		this->validELF = false;
		return;
	}

	elfHeader = (Elf64_Ehdr*) mappedFile;
	
	int e_ident_s = sizeof(elfHeader->e_ident)/sizeof(unsigned char);

	this->validELF = this->elf_magic == *(uint32_t*)&elfHeader->e_ident 
					 && e_ident_s == EI_NIDENT;	

}


ELF::~ELF()
{
	if (this->mappedFile != nullptr)
		munmap(this->mappedFile, this->fileSize);
}


void* ELF::map_file(const std::string& file)
{
	const char* file_c = file.c_str();
	
	struct stat st;
	stat(file_c, &st);
	this->fileSize = st.st_size;

	int fd = open(file_c, O_RDONLY);
	if (!fd) return nullptr;

	void* mapped_file = mmap(NULL, fileSize, PROT_READ, MAP_SHARED, fd, 0);

	return mapped_file;
}


/**
COMMENT
**/
void ELF::displayHeader() const
{
	std::cout << "Header:\n";
	std::cout << "\tMagic:\t";

	for (int i = 0; i < EI_NIDENT; ++i) {
		auto iter = HEADER_MAP_VALUES.find(i);

		if (iter != HEADER_MAP_VALUES.end()) {
			auto valueIter = iter->second.find(this->elfHeader->e_ident[i]);
			if (valueIter != iter->second.end()) {
				std::cout << valueIter->second << std::endl;
			}
		}
	}

	std::cout << std::endl;
}
