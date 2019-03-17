#pragma once

#include <string>
#include <vector>
#include <map>

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
        this->index = 0u;
    } 

    ASTNode parse(){
        return statement(0); 
    }

    private:
    
    //[RuleId][Index] -> StopTokenIndex
    std::map<size_t, std::map<std::string, int>> cache;

    std::vector<Token> tokens;
    std::vector<size_t> markers;
    size_t index;

    inline const Token& getToken() const{
        return LA(0);
    }

    inline const Token& LA(size_t offset) const{
        #if defined(VERBOSE)
        if(index + offset >= tokens.size())
            std::cout << "Index out of bounds: " << BT;
        #endif

        return tokens[index + offset];
    }

    inline const size_t LAType(size_t offset) const{
        return LA(offset).getType();
    }

    inline const bool LATypeIs(size_t offset, size_t type) const{
        if(index + offset >= tokens.size())
            return false;

        return LAType(offset) == type;
    }
    
    const int PARSE_SUCCESS = -3;
    const int CACHE_MISS = -2;
    const int FAILED_TO_PARSE = -1;
    inline void memorize(size_t index, std::string ruleId, int result){
        cache[index][ruleId] = result; //Result could be stop index
    }

    inline int checkCache(size_t index, std::string ruleId){
        if(cache.find(index) == cache.end()){
            return CACHE_MISS;
        }

        if(cache[index].find(ruleId) == cache[index].end()){
            return CACHE_MISS;
        }
        
        return cache[index][ruleId];
    }

    #if defined(VERBOSE)
    inline void printStream(int level, int max, std::string name) const{
        std::cout << " >> " << name << " ";
        printStream(level, max);
    }
    #endif

    #if defined(VERBOSE)
    inline void printStream(int level, int max) const{
        for(int i = 0; i < level; i++)
            std::cout << LEVEL_CHARS;

        std::cout << " >> ";

        for(int i = index; i < index + max && i < tokens.size(); i++)
            tokens[i].print();
        std::cout << std::endl;
    }
    #endif

    #if defined(VERBOSE)
    inline void print(int level, std::string str){
        for(int i = 0; i < level; i++)
            std::cout << LEVEL_CHARS;

        std::cout << str << std::endl;
    }
    #endif

    inline const Token& consume(){
        const Token& token = getToken();
        index++;

        //TODO: Maybe we need this check here...
        #if defined(VERBOSE)
        if(index > tokens.size()){
            std::cout << "Index out of bounds: " << BT;
            throw ParsingException("ANY", "END OF TOKENS", BT);
        }
        #endif

        return token;
    }

    inline const Token& consume(int tokenType){
        if(getToken().getType() == tokenType){
            return consume();
        }

        throw ParsingException(typeToString(tokenType), typeToString(getToken().getType()), BT);
    }

    inline void mark(){
        markers.push_back(this->index); 
    }

    inline void reset(){
        this->index = markers.back();
        markers.pop_back();
    }

    ASTNode block(size_t level);
    ASTNode statement(size_t level);
    ASTNode expression(size_t level);

    ASTNode let(size_t level);

    ASTNode branch(size_t level);
    ASTNode ret(size_t level);
    ASTNode infixOperation(size_t level);
    ASTNode functionDefinition(size_t level);
    ASTNode call(size_t level);
    ASTNode operand(size_t level);

    typedef ASTNode (Parser::*ParserFn)(size_t);
    bool speculate(size_t level, ParserFn fn, std::string ruleId);
};

//TODO: Speculation could be done with templates to make it faster
inline bool Parser::speculate(size_t level, ParserFn fn, std::string ruleId){
    bool success = true;
    size_t startIndex = this->index;

    const int cacheResult = checkCache(startIndex, ruleId);
    
    //Already parsed and unsuccessful
    if(cacheResult == FAILED_TO_PARSE){
        return false;
    }

    //Success
    if(cacheResult == PARSE_SUCCESS){
    //if(cacheResult != CACHE_MISS && cacheResult != FAILED_TO_PARSE){
        //skipTo(cacheResult); //This should be like this ... thats a problem.
        //Would like to skip non speculates too
        //Whu do i need stop index if i dont use it
        return true;
    }
    
    //Cache miss
    if(cacheResult == CACHE_MISS){
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
        //int stopIndex = static_cast<int>(this->index);
        reset();

        //ASSERT if(cacheResult == PARSE_SUCCESS && success == false)
        //ASSERT if(cacheResult == FAILED_TO_PARSE && success)

        memorize(
            startIndex, 
            ruleId, 
            success ? PARSE_SUCCESS : FAILED_TO_PARSE
        );
    }

    return success;
}

ASTNode Parser::statement(size_t level){
    #if defined(VERBOSE)
    print(level, "STATEMENT");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    if(LAType(0) == OCBR && speculate(level, &Parser::block, "block"))
        return block(level);

    else if(LAType(0) == BRANCH && speculate(level, &Parser::branch, "branch"))
        return branch(level);

    else if(LAType(0) == RETURN && speculate(level, &Parser::ret, "ret")){
        ASTNode node = ret(level);
        consume(SEMICOLON);
        return node;
    }

    else if(speculate(level, &Parser::expression, "expression")){
        ASTNode node = expression(level);
        consume(SEMICOLON);
        return node;
    }

    else if(LAType(0) == LET && speculate(level, &Parser::let, "let"))
        return let(level);

    else
        throw ParsingException("block, assignment, branch, return", typeToString(getToken().getType()), BT);
}

ASTNode Parser::block(size_t level){
    #if defined(VERBOSE)
    print(level, "BLOCK");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif
    
    consume(OCBR); 
    ASTNode node(Token(BLOCK, ""));

    while(getToken().getType() != CCBR){
        if(speculate(level, &Parser::statement, "statement"))
            node.addChild(statement(level));
        else
            throw ParsingException("statement", typeToString(getToken().getType()), BT);
    }
    consume(CCBR);

    return node;
}

ASTNode Parser::expression(size_t level){
    #if defined(VERBOSE)
    print(level, "EXPRESSION");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    if(LAType(0) == OBR 
    && (LATypeIs(1, IDENT) || LATypeIs(1, CBR)) 
    && speculate(level, &Parser::functionDefinition, "functionDefinition"))
        return functionDefinition(level);
    
    if(LAType(0) == IDENT && speculate(level, &Parser::call, "call"))
        return call(level);

    if(speculate(level, &Parser::infixOperation, "infixOperation"))
        return infixOperation(level);

    if(getToken().getType() == NUMLIT)
        return ASTNode(consume(NUMLIT));

    if(getToken().getType() == IDENT)
        return ASTNode(consume(IDENT));

    throw ParsingException("assignment, infix operation, function definition, call, identifier", typeToString(getToken().getType()), BT);
}

ASTNode Parser::branch(size_t level){
    #if defined(VERBOSE)
    print(level, "BRANCH");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    ASTNode node(ASTNode(consume(BRANCH)));

    if(speculate(level, &Parser::expression, "expression"))
        node.addChild(expression(level));
    else
        throw ParsingException("expression", typeToString(getToken().getType()), BT);

    if(speculate(level, &Parser::statement, "statement"))
        node.addChild(statement(level));
    else
        throw ParsingException("statement", typeToString(getToken().getType()), BT);

    return node;
}

ASTNode Parser::ret(size_t level){
    #if defined(VERBOSE)
    print(level, "RETURN");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    ASTNode node(ASTNode(consume(RETURN)));

    if(getToken().getType() == SEMICOLON){
        return node;
    }

    if(speculate(level, &Parser::expression, "expression")){
        node.addChild(expression(level));
        return node;
    }
    
    throw ParsingException("expression", typeToString(getToken().getType()), BT);
}

ASTNode Parser::let(size_t level){
    #if defined(VERBOSE)
    print(level, "LET");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    ASTNode node(ASTNode(consume(LET)));
    node.addChild(consume(IDENT));

    return node;
}

//Uses custom algorithm for operator precedence
ASTNode Parser::infixOperation(size_t level){
    #if defined(VERBOSE)
    print(level, "INFIXOP");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    bool withinBrackets = false;
    if(LAType(0) == OBR){
        consume(OBR);
        withinBrackets = true;
    }

    bool expectingOperand = true;

    std::vector<ASTNode> stack;
    while(true){
        if(!expectingOperand && (getToken().getType() == CBR || getToken().getType() == SEMICOLON)){
            break;
        }
        //OPERATOR
        else if(LAType(0) == INFOP && !expectingOperand){
            stack.push_back(ASTNode(consume(INFOP)));
            expectingOperand = true;
        }
        //OPERAND
        else if(speculate(level, &Parser::operand, "operand") && expectingOperand){
            stack.push_back(operand(level));
            expectingOperand = false;
        }
        //Open brackets
        else if(LAType(0) == OBR){
            stack.push_back(infixOperation(level));
            expectingOperand = false;
        }
        else{
            break;    
        }
    }

    if(withinBrackets)
        consume(CBR);
    
    if(stack.size() == 0 || stack.size() % 2 == 0)
        throw ParsingException("BAD STACK SIZE", "", BT);

    //Reducing stack until only one element is left
    while(stack.size() > 1){
        //Iterating over the operators
        for(size_t i = 1u; i < stack.size(); i += 2u){
            if(i + 2u >= stack.size() || stack[i].getToken().getPrecedence() <= stack[i + 2].getToken().getPrecedence()){
                ASTNode node = stack[i];
                node.addChild(stack[i - 1u]);
                node.addChild(stack[i + 1u]);

                stack.erase(stack.begin() + i - 1u, stack.begin() + i + 2u);
                stack.insert(stack.begin() + i - 1u, node);
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

ASTNode Parser::operand(size_t level){
    #if defined(VERBOSE)
    print(level, "OPERAND");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    if(LAType(0) == LET && speculate(level, &Parser::let, "let"))
        return let(level);

    else if(LAType(0) == OBR 
    && (LATypeIs(1, IDENT) || LATypeIs(1, CBR)) 
    && speculate(level, &Parser::functionDefinition, "functionDefinition"))
        return functionDefinition(level);

    else if(LATypeIs(0, IDENT) && speculate(level, &Parser::call, "call"))
        return call(level);

    else if(LATypeIs(0, NUMLIT)){
        return ASTNode(consume(NUMLIT));
    }

    else if(LATypeIs(0, IDENT))
        return ASTNode(consume(IDENT));

    else 
        throw ParsingException("NUMLITERAL, IDENTIFIER, CALL", typeToString(getToken().getType()), BT);    
}

ASTNode Parser::functionDefinition(size_t level){
    #if defined(VERBOSE)
    print(level, "FUNCTIONDEF");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

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

    if(LAType(0) == OCBR && speculate(level, &Parser::block, "block"))
        node.addChild(block(level));
    else
        throw ParsingException("block", typeToString(getToken().getType()), BT);    

    return node;
}

ASTNode Parser::call(size_t level){
    #if defined(VERBOSE)
    print(level, "CALL");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

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