#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "TokenTypes.cpp"
#include "../ErrorHandling.cpp"

class Token{
    
    public:
    
    Token(){
        this->type = TokenTypes::UNINITIALIZED;
        this->text = "!!!!!!!!!!!";
    }

    Token(int type, std::string text){
        this->type = type;
        this->text = text;
    }

    const int& getType() const{
        return this->type;
    }

    const std::string getText() const{
        return this->text;
    }

    const std::string getPrintString() const{
        return typeToString(type) + "('" + text + "') ";
    }

    void print() const{
        std::cout << this->getPrintString(); 
    }

    const int getPrecedence() const{
        if(getType() != TokenTypes::INFOP)
            throw std::runtime_error("NON OP HAS NO PRECEDENCE: " + typeToString(getType()) + BT);

        if(precedences.find(getText()) == precedences.end())
            throw std::runtime_error("Precedence not defined for: " + getText() + BT);

        return precedences[getText()];
    }

    private: 

    int type;
    std::string text;
};
