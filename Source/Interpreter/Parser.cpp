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

#define VERBOSE

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

    enum Rule {BLOCK, STATEMENT, EXPRESSION, LET, BRANCH, RET, INFIX_OPERATION, FUNCTION_DEFINITION, CALL, OPERAND};    
    enum CacheResult {SUCCESS, FAILURE, MISS};

    //[RuleId][Index] -> StopTokenIndex
    std::map<size_t, std::map<Rule, CacheResult>> cache;

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
    
    inline void memorize(size_t index, Rule ruleId, CacheResult result){
        cache[index][ruleId] = result; //Result could be stop index
    }

    inline CacheResult checkCache(size_t index, Rule ruleId){
        if(cache.find(index) == cache.end()){
            return MISS;
        }

        if(cache[index].find(ruleId) == cache[index].end()){
            return MISS;
        }
        
        return cache[index][ruleId];
    }

    #if defined(VERBOSE)
    inline void printStream(size_t level, size_t max, std::string name) const{
        std::cout << " >> " << name << " ";
        printStream(level, max);
    }
    #endif

    #if defined(VERBOSE)
    inline void printStream(size_t level, size_t max) const{
        for(size_t i = 0u; i < level; i++)
            std::cout << LEVEL_CHARS;

        std::cout << " >> ";

        for(size_t i = index; i < index + max && i < tokens.size(); i++)
            tokens[i].print();
        std::cout << std::endl;
    }
    #endif

    #if defined(VERBOSE)
    inline void print(size_t level, std::string str){
        for(size_t i = 0u; i < level; i++)
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
    bool speculate(size_t level, ParserFn fn, Rule ruleId);
};

//TODO: Speculation could be done with templates to make it faster
inline bool Parser::speculate(size_t level, ParserFn fn, Rule ruleId){
    std::cout << ruleId << std::endl;

    bool success = true;
    size_t startIndex = this->index;

    const CacheResult cacheResult = checkCache(startIndex, ruleId);
    
    if(ruleId == Rule::LET){
        std::cout << "LET " << cacheResult  << std::endl;
    }

    //Already parsed and unsuccessful
    if(cacheResult == FAILURE){
        return false;
    }

    //Success
    if(cacheResult == SUCCESS){
    //if(cacheResult != CACHE_MISS && cacheResult != FAILED_TO_PARSE){
        //skipTo(cacheResult); //This should be like this ... thats a problem.
        //Would like to skip non speculates too
        //Whu do i need stop index if i dont use it
        return true;
    }
    
    //Cache miss
    if(cacheResult == MISS){
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
            success ? SUCCESS : FAILURE
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

    if(LAType(0) == TokenTypes::OCBR && speculate(level, &Parser::block, Rule::BLOCK))
        return block(level);

    else if(LAType(0) == TokenTypes::BRANCH && speculate(level, &Parser::branch, Rule::BRANCH))
        return branch(level);

    else if(LAType(0) == TokenTypes::RETURN && speculate(level, &Parser::ret, Rule::RET)){
        ASTNode node = ret(level);
        consume(TokenTypes::SEMICOLON);
        return node;
    }

    else if(speculate(level, &Parser::expression, Rule::EXPRESSION)){
        ASTNode node = expression(level);
        consume(TokenTypes::SEMICOLON);
        return node;
    }

    else if(LAType(0) == TokenTypes::LET && speculate(level, &Parser::let, Rule::LET))
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
    
    consume(TokenTypes::OCBR); 
    ASTNode node(Token(TokenTypes::BLOCK, ""));

    while(getToken().getType() != TokenTypes::CCBR){
        if(speculate(level, &Parser::statement, Rule::STATEMENT))
            node.addChild(statement(level));
        else
            throw ParsingException("statement", typeToString(getToken().getType()), BT);
    }
    consume(TokenTypes::CCBR);

    return node;
}

ASTNode Parser::expression(size_t level){
    #if defined(VERBOSE)
    print(level, "EXPRESSION");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    if(LAType(0) == TokenTypes::OBR 
    && (LATypeIs(1, TokenTypes::IDENT) || LATypeIs(1, TokenTypes::CBR)) 
    && speculate(level, &Parser::functionDefinition, Rule::FUNCTION_DEFINITION))
        return functionDefinition(level);
    
    if(LAType(0) == TokenTypes::IDENT && speculate(level, &Parser::call, Rule::CALL))
        return call(level);

    if(speculate(level, &Parser::infixOperation, Rule::INFIX_OPERATION))
        return infixOperation(level);

    if(getToken().getType() == TokenTypes::NUMLIT)
        return ASTNode(consume(TokenTypes::NUMLIT));

    if(getToken().getType() == TokenTypes::IDENT)
        return ASTNode(consume(TokenTypes::IDENT));

    throw ParsingException("assignment, infix operation, function definition, call, identifier", typeToString(getToken().getType()), BT);
}

ASTNode Parser::branch(size_t level){
    #if defined(VERBOSE)
    print(level, "BRANCH");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    ASTNode node(ASTNode(consume(TokenTypes::BRANCH)));

    if(speculate(level, &Parser::expression, Rule::EXPRESSION))
        node.addChild(expression(level));
    else
        throw ParsingException("expression", typeToString(getToken().getType()), BT);

    if(speculate(level, &Parser::statement, Rule::STATEMENT))
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

    ASTNode node(ASTNode(consume(TokenTypes::RETURN)));

    if(getToken().getType() == TokenTypes::SEMICOLON){
        return node;
    }

    if(speculate(level, &Parser::expression, Rule::EXPRESSION)){
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

    ASTNode node(ASTNode(consume(TokenTypes::LET)));
    node.addChild(consume(TokenTypes::IDENT));

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
    if(LAType(0) == TokenTypes::OBR){
        consume(TokenTypes::OBR);
        withinBrackets = true;
    }

    bool expectingOperand = true;

    std::vector<ASTNode> stack;
    while(true){
        if(!expectingOperand && (getToken().getType() == TokenTypes::CBR || getToken().getType() == TokenTypes::SEMICOLON)){
            break;
        }
        //OPERATOR
        else if(LAType(0) == TokenTypes::INFOP && !expectingOperand){
            stack.push_back(ASTNode(consume(TokenTypes::INFOP)));
            expectingOperand = true;
        }
        //OPERAND
        else if(speculate(level, &Parser::operand, Rule::OPERAND) && expectingOperand){
            stack.push_back(operand(level));
            expectingOperand = false;
        }
        //Open brackets
        else if(LAType(0) == TokenTypes::OBR){
            stack.push_back(infixOperation(level));
            expectingOperand = false;
        }
        else{
            break;    
        }
    }

    if(withinBrackets)
        consume(TokenTypes::CBR);
    
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

    if(LAType(0) == TokenTypes::LET && speculate(level, &Parser::let, Rule::LET))
        return let(level);

    else if(LAType(0) == TokenTypes::OBR 
    && (LATypeIs(1, TokenTypes::IDENT) || LATypeIs(1, TokenTypes::CBR)) 
    && speculate(level, &Parser::functionDefinition, Rule::FUNCTION_DEFINITION))
        return functionDefinition(level);

    else if(LATypeIs(0, TokenTypes::IDENT) && speculate(level, &Parser::call, Rule::CALL))
        return call(level);

    else if(LATypeIs(0, TokenTypes::NUMLIT)){
        return ASTNode(consume(TokenTypes::NUMLIT));
    }

    else if(LATypeIs(0, TokenTypes::IDENT))
        return ASTNode(consume(TokenTypes::IDENT));

    else 
        throw ParsingException("NUMLITERAL, IDENTIFIER, CALL", typeToString(getToken().getType()), BT);    
}

ASTNode Parser::functionDefinition(size_t level){
    #if defined(VERBOSE)
    print(level, "FUNCTIONDEF");
    printStream(level, MAX_TOKEN_PRINT);
    level++;
    #endif

    ASTNode identifierList(Token(TokenTypes::IDENTLIST, ""));

    consume(TokenTypes::OBR);
    while(getToken().getType() == TokenTypes::IDENT){
        identifierList.addChild(consume(TokenTypes::IDENT));
        if(getToken().getType() == TokenTypes::COMMA)
            consume(TokenTypes::COMMA);
        else
            break;
    }
    consume(TokenTypes::CBR);

    ASTNode node(Token(TokenTypes::FUNDEF, ""));
    node.addChild(identifierList);

    if(LAType(0) == TokenTypes::OCBR && speculate(level, &Parser::block, Rule::BLOCK))
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

    ASTNode node(Token(TokenTypes::CALL, ""));
    node.addChild(consume(TokenTypes::IDENT));

    ASTNode expressionList(Token(TokenTypes::EXPRESSIONLIST, ""));
    consume(TokenTypes::OBR);
    
    while(getToken().getType() != TokenTypes::CBR){
        expressionList.addChild(expression(level));
        if(getToken().getType() == TokenTypes::COMMA)
            consume(TokenTypes::COMMA);
        else
            break;
    }

    consume(TokenTypes::CBR);
    node.addChild(expressionList);

    return node;
}