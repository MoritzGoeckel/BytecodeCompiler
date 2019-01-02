#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>

std::map<std::string, int> precedences = {
    {"*", 5}, 
    {"/", 5}, 
    {"+", 6},
    {"-", 6},
    {"&&", 6},
    {"||", 6}, 
    {"==", 7}, 
    {"!=", 7},
    {"<", 7}, 
    {">", 7}, 
    {"=", 8}
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
    "LET"
};

std::string typeToString(int type){
    return types[type];
}

const int OCBR = 0;
const int CCBR = 1;
const int OBR = 2;
const int CBR = 3;
const int INFOP = 4;
const int RETURN = 5;
const int COMMA = 6;
const int SEMICOLON = 7;
const int IDENT = 8;
const int BRANCH = 9;
const int NUMLIT = 10;
const int BLOCK = 11;
const int IDENTLIST = 12;
const int FUNDEF = 13;
const int EXPRESSIONLIST = 14;
const int CALL = 15;
const int UNINITIALIZED = 16;
const int LET = 17;
