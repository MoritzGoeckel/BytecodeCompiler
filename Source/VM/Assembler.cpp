#pragma once

#include <vector>
#include <string>

#include <iostream>
#include <sstream>
#include <fstream>

#include "Types.cpp"
#include "../ErrorHandling.cpp"

#include "OptCodeEngine.cpp"
#include "ByteCode.cpp"

#include<bits/stdc++.h>

static void split(const std::string& str, std::vector<std::string>& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        if(token != "" && token != " ")
            cont.push_back(token);
    }
}

static ByteCode assembleStream(std::stringstream& ss);

ByteCode assembleString(std::string str){
    std::stringstream ss(str);
    return assembleStream(ss);
}

ByteCode assembleFile(std::string inPath){
    std::ifstream file(inPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return assembleStream(buffer);
}

static ByteCode assembleStream(std::stringstream& ss){
    OptCodeEngine opts;
    ByteCode code;

    std::map<std::string, int> labels;
    std::vector<std::string> lines;

    int8 nextVariableId = 0;
    std::map<std::string, int8> variables;

    for(std::string line; getline(ss, line);){
        if(!line.empty() && line.front() != '#') //# is comment or maybe ;
            lines.push_back(line);
    }

    //for(std::string& s : lines)
    //    std::cout << "LINE: " << s << std::endl;

    //Detect, index and remove labels
    size_t codeIndex = 0u;
    for(size_t lineIndex = 0u; lineIndex < lines.size(); lineIndex++) {
        std::vector<std::string> tokens;
        split(lines[lineIndex], tokens, ' ');

        if(tokens[0].back() == ':' && tokens[0].front() == '&'){
            tokens[0].erase(tokens[0].end() - 1);
            if(labels.find(tokens[0]) != labels.end())
                throw std::runtime_error("Label already declared" + BT);

            labels[tokens[0]] = codeIndex;
            lines.erase(lines.begin() + lineIndex);
            lineIndex--;
        }
        else{
            codeIndex += tokens.size();
        }
    }

    //for (auto& t : labels)
    //    std::cout << t.first << "\t\t" << t.second << std::endl;

    for(auto const& line: lines) {
        //std::cout << "Processing line: " << line << std::endl;

        std::vector<std::string> tokens;
        split(line, tokens, ' ');

        if(tokens[0].back() == ':')
            throw std::runtime_error("Labels should be removed by now" + BT);

        //Statement
        code.add(opts.encodeOptString(tokens[0u]));
        for(size_t i = 1u; i < tokens.size(); i++){
            //std::cout << "Processing token: " << tokens[i] << std::endl;

            // Labels
            if(tokens[i].front() == '&'){
                if(labels.find(tokens[i]) == labels.end())
                    throw std::runtime_error("Unknown label" + BT);

                //tokens[i] = std::to_string(labels[tokens[i]]);
                code.add(labels[tokens[i]]);
            }
            
            // Variables
            else if(tokens[i].front() == '%'){
                std::string name = tokens[i];
                int8 id;
                if(variables.find(name) == variables.end()){
                    // New Variable
                    variables[name] = nextVariableId++;
                    //std::cout << "New var: " << name << " mapping to " << std::to_string(variables[name]) << std::endl;
                }
                code.add(variables[name]);
                //std::cout << "Old var: " << name << " mapping to " << std::to_string(variables[name]) << std::endl;
            }

            // Values
            else{
                code.add(stoi(tokens[i]));
            }
        }
    }

    return code;
}
