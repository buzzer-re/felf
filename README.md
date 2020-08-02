# Felf
![C/C++ CI](https://github.com/AandersonL/quickelf/workflows/C/C++%20CI/badge.svg?branch=master)

felf it's a library to parse x64 ELF files into C++ structures (unordered_map, vector and so on). 

### Why ?

Manipulating raw ELF files can be complex for some people and sometimes not worth to lose time, the goal here was to built something that give a nice way to work with this binary files by parsing their internal structures to faster data structures in C++.


### What this really do:

* Map ELF into memory
* Create the principal ELF structs on top of the mapped file
* Use maps and vectors to organize all this

All the raw files is mapped into memory and the structs are organized to the raw data itself.


### Example: Sections



```cpp
struct SectionHeaderTable {
	Elf64_Shdr* section_head;
	uint16_t length;
	uint16_t size;
	std::vector<Elf64_Shdr*> sectionArray;
	std::unordered_map<std::string , Elf64_Shdr*> sectionsMapped;
	std::unordered_map<std::string , Elf64_Shdr*>::iterator sectionsMappedIter;
};
```
The struct above show what this lib do, by parsing an ELF file this will create for you a ***SectionHeaderTable*** with the first section pointer (section_head), their lenght and size (extracted from ElfHeader) a vector of ***Elf64_Shdr**** pointers and a unordered_map with the section name as ***key*** and the ***Elf64_Shdr**** as value, also a pre-built iterator for general porpuse.


## Installing

Pre-Requisites:
* cmake
* build-tools (g++, clang...)

All the installation process uses cmake for that, so just hit:
```
./install.sh
```

This will do all the boring part.


# The structures

Here is all the structs that will be created:


### Symbol table

```cpp
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
	std::unordered_map<std::string , Elf64_Sym*>::iterator symbolsMappedIter;

	std::unordered_map<std::string, SymbolData*> symbolDataMapped;
	std::unordered_map<std::string , SymbolData*>::iterator symbolDataMappedIter;
};
```

The same idea of Section struct above, but here beside a map to the symbol struct there is a map to the symbol raw data itself (eg: Look for ***main*** data).


### Program Header table

```cpp
struct ProgramHeaderTable {
	Elf64_Phdr* program_head;
	uint16_t size;
	uint16_t length;
	std::vector<Elf64_Phdr*> phrVector;
};
```

Simple struct that hold the first program_header and the program_header table itself in a ***std::vector***


Raw structs:
```cpp
Elf64_Ehdr* elfHeader;
Elf64_Shdr* stringSectionHdr;
Elf64_Shdr* stringSymbolTable;
```

There is also the ***Elf Header*** ***String section header***  and ***Symbol string table***.



#### Pratical examples:

### Simple ELF loading

```cpp
#include <felf/ELF.h>
#include <string.h>

int main() {
	
	ELF elf("/usr/bin/ls", MAP_RO);
	
	if (elf.valid()) {	
		std::cout << "Loaded!\n";	
		std::cout << elf.elfHeader->e_ident << std::endl;	
	}
		
	
	return 0;
}
```
* MAP_RO -> Map Read only
* MAP_RW -> Map Read/Write

Compile:
```
$ g++ -o hello_elf hello.cpp -lfelf++
$ ./teste
Loaded!
ELF
```


### Symbol table hiding by corruption
[Here](app/corrupt_example.cpp) there is a example using this lib for corrupt the symbol table by making their name index in string table points to -1.

```cpp
for (elf.symbolTable.symbolsMappedIter= elf.symbolTable.symbolsMapped.begin(); 
    elf.symbolTable.symbolsMappedIter != elf.symbolTable.symbolsMapped.end(); 
    ++elf.symbolTable.symbolsMappedIter) {
    
    std::cout << "Corrupting " << elf.symbolTable.symbolsMappedIter->first << std::endl;
    elf.symbolTable.symbolsMappedIter->second->st_name = -1;
}
```
```sh
~/.../quickelf/app >>> ./corrupt --input ~/teste                   
Corrupting _ZNSt8ios_base4InitD1Ev@@GLIBCXX_3.4
Corrupting _ITM_registerTMCloneTable
Corrupting __libc_start_main@@GLIBC_2.2.5
Corrupting _ITM_deregisterTMCloneTable
Corrupting __gxx_personality_v0@@CXXABI_1.3
Corrupting _end
Corrupting __data_start
Corrupting _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_@@GLIBCXX_3.4.21
Corrupting _ZN3ELFD1Ev
Corrupting _ZSt4cout@@GLIBCXX_3.4
Corrupting __TMC_END__
Corrupting __stack_chk_fail@@GLIBC_2.4
Corrupting _ZNSaIcED1Ev@@GLIBCXX_3.4
Corrupting _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKh@@GLIBCXX_3.4
Corrupting _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@@GLIBCXX_3.4
Corrupting _start
Corrupting _fini
Corrupting DW.ref.__gxx_personality_v0
Corrupting __gmon_start__
Corrupting _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev@@GLIBCXX_3.4.21
Corrupting _ZN3ELFC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEi
Corrupting __bss_start
Corrupting __dso_handle
Corrupting main
```


***readelf -s <file>***
```
28: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS <corrupt>
29: 0000000000001130     0 FUNC    LOCAL  DEFAULT   13 <corrupt>
30: 0000000000001160     0 FUNC    LOCAL  DEFAULT   13 <corrupt>
31: 00000000000011a0     0 FUNC    LOCAL  DEFAULT   13 <corrupt>
```

The program will still work, this is like strip a binary without strip



## What next ?
* Python bindings [Priority]
* Build tools to work with a large amount of ELF's, just like [telfhash](https://github.com/trendmicro/telfhash) but using this lib as the back bone.



Any error or problem please open a issue.

Thanks