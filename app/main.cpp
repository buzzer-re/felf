#include <iostream>


#include <unistd.h>
#include <sys/mman.h>

#include "Argparse.h"
#include "ELF.h"


#define INPUT_ARG "input"
#define DISPLAY_ARG "display-header"


bool exists(const std::string& filePath);


int main(int argc, char* argv[]) 
{

    ArgParse args(argc, argv);
    args.addArgument(INPUT_ARG, true, true);
    args.addArgument(DISPLAY_ARG, false, false);
    
    args.parse();
    
    Argument& input = args.getArgument(INPUT_ARG);
    std::string& filePath = input.argValue;

    if (!exists(filePath)) {
        std::cerr << "Invalid input file!\n";
        return 1;
    }
	
	ELF elf(filePath, MAP_RW);
	
	if (!elf.valid()) {
		std::cerr << "Invalid ELF provided!\n";
		return 1;
	}



    elf.symbolTable.symbolDataMappedIter = elf.symbolTable.symbolDataMapped.find("main");

    for (auto i = 0; i < elf.symbolTable.symbolDataMappedIter->second->symbol->st_size; ++i) {
        printf("%c", elf.symbolTable.symbolDataMappedIter->second->data);
    }
    // for (elf.symbolTable.symbolsMappedIter= elf.symbolTable.symbolsMapped.begin(); 
    //     elf.symbolTable.symbolsMappedIter != elf.symbolTable.symbolsMapped.end(); 
    //     ++elf.symbolTable.symbolsMappedIter) {
        
    //     std::cout << "Corrupting " << elf.symbolTable.symbolsMappedIter->first << std::endl;
    //     elf.symbolTable.symbolsMappedIter->second->st_name = -1;
    // }
    
    return 0;   
}


bool exists(const std::string& filePath) 
{
    return access(filePath.c_str(), F_OK) != -1;
}
