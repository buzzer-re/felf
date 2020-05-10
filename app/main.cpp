#include <iostream>


#include <unistd.h>
#include <sys/mman.h>

#include "Argparse.h"
#include "ELF.h"


#define INPUT_ARG "input"
#define DISPLAY_ARG "display"


bool exists(const std::string& filePath);

int main(int argc, char* argv[]) 
{

    ArgParse args(argc, argv);
    args.addArgument(INPUT_ARG, true, true);
    args.addArgument(DISPLAY_ARG, true, false);

    args.parse();
    
    Argument& input = args.getArgument(INPUT_ARG);
    std::string& filePath = input.argValue;

    if (!exists(filePath)) {
        std::cerr << "Invalid input file!\n";
        return 1;
    }
	
	
	ELF elf(filePath);
	
	if (!elf.valid()) {
		std::cerr << "Invalid ELF provided!\n";
		return 1;
	}

    return 0;   
}


bool exists(const std::string& filePath) 
{
    return access(filePath.c_str(), F_OK) != -1;
}
