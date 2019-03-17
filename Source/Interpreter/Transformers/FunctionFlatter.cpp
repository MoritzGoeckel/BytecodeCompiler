#pragma once

#include "../../ErrorHandling.cpp"
#include "../ASTNode.cpp"
#include "../Token.cpp"
#include "../TokenTypes.cpp"

#include <vector>

//Main is going to be the last function in the vector
//The ast will only be a block with an reference to the main
void flattenFunctions(ASTNode& root, std::vector<ASTNode>& fns){
    for(size_t i = 0; i < root.getChildCount(); i++){
        flattenFunctions(root.getChildMutable(i), fns);

        if(root.getChild(i).getTokenType() == FUNDEF){
            ASTNode fnRef(Token(FNREF, std::to_string(fns.size())));
            fns.push_back(root.getChild(i));
            root.setChild(i, fnRef);
        }
    }
}

void createMain(ASTNode& root){
    ASTNode newRoot = ASTNode(Token(BLOCK, ""));
    
    newRoot.addChild(Token(FUNDEF, "main"));

    newRoot.getChildMutable(0).addChild(Token(IDENTLIST, ""));
    newRoot.getChildMutable(0).addChild(root);    

    root = newRoot;
}