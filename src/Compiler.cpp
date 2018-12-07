#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <iostream>

#include <sstream>

#include "OptCodeEngine.cpp"
#include "ByteCode.cpp"

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

    std::map<std::string, int> labels;

    for(std::string line; getline(inFile, line);){
        std::vector<std::string> tokens;
        split(line, tokens, ' ');

        while(tokens.size() < 3)
            tokens.push_back("0");

        //Comment
        if(tokens[0] == ";")
            continue;

        //LABEL
        if(tokens[0].back() == ':'){
            if(labels.find(tokens[0]) != labels.end())
                throw;

            labels[tokens[0]] = code.size();
            continue;
        }

        //JMP
        if(tokens[0] == "JMP"){
            if(labels.find(tokens[1]) == labels.end())
                throw;

            tokens[1] = labels[tokens[1]];
        }

        //Statement
        code.addStatement(opts.encodeOptString(tokens[0]), stoi(tokens[1]), stoi(tokens[2]));

        //TODO: Remove
        std::cout << "CMD: " << tokens[0] << "\tP1: " << tokens[1] << "\tP2: " << tokens[2] << std::endl;
    }

    code.write(outPath);
}