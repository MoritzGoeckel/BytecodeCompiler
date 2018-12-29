#pragma once

#include <string>
#include <vector>

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
        std::cout << "Consume index " << std::to_string(index) << " " << typeToString(getToken().getType()) << std::endl;
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
        std::cout << "Mark index " << std::to_string(index)  << " " << typeToString(getToken().getType()) << std::endl;
        markers.push_back(this->index); 
    }

    void reset(){
        this->index = markers.back();
        std::cout << "Reset to " << std::to_string(index) << " " << typeToString(getToken().getType()) << std::endl;
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
    ASTNode operand();

    typedef ASTNode (Parser::*ParserFn)();
    bool speculate(ParserFn fn);
};

bool Parser::speculate(ParserFn fn){
    bool success = true;
    mark();
    try{
        //Call the function on this object without parameters
        ((*this).*(fn)) ();
    }catch(ParsingException& e){
        success = false;
    }
    reset();

    return success;
}

//    std::cout << "ASSIGNMENT FIRST " << typeToString(getToken().getType()) << std::endl;
//    std::cout << "ASSIGNMENT SECOND " << typeToString(LA(1).getType()) << std::endl;
//    std::cout << "ASSIGNMENT SECOND " << typeToString(LA(2).getType()) << std::endl;

ASTNode Parser::statement(){

    std::cout << "STATEMENT!" << std::endl;

    if(speculate(&Parser::block))
        return block();

    else if(speculate(&Parser::branch))
        return branch();

    else if(speculate(&Parser::ret)){
        ASTNode node = ret();
        consume(SEMIC);
        return node;
    }

    else if(speculate(&Parser::call)){
        ASTNode node = call();
        consume(SEMIC);
        return node;
    }

    else if(speculate(&Parser::assignment)){
        ASTNode node = assignment();
        consume(SEMIC);
        return node;
    }

    else
        throw ParsingException("block, assignment, branch, return", typeToString(getToken().getType()), BT);
}

ASTNode Parser::block(){
    ASTNode node(Token(BLOCK, ""));
    consume(OCBR); 
    while(getToken().getType() != CCBR){
        node.addChild(statement());
    }
    consume(CCBR);

    std::cout << "BLOCK!" << std::endl;
    return node;
}

ASTNode Parser::expression(){

    std::cout << "EXPR!" << std::endl;

    if(speculate(&Parser::assignment))
        return assignment();
    
    if(speculate(&Parser::functionDefinition))
        return functionDefinition();
    
    if(speculate(&Parser::call))
        return call();

    //TODO: This is bad infinite recursion
    if(speculate(&Parser::infixOperation))
        return infixOperation();

    if(getToken().getType() == NUMLIT)
        return ASTNode(consume(NUMLIT));

    if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

    throw ParsingException("assignment, infix operation, function definition, call, identifier", typeToString(getToken().getType()), BT);
}

ASTNode Parser::assignment(){
    ASTNode leftSide = ASTNode(consume(IDENT));
    Token assignToken = consume(ASSIGN);
    ASTNode rightSide = expression();

    ASTNode node(assignToken);
    node.addChild(leftSide);
    node.addChild(rightSide);

    std::cout << "ASSIGNMENT!" << std::endl;
    return node;
}

ASTNode Parser::branch(){
    ASTNode node(ASTNode(consume(BRANCH)));
    node.addChild(expression());
    node.addChild(block());

    std::cout << "BRANCH!" << std::endl;
    return node;
}

ASTNode Parser::ret(){
    ASTNode node(ASTNode(consume(RETURN)));
    node.addChild(expression());

    std::cout << "RETURN!" << std::endl;
    return node;
}

ASTNode Parser::infixOperation(){
    ASTNode left = operand();
    std::cout << "left done" << std::endl;
    std::cout << "next: " << typeToString(getToken().getType()) << std::endl;

    Token op = consume(INFOP);
    std::cout << "op done" << std::endl;

    ASTNode right = expression();

    ASTNode node(op);
    node.addChild(left);
    node.addChild(right);

    std::cout << "INFIX OPERATION!" << std::endl;
    return node;
}

ASTNode Parser::operand(){
    if(getToken().getType() == NUMLIT)
        return ASTNode(consume(NUMLIT));

    if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

    if(speculate(&Parser::call))
        return call();

    throw ParsingException("NUMLITERAL, IDENTIFIER, CALL", typeToString(getToken().getType()), BT);    
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

    std::cout << "FUNCTION DEFINITION!" << std::endl;
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

    std::cout << "FUNCTION CALL!" << std::endl;
    return node;
}