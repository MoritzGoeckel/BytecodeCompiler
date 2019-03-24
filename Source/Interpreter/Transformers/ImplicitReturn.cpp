#pragma once

#include "../../ErrorHandling.cpp"
#include "../ASTNode.cpp"
#include "../Token.cpp"

#include <vector>

//Main is going to be the last function in the vector
//The ast will only be a block with an reference to the main
void addVoidReturn(std::vector<ASTNode>& fns){
    for(auto& fn : fns){
        auto& block = fn.getChildMutable(1);
        if(block.getChildMutable(block.getChildCount() - 1).getTokenType() != TokenType::RETURN){
            ASTNode retNode(Token(TokenType::RETURN, std::string("Implicit return")));
            retNode.addChild(ASTNode(Token(TokenType::NUMLIT, std::string("0"))));
            block.addChild(retNode);
        }
    }
}