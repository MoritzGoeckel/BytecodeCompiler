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

ASTNode Parser::statement(){

    if(speculate(&Parser::block))
        return block();

    else if(speculate(&Parser::branch))
        return branch();

    else if(speculate(&Parser::ret)){
        ASTNode node = ret();
        consume(SEMIC);
        return node;
    }

    else if(speculate(&Parser::expression)){
        ASTNode node = expression();
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

    return node;
}

ASTNode Parser::expression(){
    
    if(speculate(&Parser::functionDefinition))
        return functionDefinition();
    
    if(speculate(&Parser::call))
        return call();

    if(speculate(&Parser::infixOperation))
        return infixOperation();

    if(getToken().getType() == NUMLIT)
        return ASTNode(consume(NUMLIT));

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

//Uses custom algorithm for operator precedence
ASTNode Parser::infixOperation(){

    bool withinBrackets = false;
    if(getToken().getType() == OBR){
        consume(OBR);
        withinBrackets = true;
    }

    bool expectingOperand = true;

    std::vector<ASTNode> stack;
    while(true){
        //OPERAND
        if(speculate(&Parser::operand) && expectingOperand){
            stack.push_back(operand());
            expectingOperand = false;
        }
        //OPERATOR
        else if(getToken().getType() == INFOP && !expectingOperand){
            stack.push_back(ASTNode(consume(INFOP)));
            expectingOperand = true;
        }
        //Open brackets
        else if(getToken().getType() == OBR){
            stack.push_back(infixOperation());
            expectingOperand = false;
        }
        else{
            break;    
        }
    }

    if(withinBrackets)
        consume(CBR);

    if(stack.size() < 3 || stack.size() % 2 == 0)
        throw ParsingException("BAD STACK SIZE", "", BT);

    //Reducing stack until only one element is left
    while(stack.size() > 1){
        //Iterating over the operators
        for(int i = 1; i < stack.size(); i += 2){
            if(i + 2 >= stack.size() || stack[i].getToken().getPrecedence() <= stack[i + 2].getToken().getPrecedence()){
                ASTNode node = stack[i];
                node.addChild(stack[i - 1]);
                node.addChild(stack[i + 1]);

                stack.erase(stack.begin() + i - 1, stack.begin() + i + 2);
                stack.insert(stack.begin() + i - 1, node);
            }
        }
    }

    if(stack.size() != 1)
        throw ParsingException("Stack size should be 1 but is " + std::to_string(stack.size()), "", BT);

    return stack[0];
}

ASTNode Parser::operand(){

    if(speculate(&Parser::functionDefinition))
        return functionDefinition();

    if(speculate(&Parser::call))
        return call();

    if(getToken().getType() == NUMLIT)
        return ASTNode(consume(NUMLIT));

    if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

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
    return node;
}