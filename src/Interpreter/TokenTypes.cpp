#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "../ErrorHandling.cpp"

std::map<std::string, size_t> precedences = {
    {"*", 5u}, 
    {"/", 5u}, 
    {"+", 6u},
    {"-", 6u},
    {"&&", 6u},
    {"||", 6u}, 
    {"==", 7u}, 
    {"!=", 7u},
    {"<", 7u}, 
    {">", 7u}, 
    {"=", 8u}
};

std::vector<std::string> types = {
    "OCBR",
    "CCBR",
    "OBR",
    "CBR",
    "INFOP",
    "RETURN",
    "COMMA",
    "SEMICOLON",
    "IDENT",
    "BRANCH",
    "NUMLIT",
    "BLOCK",
    "IDENTLIST",
    "FUNDEF",
    "EXPRLIST",
    "CALL",
    "UNINITIALIZED",
    "LET",
    "FNREF"
};

std::string typeToString(size_t type){
    if(type >= types.size())
        std::cout << "Index out of bounds: " << BT;
    
    return types[type];
}

const size_t OCBR = 0;
const size_t CCBR = 1;
const size_t OBR = 2;
const size_t CBR = 3;
const size_t INFOP = 4;
const size_t RETURN = 5;
const size_t COMMA = 6;
const size_t SEMICOLON = 7;
const size_t IDENT = 8;
const size_t BRANCH = 9;
const size_t NUMLIT = 10;
const size_t BLOCK = 11;
const size_t IDENTLIST = 12;
const size_t FUNDEF = 13;
const size_t EXPRESSIONLIST = 14;
const size_t CALL = 15;
const size_t UNINITIALIZED = 16;
const size_t LET = 17;
const size_t FNREF = 18;
