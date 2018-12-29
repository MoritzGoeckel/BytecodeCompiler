#pragma once

#include <string>
#include <vector>
#include <functional>

#include "Token.cpp"
#include "../ErrorHandling.cpp"
#include "ASTNode.cpp"
#include "Exceptions.cpp"

class Parser{
    public:
    
    Parser(const std::vector<Token>& tokens){
        this->tokens = tokens;
        this->index = 0;
    } 

    ASTNode parse(){
        return statement(); 
    }

    private:
    
    std::vector<Token> tokens;
    std::vector<int> markers;
    int index;

    const Token& getToken() const{
        return LA(0);
    }

    const Token& LA(int offset) const{
        return tokens[index + offset];
    }

    const Token& consume(){
        return tokens[index++];
    }

    const Token& consume(int tokenType){
        const Token& token = getToken();
        if(token.getType() == tokenType){
            consume();
            return token;
        }
        throw ParsingException(typeToString(token.getType()), typeToString(tokenType), BT);
    }

    void mark(){
        markers.push_back(this->index); 
    }

    void reset(){
        this->index = markers.back();
        markers.pop_back();
    }

    ASTNode block();
    ASTNode statement();
    ASTNode expression();

    ASTNode assignment();
    ASTNode branch();
    ASTNode ret();
    ASTNode infixOperation();
    ASTNode functionDefinition();
    ASTNode call();

    typedef ASTNode (Parser::*ParserFn)();
    bool speculate(ParserFn fn);
};

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

bool Parser::speculate(ParserFn fn){
    bool success = true;
    mark();
    try{
        //std::invoke(fn, this);
        CALL_MEMBER_FN(*this, fn) ();
    }catch(ParsingException& e){
        success = false;
    }
    reset();

    return success;
}

ASTNode Parser::statement(){
    if(speculate(&Parser::block))
        return block();

    if(speculate(&Parser::assignment))
        return assignment();
    
    if(speculate(&Parser::branch))
        return branch();
    
    if(speculate(&Parser::ret))
        return ret();

    throw ParsingException("block, assignment, branch, return", typeToString(getToken().getType()), BT);
}

ASTNode Parser::block(){
    ASTNode node(Token(BLOCK, ""));
    consume(OCBR); 
    while(getToken().getType() != CCBR){
        node.addChild(statement());
    }
    consume(CCBR);

    return node;
}

ASTNode Parser::assignment(){
    ASTNode leftSide = ASTNode(consume(IDENT));
    Token assignToken = consume(ASSIGN);
    ASTNode rightSide = expression();

    ASTNode node(assignToken);
    node.addChild(leftSide);
    node.addChild(rightSide);

    return node;
}

ASTNode Parser::expression(){
    if(speculate(&Parser::assignment))
        return assignment();
    
    if(speculate(&Parser::infixOperation))
        return infixOperation();
    
    if(speculate(&Parser::functionDefinition))
        return functionDefinition();
    
    if(speculate(&Parser::call))
        return call();

    if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

    throw ParsingException("assignment, infix operation, function definition, call, identifier", typeToString(getToken().getType()), BT);
}

ASTNode Parser::branch(){
    ASTNode node(ASTNode(consume(BRANCH)));
    node.addChild(expression());
    node.addChild(block());

    return node;
}

ASTNode Parser::ret(){
    ASTNode node(ASTNode(consume(RETURN)));
    node.addChild(expression());

    return node;
}

ASTNode Parser::infixOperation(){
    ASTNode left = expression();
    Token op = consume(RETURN);
    ASTNode right = expression();

    ASTNode node(op);
    node.addChild(left);
    node.addChild(right);

    return node;
}

ASTNode Parser::functionDefinition(){
    ASTNode identifierList(Token(IDENTLIST, ""));

    consume(OBR);
    while(getToken().getType() == IDENT){
        identifierList.addChild(consume(IDENT));
        if(getToken().getType() == COMMA)
            consume(COMMA);
        else
            break;
    }
    consume(CBR);

    ASTNode node(Token(FUNDEF, ""));
    node.addChild(identifierList);
    node.addChild(block());

    return node;
}

ASTNode Parser::call(){
    ASTNode node(Token(CALL, ""));
    node.addChild(consume(IDENT));

    ASTNode expressionList(Token(EXPRESSIONLIST, ""));
    consume(OBR);
    while(speculate(&Parser::expression)){
        expressionList.addChild(expression());
        if(getToken().getType() == COMMA)
            consume(COMMA);
        else
            break;
    }
    consume(CBR);

    node.addChild(expressionList);
}