#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <iostream>

#include <sstream>

#include "OptCodeEngine.cpp"

void parseText(std::vector<unsigned char>& dada, std::string text){
    //TODO: Create bytecode from text

    //"LABEL" LINE
    //";"
}

static void split(const std::string& str, std::vector<std::string>& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        if(token != "" && token != " ")
            cont.push_back(token);
    }
}

void compileFile(std::string inPath, std::string outPath){
    //TODO: Read from file, parseText, write to file
    std::ifstream inFile(inPath);

    OptCodeEngine opts;
    ByteCode code;

    for(std::string line; getline(inFile, line);){
        std::vector<std::string> tokens;
        split(line, tokens, ' ');

        while(tokens.size() < 3)
            tokens.push_back("0");

        //Comment
        if(tokens[0] == ";")
            continue;

        //LABEL
        //JMP

        //Statement
        code.addStatement(opts.encodeOptString(tokens[0]), stoi(tokens[1]), stoi(tokens[2]));

        //TODO: Remove
        std::cout << "CMD: " << tokens[0] << "\tP1: " << tokens[1] << "\tP2: " << tokens[2] << std::endl;
    }
}