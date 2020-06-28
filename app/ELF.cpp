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
	Elf64_Shdr* section;
	Elf64_Sym* symbol;
	Elf64_Phdr* phr;
	std::string sectionName;
	

	/// Mapping program header table
	if (elfHeader->e_phoff) {
		this->phrTable.length = elfHeader->e_phnum;
		this->phrTable.size = elfHeader->e_phentsize;
		this->phrTable.program_head = (Elf64_Phdr*) ( (uint64_t) elfHeader->e_phoff + (uint64_t) this->mappedFile);
		this->phrTable.phrVector.reserve(this->phrTable.size);

		for (auto i = 0; i < phrTable.length; ++i) {
			phr = (Elf64_Phdr*) ( (uint64_t) this->phrTable.program_head + (i*this->phrTable.size));
			this->phrTable.phrVector.push_back(phr);
		}
	}
	/// Ending mapping program header table
	
	/// Section array	
	this->elfSection.length  = this->elfHeader->e_shnum;
	this->elfSection.section_head = (Elf64_Shdr*) ( (uint64_t)this->mappedFile + this->elfHeader->e_shoff);
	this->elfSection.size    = this->elfHeader->e_shentsize;
	this->stringSectionHdr = (Elf64_Shdr*) ((uint64_t) this->elfSection.section_head + ((uint64_t) this->elfSection.size * this->elfHeader->e_shstrndx));
	this->elfSection.sectionsMapped.reserve(this->elfSection.length);
	/// Building section map

	/// Map all sections in a map
	for (auto i = 0; i < this->elfSection.length; ++i) {
		section = (Elf64_Shdr*) ((uint64_t)this->elfSection.section_head + (i*this->elfSection.size));
		sectionName = this->getNameFromStringTable(section->sh_name);
		this->elfSection.sectionsMapped.insert(std::make_pair(sectionName,  section));
	}

	/// End section map
	
	/// Map symtab
	this->elfSection.sectionsMappedIter = this->elfSection.sectionsMapped.find(".symtab");
	if (this->elfSection.sectionsMappedIter != this->elfSection.sectionsMapped.end()) { // Found symbol table
		this->stringSymbolTable = this->elfSection.sectionsMapped.find(".strtab")->second;
		this->symbolTable.section = this->elfSection.sectionsMappedIter->second;
		this->symbolTable.symbol_head = (Elf64_Sym*) ((uint64_t) this->symbolTable.section->sh_offset + (uint64_t) this->mappedFile);
		this->symbolTable.size = sizeof(Elf64_Sym); 
		this->symbolTable.length = this->elfSection.sectionsMappedIter->second->sh_size/sizeof(Elf64_Sym);
		this->symbolTable.symbolsMapped.reserve(this->symbolTable.length);

		std::string symbolName;
		for (auto i = 0; i < this->symbolTable.length; ++i) {
			symbol = (Elf64_Sym*) ((uint64_t) this->symbolTable.symbol_head + (i * this->symbolTable.size));
			symbolName = this->getNameFromSymbolStringTable(symbol->st_name);
			this->symbolTable.symbolsMapped.insert(std::make_pair(symbolName, symbol));
		}
	}


}

/**
 * Extract name from string table
 */
std::string ELF::getNameFromStringTable(uint64_t index) const
{
	std::string name = (char*) ( ((uint64_t) this->stringSectionHdr->sh_offset + (uint64_t) this->mappedFile) + index);

	return name;
}

/*
	Extract name from symbol string table
*/

std::string ELF::getNameFromSymbolStringTable(uint64_t index) const
{

	std::string name = (char*) ( ((uint64_t) this->stringSymbolTable->sh_offset + (uint64_t) this->mappedFile) + index);

	return name;
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
