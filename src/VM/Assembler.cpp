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

    for(std::string line; getline(ss, line);)
        lines.push_back(line);

    /*for(std::string& s : lines)
        std::cout << "LINE: " << s << std::endl;*/

    //Detect, index and remove labels
    int codeIndex = 0;
    for(int lineIndex = 0; lineIndex < lines.size(); lineIndex++) {
        std::vector<std::string> tokens;
        split(lines[lineIndex], tokens, ' ');
        
        if(tokens[0].back() == ':'){
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
        //std::cout << "Processing: " << line << std::endl;

        std::vector<std::string> tokens;
        split(line, tokens, ' ');

        /*tokens.erase(std::remove_if(
            tokens.begin(), 
            tokens.end(),
            [](std::string s){return s.size() <= 1 && !isdigit(s[0]);}
        ), tokens.end());*/

        //Comment
        if(tokens[0] == ";")
            continue;

        //JMP
        if(tokens[0] == "JMP" || tokens[0] == "JMPC" || tokens[0] == "CALL" ){
            if(labels.find(tokens[1]) == labels.end())
                throw std::runtime_error("Unknown label" + BT);

            tokens[1] = std::to_string(labels[tokens[1]]);
        }

        if(tokens[0].back() == ':')
            throw std::runtime_error("Labels should be removed by now" + BT);

        //std::cout << "CMD: " << tokens[0] << "\tP1: " << tokens[1] << "\tP2: " << tokens[2] << std::endl;

        //Statement
        code.add(opts.encodeOptString(tokens[0]));
        for(int i = 1; i < tokens.size(); i++){
            if(tokens[i].front() == '%'){
                //std::cout << "'" << tokens[i] << "'" << std::endl;

                //Its a variable
                std::string name = tokens[i];
                int8 id;
                if(variables.find(name) == variables.end()){
                    //Its a new one
                    variables[name] = nextVariableId++;
                    //std::cout << "New var: " << name << " mapping to " << std::to_string(variables[name]) << std::endl;
                    //std::cout << "new! " << name << " " << std::to_string(variables[name]) << std::endl; 
                }
                code.add(variables[name]);
                //std::cout << "Var: " << name << " mapping to " << std::to_string(variables[name]) << std::endl;
                
                //std::cout << name << " " << std::to_string(variables[name]) << std::endl; 
            }
            else{
                //Its a value
                code.add(stoi(tokens[i]));
            }
        }
    }

    return code;
}
