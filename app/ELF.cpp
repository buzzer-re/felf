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
	
	if (this->validELF) {
		this->build_elf();
	}

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
	close(fd);

	return mapped_file;	
}

/**
 * 
 * Build elf file structs
 */
void ELF::build_elf()
{	
	/// Section array	
	this->elfSection.length  = this->elfHeader->e_shnum;

	/// This point to first section table array element
	this->elfSection.section_head = (Elf64_Shdr*) ( (uint64_t)this->mappedFile + this->elfHeader->e_shoff);
	this->elfSection.size    = this->elfHeader->e_shentsize;
	this->stringSectionHdr = (Elf64_Shdr*) ((uint64_t) this->elfSection.section_head + ((uint64_t) this->elfSection.size * this->elfHeader->e_shstrndx));
	
	// Elf64_Shdr* head;
	// for (int i = 0; i < this->elfSection.length; ++i) {
	// 	head = (Elf64_Shdr*) ((uint64_t)this->elfSection.section_head + (i*this->elfSection.size));
	// 	std::printf("%s\n", (char*) ((uint64_t) this->stringSectionHdr->sh_offset + (uint64_t) this->mappedFile) + head->sh_name);

	// }

}

/**
COMMENT
**/
void ELF::displayHeader() const
{
	std::cout << "Header:\n";

	HEADER_MAP_VALUE_TO_STRING::const_iterator valueIter;
	HEADER_MAP_BYTE_TO_MAP::const_iterator iter;
	std::string valueOutput;
	unsigned char byteat;

	/// Magic Number parser
	for (int i = 0; i < EI_NIDENT; ++i) {
		iter = HEADER_MAP_VALUES.find(i);
		byteat = this->elfHeader->e_ident[i];

		if (iter != HEADER_MAP_VALUES.end()) {
			valueIter = iter->second.find(byteat);
			if (valueIter != iter->second.end()) {
				// std::printf("%s (0x%x)\n", valueIter->second, byteat);
			}
		} else {
			std::printf("0x%x ", SHRINK_ASCII(byteat), byteat);
		}
	}

	std::cout << std::endl;
}
