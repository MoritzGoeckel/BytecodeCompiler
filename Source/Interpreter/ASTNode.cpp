#pragma once

#include <vector>
#include <iostream>
#include "Token.cpp"

static void printSpace(size_t num){
    for(size_t i = 0; i < num; i++)
        std::cout << " ";
}

class ASTNode{

    private:
    std::vector<ASTNode> children;
    Token token;

    void print(size_t indent) const{
        printSpace(indent);
        token.print();
        std::cout << std::endl;

        if(children.size() != 0u){
            printSpace(indent);
            std::cout << "{" << std::endl;
            
            for(auto const& t: children)
                t.print(indent + 5u);
            
            printSpace(indent);            
            std::cout << "}" << std::endl;
        }
    }

    public:

    ASTNode(Token token){
        this->token = token;
    }

    const std::string getText() const{
        return token.getText();
    }

    const Token& getToken() const{
        return token;
    }

    const TokenType getTokenType() const{
        return token.getType();
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

    const size_t getChildCount() const{
        return children.size();
    }

    const ASTNode& getChild(size_t index) const{
        return children[index];
    }

    const std::vector<ASTNode>& getChildren() const{
        return children;
    }

    ASTNode& getChildMutable(size_t index){
        return children[index];
    }

    void setChild(size_t index, ASTNode& child){
        children[index] = child;
    }
};