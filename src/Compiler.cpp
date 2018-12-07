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
    std::ifstream inFile(inPath);

    OptCodeEngine opts;
    ByteCode code;

    std::map<std::string, int> labels;

    std::vector<std::string> lines;

    for(std::string line; getline(inFile, line);)
        lines.push_back(line);

    //Detect, index and remove labels
    for(int i = 0; i < lines.size(); i++) {
        std::vector<std::string> tokens;
        split(lines[i], tokens, ' ');
        
        if(tokens[0].back() == ':'){
            tokens[0].erase(tokens[0].end() - 1);
            if(labels.find(tokens[0]) != labels.end())
                throw "Label already declared";

            labels[tokens[0]] = i;
            lines.erase(lines.begin() + i);
            i--;
        }
    }

    //for (auto& t : labels)
    //    std::cout << t.first << "\t\t" << t.second << std::endl;

    for(auto const& line: lines) {
        //std::cout << "Processing: " << line << std::endl;

        std::vector<std::string> tokens;
        split(line, tokens, ' ');

        while(tokens.size() < 3)
            tokens.push_back("0");

        //Comment
        if(tokens[0] == ";")
            continue;

        //JMP
        if(tokens[0] == "JMP" || tokens[0] == "JPE" || tokens[0] == "JPLE" || tokens[0] == "JPGE" || tokens[0] == "JPG" || tokens[0] == "JPL"){
            if(labels.find(tokens[1]) == labels.end())
                throw "Unknown label";

            //std::cout << "Found label link " << labels[tokens[1]] << std::endl;
            tokens[1] = std::to_string(labels[tokens[1]]);
        }

        if(tokens[0].back() == ':')
            throw;

        //TODO: Remove
        //std::cout << "CMD: " << tokens[0] << "\tP1: " << tokens[1] << "\tP2: " << tokens[2] << std::endl;

        //Statement
        code.addStatement(opts.encodeOptString(tokens[0]), stoi(tokens[1]), stoi(tokens[2]));
    }

    code.write(outPath);
}