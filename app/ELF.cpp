#include "ELF.h"



ELF::ELF(const std::string& fPath, int mode)
{
	
	this->mappedFile = this->map_file(fPath, mode);

	if (mappedFile == nullptr) {
		std::cerr << strerror(errno) << std::endl;
		this->validELF = false;
		return;
	}

	this->elfHeader = (Elf64_Ehdr*) mappedFile;
	
	int e_ident_s = sizeof(this->elfHeader->e_ident)/sizeof(unsigned char);
	this->validELF = this->elf_magic == *(uint32_t*)&elfHeader->e_ident 
					 && e_ident_s == EI_NIDENT;
	
	if (this->validELF) {
		this->build_quick_elf();
	}

}


ELF::~ELF()
{
	if (this->mappedFile != nullptr) {
		munmap(this->mappedFile, this->fileSize);

		for (auto it = this->symbolTable.symbolDataMapped.begin(); 
			it != this->symbolTable.symbolDataMapped.end(); 
			++it)	
			delete it->second;
		
		for (auto it = this->elfSection.sectionData.begin();
			it != this->elfSection.sectionData.end();
			++it)
			delete it->second;
	}
}


void* ELF::map_file(const std::string& file, int mode)
{
	const char* file_c = file.c_str();
	
	struct stat st;
	stat(file_c, &st);
	this->fileSize = st.st_size;

	int fd = open(file_c, mode - 1);
	if ( fd < 0 ) return nullptr;

	void* mapped_file = mmap(NULL, fileSize, mode, MAP_SHARED, fd, 0);

	close(fd);

	return mapped_file;	
}

/**
 *	Save mapped file back to disk 
 * 
 */
bool ELF::save(const std::string& output) const 
{
	if (this->mappedFile != nullptr && this->validELF) {
		std::ofstream outputElf(output.c_str(), std::ios::out | std::ios::binary);
		bool good;

		if (!outputElf) return false;

		outputElf.write((char*) this->mappedFile, this->fileSize);

		good = outputElf.good();
		outputElf.close();

		if (good) {
			chmod(output.c_str(), S_IRWXU | S_IRGRP | S_IXGRP |S_IROTH | S_IXOTH); // 755
		}

		return good;
	}

	return false;
}

/**
 * 
 * Build elf file structs
 */
void ELF::build_quick_elf()
{	
	Elf64_Shdr* section;
	Elf64_Sym* symbol;
	void* symbolData;
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
	this->elfSection.sectionArray.reserve(this->elfSection.length);
	/// Building section map

	/// Map all sections in a map
	for (auto i = 0; i < this->elfSection.length; ++i) {
		section = (Elf64_Shdr*) ((uint64_t)this->elfSection.section_head + (i*this->elfSection.size));
		sectionName = this->getNameFromStringTable(section->sh_name);
		this->elfSection.sectionsMapped.insert(std::make_pair(sectionName,  section));
		this->elfSection.sectionArray.push_back(section);

		SectionData* sectionData = new SectionData;
		sectionData->size = section->sh_size;
		sectionData->section = section;
		sectionData->data = (unsigned char*) ((uint64_t) section->sh_offset + (uint64_t) this->mappedFile);

		this->elfSection.sectionData.insert(std::make_pair(sectionName, sectionData));
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
			SymbolData* symbolData = new SymbolData;
			
			symbolData->size = symbol->st_size;
			if (symbol->st_shndx <= this->elfSection.sectionArray.size()) {
				symbolData->data = (unsigned char*) ( (uint64_t)this->mappedFile + 
													  (uint64_t) symbol->st_value);
			}

			this->symbolTable.symbolsMapped.insert(std::make_pair(symbolName, symbol));
			this->symbolTable.symbolDataMapped.insert(std::make_pair(symbolName, symbolData));
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


void ELF::incFileSize(uint value)  
{
	this->fileSize += value;
}

void ELF::decFileSize(uint value)
{
	this->fileSize -= value;
}

