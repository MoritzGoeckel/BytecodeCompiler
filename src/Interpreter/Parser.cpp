#pragma once

#include <string>
#include <vector>

#include "Token.cpp"
#include "../ErrorHandling.cpp"
#include "ASTNode.cpp"
#include "Exceptions.cpp"

#define MAX_TOKEN_PRINT 8
#define LEVEL_CHARS "   "

//#define VERBOSE

class Parser{
    public:
    
    Parser(const std::vector<Token>& tokens){
        this->tokens = tokens;
        this->index = 0;
    } 

    ASTNode parse(){
        return statement(0); 
    }

    private:
    
    std::vector<Token> tokens;
    std::vector<int> markers;
    int index;

    const Token& getToken() const{
        return LA(0);
    }

    const Token& LA(int offset) const{
        if(index + offset >= tokens.size())
            std::cout << "Index out of bounds: " << BT;

        return tokens[index + offset];
    }

    void printStream(int level, int max, std::string name) const{
        std::cout << " >> " << name << " ";
        printStream(level, max);
    }

    void printStream(int level, int max) const{
        #if defined(VERBOSE)
        for(int i = 0; i < level; i++)
            std::cout << LEVEL_CHARS;

        std::cout << " >> ";

        for(int i = index; i < index + max && i < tokens.size(); i++)
            tokens[i].print();
        std::cout << std::endl;
        #endif
    }

    void print(int level, std::string str){
        #if defined(VERBOSE)
        for(int i = 0; i < level; i++)
            std::cout << LEVEL_CHARS;

        std::cout << str << std::endl;
        #endif
    }

    const Token& consume(){
        const Token& token = getToken();
        index++;

        if(index > tokens.size()){
            std::cout << "Index out of bounds: " << BT;
            throw ParsingException("ANY", "END OF TOKENS", BT);
        }

        return token;
    }

    const Token& consume(int tokenType){
        if(getToken().getType() == tokenType){
            return consume();
        }

        throw ParsingException(typeToString(tokenType), typeToString(getToken().getType()), BT);
    }

    void mark(){
        markers.push_back(this->index); 
    }

    void reset(){
        this->index = markers.back();
        markers.pop_back();
    }

    ASTNode block(int level);
    ASTNode statement(int level);
    ASTNode expression(int level);

    ASTNode let(int level);

    ASTNode branch(int level);
    ASTNode ret(int level);
    ASTNode infixOperation(int level);
    ASTNode functionDefinition(int level);
    ASTNode call(int level);
    ASTNode operand(int level);

    typedef ASTNode (Parser::*ParserFn)(int);
    bool speculate(int level, ParserFn fn);
};

bool Parser::speculate(int level, ParserFn fn){
    bool success = true;
    mark();
    try{
        //Call the function on this object without parameters
        ((*this).*(fn)) (level);
    }catch(ParsingException& e){
        #if defined(VERBOSE)
        e.printShort();
        #endif
        success = false;
    }
    reset();

    return success;
}

ASTNode Parser::statement(int level){
    print(level, "STATEMENT");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    if(speculate(level, &Parser::block))
        return block(level);

    else if(speculate(level, &Parser::branch))
        return branch(level);

    else if(speculate(level, &Parser::ret)){
        ASTNode node = ret(level);
        consume(SEMICOLON);
        return node;
    }

    else if(speculate(level, &Parser::expression)){
        ASTNode node = expression(level);
        consume(SEMICOLON);
        return node;
    }

    else if(speculate(level, &Parser::let))
        return let(level);

    else
        throw ParsingException("block, assignment, branch, return", typeToString(getToken().getType()), BT);
}

ASTNode Parser::block(int level){
    print(level, "BLOCK");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    
    ASTNode node(Token(BLOCK, ""));
    consume(OCBR); 

    while(getToken().getType() != CCBR){
        if(speculate(level, &Parser::statement))
            node.addChild(statement(level));
        else
            throw ParsingException("statement", typeToString(getToken().getType()), BT);
    }
    consume(CCBR);

    return node;
}

ASTNode Parser::expression(int level){
    print(level, "EXPRESSION");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    if(speculate(level, &Parser::functionDefinition))
        return functionDefinition(level);
    
    if(speculate(level, &Parser::call))
        return call(level);

    if(speculate(level, &Parser::infixOperation))
        return infixOperation(level);

    if(getToken().getType() == NUMLIT)
        return ASTNode(consume(NUMLIT));

    if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

    throw ParsingException("assignment, infix operation, function definition, call, identifier", typeToString(getToken().getType()), BT);
}

ASTNode Parser::branch(int level){
    print(level, "BRANCH");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    ASTNode node(ASTNode(consume(BRANCH)));

    if(speculate(level, &Parser::expression))
        node.addChild(expression(level));
    else
        throw ParsingException("expression", typeToString(getToken().getType()), BT);

    if(speculate(level, &Parser::block))
        node.addChild(block(level));
    else
        throw ParsingException("block", typeToString(getToken().getType()), BT);

    return node;
}

ASTNode Parser::ret(int level){
    print(level, "RETURN");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    ASTNode node(ASTNode(consume(RETURN)));
    if(speculate(level, &Parser::expression))
        node.addChild(expression(level));
    else
        throw ParsingException("expression", typeToString(getToken().getType()), BT);

    return node;
}

ASTNode Parser::let(int level){
    print(level, "LET");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    ASTNode node(ASTNode(consume(LET)));
    node.addChild(consume(IDENT));

    return node;
}

//Uses custom algorithm for operator precedence
ASTNode Parser::infixOperation(int level){
    print(level, "INFIXOP");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    bool withinBrackets = false;
    if(getToken().getType() == OBR){
        consume(OBR);
        withinBrackets = true;
    }

    bool expectingOperand = true;

    std::vector<ASTNode> stack;
    while(true){
        //OPERAND
        if(speculate(level, &Parser::operand) && expectingOperand){
            stack.push_back(operand(level));
            expectingOperand = false;
        }
        //OPERATOR
        else if(getToken().getType() == INFOP && !expectingOperand){
            stack.push_back(ASTNode(consume(INFOP)));
            expectingOperand = true;
        }
        //Open brackets
        else if(getToken().getType() == OBR){
            stack.push_back(infixOperation(level));
            expectingOperand = false;
        }
        else{
            break;    
        }
    }

    if(withinBrackets)
        consume(CBR);

    //std::cout << std::to_string(stack.size()) << " STACK SIZE!!" << std::endl;
    
    if(stack.size() == 0 || stack.size() % 2 == 0)
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
    
    #if defined(VERBOSE)    
    std::cout << std::to_string(stack.size()) << " STACK SIZE" << std::endl;
    #endif

    return stack[0];
}

ASTNode Parser::operand(int level){
    print(level, "OPERAND");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    if(speculate(level, &Parser::let))
        return let(level);

    else if(speculate(level, &Parser::functionDefinition))
        return functionDefinition(level);

    else if(speculate(level, &Parser::call))
        return call(level);

    else if(getToken().getType() == NUMLIT){
        return ASTNode(consume(NUMLIT));
    }

    else if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

    else 
        throw ParsingException("NUMLITERAL, IDENTIFIER, CALL", typeToString(getToken().getType()), BT);    
}

ASTNode Parser::functionDefinition(int level){
    print(level, "FUNCTIONDEF");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

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

    if(speculate(level, &Parser::block))
        node.addChild(block(level));
    else
        throw ParsingException("block", typeToString(getToken().getType()), BT);    

    return node;
}

ASTNode Parser::call(int level){
    print(level, "CALL");
    printStream(level, MAX_TOKEN_PRINT);
    level++;

    ASTNode node(Token(CALL, ""));
    node.addChild(consume(IDENT));

    ASTNode expressionList(Token(EXPRESSIONLIST, ""));
    consume(OBR);
    
    while(getToken().getType() != CBR){
        expressionList.addChild(expression(level));
        if(getToken().getType() == COMMA)
            consume(COMMA);
        else
            break;
    }

    consume(CBR);
    node.addChild(expressionList);

    return node;
}