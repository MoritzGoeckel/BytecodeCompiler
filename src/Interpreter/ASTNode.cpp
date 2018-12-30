#pragma once

#include <vector>
#include <iostream>
#include "Token.cpp"

static void printSpace(int num){
    for(int i = 0; i < num; i++)
        std::cout << " ";
}

class ASTNode{

    private:
    
    std::vector<ASTNode> children;
    Token token;

    void print(int indent) const{
        printSpace(indent);
        token.print();
        std::cout << std::endl;

        if(children.size() != 0){
            printSpace(indent);
            std::cout << "{" << std::endl;
            
            for(auto const& t: children)
                t.print(indent + 5);
            
            printSpace(indent);            
            std::cout << "}" << std::endl;
        }
    }

    public:

    ASTNode(Token token){
        this->token = token;
    }

    const Token& getToken() const{
        return token;
    }

    void addChild(Token token){
        children.push_back(ASTNode(token));
    }

    void addChild(ASTNode node){
        children.push_back(node);
    }

    void print() const{
        print(0);
    }
};