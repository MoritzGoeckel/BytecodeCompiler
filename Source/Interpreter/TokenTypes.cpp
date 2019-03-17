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

enum TokenType{
    OCBR, 
    CCBR, 
    OBR, 
    CBR, 
    INFOP, 
    RETURN, 
    COMMA, 
    SEMICOLON, 
    IDENT, 
    BRANCH, 
    NUMLIT, 
    BLOCK,
    IDENTLIST,
    FUNDEF,
    EXPRESSIONLIST,
    CALL,
    UNINITIALIZED,
    LET,
    FNREF
};

std::string typeToString(const TokenType type){
    if(type >= types.size())
        std::cout << "Index out of bounds: " << BT;
    
    return types[type];
}
