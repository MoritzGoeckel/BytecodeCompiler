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
        //std::cout << "Consume index " << std::to_string(index) << " " << typeToString(getToken().getType()) << std::endl;
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
        //std::cout << "Mark index " << std::to_string(index)  << " " << typeToString(getToken().getType()) << std::endl;
        markers.push_back(this->index); 
    }

    void reset(){
        this->index = markers.back();
        //std::cout << "Reset to " << std::to_string(index) << " " << typeToString(getToken().getType()) << std::endl;
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

    //This is one is dangerous for infinite recursion
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

ASTNode Parser::infixOperation(){

    //TODO: Maybe use custom algorithm here
    //Stack based with precedence and until semicolon

    ASTNode left = operand();
    Token op = consume(INFOP);
    ASTNode right = expression();

    //If its an assignment left should be an identification
    if(op.getText() == "=" && left.getToken().getType() != IDENT)
        throw ParsingException("IDENT", typeToString(left.getToken().getType()), BT);    

    ASTNode node(op);
    node.addChild(left);
    node.addChild(right);

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