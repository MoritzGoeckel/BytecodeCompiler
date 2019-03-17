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

        if(getChar() == '{') { consume(); return Token(TokenType::OCBR, "{"); }
        if(getChar() == '}') { consume(); return Token(TokenType::CCBR, "}"); }
        if(getChar() == '(') { consume(); return Token(TokenType::OBR, "("); }
        if(getChar() == ')') { consume(); return Token(TokenType::CBR, ")"); }

        if(getChar() == '+') { consume(); return Token(TokenType::INFOP, "+"); }
        if(getChar() == '-') { consume(); return Token(TokenType::INFOP, "-"); }
        if(matches("&&")) { consume("&&"); return Token(TokenType::INFOP, "&&"); }
        if(matches("||")) { consume("||"); return Token(TokenType::INFOP, "||"); }
        if(getChar() == '/') { consume(); return Token(TokenType::INFOP, "/"); }
        if(getChar() == '*') { consume(); return Token(TokenType::INFOP, "*"); }
    
        if(getChar() == '=') { consume(); return Token(TokenType::INFOP, "="); }

        if(matches("==")) { consume("=="); return Token(TokenType::INFOP, "=="); }
        if(matches("!=")) { consume("!="); return Token(TokenType::INFOP, "!="); }
        if(getChar() == '>') { consume(); return Token(TokenType::INFOP, ">"); }
        if(getChar() == '<') { consume(); return Token(TokenType::INFOP, "<"); }

        if(matches("return")) { consume("return"); return Token(TokenType::RETURN, "return"); }
        if(matches("if")) { consume("if"); return Token(TokenType::BRANCH, "if"); }
        if(matches("while")) { consume("while"); return Token(TokenType::BRANCH, "while"); }

        if(matches("let")) { consume("let"); return Token(TokenType::LET, ""); }
        
        if(getChar() == ',') { consume(); return Token(TokenType::COMMA, ","); } 
        if(getChar() == ';') { consume(); return Token(TokenType::SEMICOLON, ";"); }
              
        if(isdigit(getChar()))
            return Token(TokenType::NUMLIT, consumeAlphanumWord());

        if(isalpha(getChar()))
            return Token(TokenType::IDENT, consumeAlphanumWord());

        //Will never happen
        throw std::runtime_error("Token unknown: " + std::string(1, getChar()) + BT);
    }
 
    bool eof(){
        return this->index >= this->input.length() - 1u;
    }   

    private:

    size_t index;
    std::string input;

    void consume(){
        this->index++;
    }

    char getChar(){
        return this->input.at(index);
    }

    char getCharLookahead(size_t offset){
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
        for(size_t i = 0; i < pattern.length(); i++){
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
