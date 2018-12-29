#pragma once

#include <string>
#include <iostream>
#include <vector>

std::string typeToString(int type){
    std::vector<std::string> types;
    types.push_back("OCBR");
    types.push_back("CCBR");
    types.push_back("OBR");
    types.push_back("CBR");
    types.push_back("INFOP");
    types.push_back("RETURN");
    types.push_back("COMMA");
    types.push_back("SEMIC");
    types.push_back("COMP");
    types.push_back("ASSIGN");
    types.push_back("IDENT");
    types.push_back("BRANCH");
    types.push_back("NUMLIT");
    types.push_back("BLOCK");
    types.push_back("IDENTLIST");
    types.push_back("FUNDEF");
    types.push_back("EXPRESSIONLIST");
    types.push_back("CALL");

    return types[type];
}

int OCBR = 0;
int CCBR = 1;
const int OBR = 2;
const int CBR = 3;
const int INFOP = 4;
const int RETURN = 5;
const int COMMA = 6;
const int SEMIC = 7;
const int COMP = 8;
const int ASSIGN = 9;
const int IDENT = 10;
const int BRANCH = 11;
const int NUMLIT = 12;
const int BLOCK = 13;
const int IDENTLIST = 14;
const int FUNDEF = 15;
const int EXPRESSIONLIST = 16;
const int CALL = 17;

class Token{
    
    public:
    
    Token(){
        this->type = -1;
        this->text = "EMPTY";
    }

    Token(int type, std::string text){
        this->type = type;
        this->text = text;
    }

    int getType() const{
        return this->type;
    }

    std::string getText() const{
        return this->text;
    }

    void print() const{
        std::cout << "TOKEN('" << typeToString(type) << "', '" << text << "') "; 
    }

    private: 

    int type;
    std::string text;
};
