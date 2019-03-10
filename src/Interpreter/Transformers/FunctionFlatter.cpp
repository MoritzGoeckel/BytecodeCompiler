#pragma once

#include "../../ErrorHandling.cpp"
#include "../ASTNode.cpp"
#include "../Token.cpp"
#include "../TokenTypes.cpp"

#include <vector>

void flattenFunctions(ASTNode& root, std::vector<ASTNode>& fns){
    for(int i = 0; i < root.getChildCount(); i++){
        flattenFunctions(root.getChildMutable(i), fns);

        if(root.getChild(i).getTokenType() == FUNDEF){
            ASTNode fnRef(Token(FNREF, std::to_string(fns.size())));
            fns.push_back(root.getChild(i));
            root.setChild(i, fnRef);
        }
    }
}