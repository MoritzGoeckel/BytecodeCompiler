#pragma once

#include "ASTNode.cpp"
#include "Token.cpp"
#include "RegisterManager.cpp"
#include "../ErrorHandling.cpp"

#include <string>
#include <vector>

class Compiler{
    private:
    std::string endl;
    RegisterManager rs;

    std::string emit(const ASTNode& node);
    std::string emit(const ASTNode& node, std::string varname);
    std::string block(const ASTNode& node);
    std::string numlit(const ASTNode& node, std::string varname);
    std::string infop(const ASTNode& node, std::string varname);
    std::string call(const ASTNode& node, std::string varname);
    std::string expression(const ASTNode& node, std::string varname);
    std::string funDef(const ASTNode& node, std::string varname);
    std::string ret(const ASTNode& node);
    
    public:
    Compiler(){
        endl = "\n"; //TODO: Should be system unspecific
        rs = RegisterManager();
    }

    std::string compile(const std::vector<ASTNode> functions, const ASTNode main);
};

std::string Compiler::compile(const std::vector<ASTNode> functions, const ASTNode main){
    std::string output("");

    output += emit(main, "main");
    output += "END" + endl;
    output += "##############" + endl;

    for(size_t i = 0; i < functions.size(); i++){
        output += emit(functions[i], "fn" + std::to_string(i));
        output += "##############" + endl;
    }

    // Removing the first &main:
    output.erase(0, 6);
    return output;
}

std::string Compiler::block(const ASTNode& node){
    //std::cout << "BLOCK" << std::endl;

    std::string code = "";
    for(size_t i = 0; i < node.getChildCount(); i++){
        code += emit(node.getChild(i));
    }
    return code;
}

std::string Compiler::ret(const ASTNode& node){
    std::string code("");
    const std::string tmpVar = rs.borrow();
    for(size_t i = 0; i < node.getChildCount(); i++){
        code += emit(node.getChild(i), tmpVar);
    }
    code += "PUSH %" + tmpVar + endl;
    rs.giveBack(tmpVar);
    code += "RETURN" + endl;

    return code;
}

std::string Compiler::numlit(const ASTNode& node, std::string varname){
    //std::cout << "NUMLIT" << std::endl;

    if(varname == "NONE")
        return "";

    std::string value = node.getToken().getText();
    return "LOAD " + value + " " + "%" + varname + endl;
}

std::string Compiler::infop(const ASTNode& node, std::string varname){
    std::string nodeText = node.getText();

    //Assignment
    if(nodeText == "="){
        std::string actualVarname = "";
        
        if(node.getChild(0).getTokenType() == TokenType::IDENT){
            actualVarname = node.getChild(0).getText();
        }else if(node.getChild(0).getTokenType() == TokenType::LET){
            actualVarname = node.getChild(0).getChild(0).getText();
        }else{
            throw std::runtime_error("Expecting IDENT or LET on the left side" + BT);
        }

        return emit(node.getChild(1), actualVarname);
    }

    //ADD, SUB, MUL, DIV
    if(nodeText == "+" || nodeText == "*" || nodeText == "/" || nodeText == "-"){
        std::string optcode = "";
        if(nodeText == "+")
            optcode = "ADD";
        else if(nodeText == "-")
            optcode = "SUB";
        else if(nodeText == "*")
            optcode = "MUL";
        else if(nodeText == "/")
            optcode = "DIV";
        else
            throw std::runtime_error("Unknown operator text " + nodeText + BT);

        std::string x = rs.borrow();
        //std::string y = rs.borrow();

        std::string code = emit(node.getChild(0), x);
        code += emit(node.getChild(1), varname);
        code += optcode + " %" + x + " %" +  varname + " %" + varname + endl;

        //code += compile(node.getChild(1), y);
        //code += optcode + " " + x + " " + y + " " + varname + endl;

        //Todo: Remove commented code if Im sure it works

        rs.giveBack(x);
        //rs.giveBack(y);

        return code;
    }

    throw std::runtime_error("Unexpected text: " + node.getToken().getText() + BT);
}

std::string Compiler::funDef(const ASTNode& node, std::string varname){
    std::string output("&" + varname + ":" + endl);
    ASTNode identlist = node.getChild(0);
    if(identlist.getTokenType() != TokenType::IDENTLIST){
        throw std::runtime_error("Expected token type IDENTLIST but got " + node.getToken().getText() + BT);
    }

    std::string pops("");
    for(const auto& ident : identlist.getChildren()){
        pops = "POP %" + ident.getText() + endl + pops;
    }
    output += pops;

    output += emit(node.getChild(1));

    return output;
}

std::string Compiler::call(const ASTNode& node, std::string varname){
    std::string ident = node.getChild(0).getToken().getText();

    //Evaluate parameter expressions and pushing for later use
    const ASTNode& exprlist = node.getChild(1);
    const std::string tmpVar = rs.borrow();
    int parameterCount = exprlist.getChildCount();
    std::string code = "";
    //Evaluating the last parameter first to maintain order
    //  while using the stack
    for(int i = parameterCount - 1; i >= 0; i--){
        code += expression(exprlist.getChild(i), tmpVar);
        code += "PUSH %" + tmpVar + endl;
    }
    rs.giveBack(tmpVar);

    //Actual function call

    if(ident == "print")
    {
        const std::string tmpVar = rs.borrow();
        for(int i = 0; i < parameterCount; i++){
            code += "POP %" + tmpVar + endl;
            code += "OUT %" + tmpVar + endl;
        }
        rs.giveBack(tmpVar);

        // Thats returning 0 (void) for print
        code += "LOAD 0 %" + tmpVar + endl;
        code += "MOVE %" + tmpVar + " %" + varname + endl;
        return code;
    }
    else 
    {
        //Todo: Call to content of ident
        //CALLV WEN????

        //Call
        code += "CALLV %" + ident + endl;

        //Get return
        //Only if returns something
        const std::string tmpVar2 = rs.borrow();
        code += "POP %" + tmpVar2 + endl;
        code += "MOVE %" + tmpVar2 + " %" + varname + endl;
        rs.giveBack(tmpVar2);
    }

    return code;
}

std::string Compiler::expression(const ASTNode& node, std::string varname){
    if(node.getTokenType() == TokenType::INFOP){
        return infop(node, varname);
    }

    if(node.getTokenType() == TokenType::NUMLIT){
        return numlit(node, varname);
    }

    if(node.getTokenType() == TokenType::CALL){
        return call(node, varname);
    }

    if(node.getTokenType() == TokenType::FUNDEF){
        return funDef(node, varname);
    }

    if(node.getTokenType() == TokenType::IDENT){
        //Todo: This is not the most efficient way
        return "MOVE %" + node.getToken().getText() + " %" + varname + endl;
    }

    if(node.getTokenType() == TokenType::FNREF){
        //Todo: This is not the most efficient way
        return "LOAD &fn" + node.getText() + " %" + varname + endl;
    }

    throw std::runtime_error("Unexpected node: " + node.getToken().getPrintString() + BT);
}

std::string Compiler::emit(const ASTNode& node){
    return emit(node, "NONE");
}

std::string Compiler::emit(const ASTNode& node, std::string varname){
    int type = node.getTokenType();

    if(type == TokenType::BLOCK){
        return block(node);
    }

    if(    type == TokenType::INFOP 
        || type == TokenType::NUMLIT 
        || type == TokenType::CALL 
        || type == TokenType::IDENT 
        || type == TokenType::FUNDEF
        || type == TokenType::FNREF){
        return expression(node, varname);
    }

    if(type == TokenType::RETURN){
        return ret(node);
    }

    throw std::runtime_error("Unexpected node: " + node.getToken().getPrintString() + BT);
}