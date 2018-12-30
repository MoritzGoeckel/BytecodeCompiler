#pragma once

#include "Token.cpp"
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <cctype>

#include "../ErrorHandling.cpp"

class Lexer{
    
    public:
    
    Lexer(const std::string& path){
        this->index = 0;

        std::ifstream t(path);
        this->input = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    }

    void print(){
        std::cout << "Lexer: " << this->input << std::endl;
    }
    
    Token getNextToken(){
        consumeWhitespace();

        if(getChar() == '{') { consume(); return Token(OCBR, "{"); }
        if(getChar() == '}') { consume(); return Token(CCBR, "}"); }
        if(getChar() == '(') { consume(); return Token(OBR, "("); }
        if(getChar() == ')') { consume(); return Token(CBR, ")"); }

        if(getChar() == '+') { consume(); return Token(INFOP, "+"); }
        if(getChar() == '-') { consume(); return Token(INFOP, "-"); }
        if(matches("&&")) { consume("&&"); return Token(INFOP, "&&"); }
        if(matches("||")) { consume("||"); return Token(INFOP, "||"); }
        if(getChar() == '/') { consume(); return Token(INFOP, "/"); }
        if(getChar() == '*') { consume(); return Token(INFOP, "*"); }
    
        if(getChar() == '=') { consume(); return Token(INFOP, "="); }

        if(matches("==")) { consume("=="); return Token(INFOP, "=="); }
        if(matches("!=")) { consume("!="); return Token(INFOP, "!="); }
        if(getChar() == '>') { consume(); return Token(INFOP, ">"); }
        if(getChar() == '<') { consume(); return Token(INFOP, "<"); }

        if(matches("return")) { consume("return"); return Token(RETURN, "return"); }
        if(matches("if")) { consume("if"); return Token(BRANCH, "if"); }
        if(matches("while")) { consume("while"); return Token(BRANCH, "while"); }
        
        if(getChar() == ',') { consume(); return Token(COMMA, ","); } 
        if(getChar() == ';') { consume(); return Token(SEMICOLON, ";"); }
              
        if(isdigit(getChar()))
            return Token(NUMLIT, consumeAlphanumWord());

        if(isalpha(getChar()))
            return Token(IDENT, consumeAlphanumWord());

        //Will never happen
        throw std::runtime_error("Token unknown: " + std::string(1, getChar()) + BT);
    }
 
    bool eof(){
        return this->index >= this->input.length() - 1;
    }   

    private:

    int index;
    std::string input;

    void consume(){
        this->index++;
    }

    char getChar(){
        return this->input.at(index);
    }

    char getCharLookahead(int offset){
        return this->input.at(index + offset);
    }

    void consume(std::string txt){
        for(char& c : txt){
            if(c != this->getChar())
                throw std::runtime_error("Unexpected char" + BT);
            else
                consume(); 
        }
    }

    bool matches(std::string pattern){
        for(int i = 0; i < pattern.length(); i++){
            if(pattern.at(i) != this->getCharLookahead(i))
                return false;
        }
        return true;
    }

    void consumeWhitespace(){
        std::string whiteSpaces = " \t\n\r";
        while(true){
            std::string c(1, this->getChar());
            if (whiteSpaces.find(c) == std::string::npos)
                return;
            consume();
        }
    }

    std::string consumeAlphanumWord(){
        std::string output = "";
        while(true){
            char c = this->getChar();
            if (!isalnum(c))
                break;
            output.push_back(c);
            consume();
        }
        return output;
    }

};
