#pragma once

#include <vector>
#include <iostream>
#include "Token.cpp"

class ASTNode{

    private:
    
    std::vector<ASTNode> children;
    Token token;

    public:

    ASTNode(Token token){
        this->token = token;
    }

    void addChild(Token token){
        children.push_back(ASTNode(token));
    }

    void addChild(ASTNode node){
        children.push_back(node);
    }

    void print() const{
        token.print();
        if(children.size() != 0){
            std::cout << "{ ";
            for(auto const& t: children)
                t.print();
            std::cout << " }";
        }
    }
};