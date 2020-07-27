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

    for (unsigned i = 0; i < elf.symbolTable.symbolDataMappedIter->second->size; ++i) {
        printf("0x%x", * (elf.symbolTable.symbolDataMapped.find("main")->second->data + i));
    }
    
    elf.save("quicksave");
    
    return 0;   
}


bool exists(const std::string& filePath) 
{
    return access(filePath.c_str(), F_OK) != -1;
}
