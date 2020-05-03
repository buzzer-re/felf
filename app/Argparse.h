#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include <unistd.h>
#include <getopt.h>


#include <iostream>

/// Struct that represent an argument
struct Argument {
    std::string argName;
    std::string argValue;
    bool required;
    bool required_arg;
    int argIndex;
};


class ArgParse {
public:
    ArgParse(int argc, char* argv[]);
    bool parse();
    /// required an argument and if its requried to be passed
    bool addArgument(const std::string& argument, bool required_arg, bool required);
    std::unordered_map<std::string, Argument>& getArgumentMap() const;
    
    Argument& getArgument(const std::string& argument);

private:
    std::unordered_map<std::string, Argument> arguments;
    std::unordered_map<std::string, Argument>::iterator argIt;
    int argc;
    char** argv; /// An pointer to argv
};



ArgParse::ArgParse(int argc, char* argv[])
{
    this->argc = argc;
    this->argv = argv;
}


bool ArgParse::addArgument(const std::string& argument, bool required_arg, bool required) 
{
    argIt = this->arguments.find(argument);

    if (argIt != this->arguments.end() )
        return false;

    Argument newArgument{argument,"", required, required_arg};

    this->arguments.insert(std::make_pair(argument, newArgument));

    return true;
}


bool ArgParse::parse() 
{         
    struct option long_options[this->arguments.size()];

    int arg = 0;
    int op_i = 0;
    int need_matches = 0;
    int matches = 0;
    /// fill long options with our arguments
    for (argIt = arguments.begin(); argIt != arguments.end(); ++argIt, ++op_i) {
        long_options[op_i] = {
            argIt->first.c_str(), argIt->second.required_arg, 0, argIt->first.at(0)
        };
        if (argIt->second.required) need_matches++;
    }

    op_i = 0; // reset var for other loop
    // parse argv using getotp
    while ((arg = getopt_long(this->argc, this->argv, "abc:d:f:", long_options, &op_i)) != -1) {
        if (arg == '?') return false;

        std::string argumentName = long_options[op_i].name;
        this->argIt = this->arguments.find(argumentName);
        
        if (argIt->second.argValue != "") continue; /// repeated

        argIt->second.argIndex = op_i;

        if (argIt->second.required_arg) {
            argIt->second.argValue = optarg;
        }

        if (argIt->second.required) matches++;

    }

    return matches == need_matches;
}


/// Throws error if the argument does not was inserted before
Argument& ArgParse::getArgument(const std::string& argument) 
{
    this->argIt = this->arguments.find(argument);

    if (argIt == this->arguments.end()) 
        throw ("Unable to find argument ");


    return argIt->second;
}
