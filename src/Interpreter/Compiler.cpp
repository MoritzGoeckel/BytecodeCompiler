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
    std::string Compiler::funDef(const ASTNode& node, std::string varname);


    public:
    Compiler(){
        endl = "\n"; //TODO: Should be system unspecific
        rs = RegisterManager();
    }

    std::string compile(const ASTNode& node);
};

std::string Compiler::compile(const ASTNode& node){
    return emit(node) + "END";
}

std::string Compiler::block(const ASTNode& node){
    //std::cout << "BLOCK" << std::endl;

    std::string code = "";
    for(int i = 0; i < node.getChildCount(); i++){
        code += emit(node.getChild(i));
    }
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
    std::string nodeText = node.getToken().getText();

    //Assignment
    if(nodeText == "="){
        std::string actualVarname = "";
        
        if(node.getChild(0).getTokenType() == IDENT){
            actualVarname = node.getChild(0).getToken().getText();
        }else if(node.getChild(0).getTokenType() == LET){
            actualVarname = node.getChild(0).getChild(0).getToken().getText();
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
    //Todo: Write to functionlist with unique name, put the reference into varname
    
    //Function:

    //RETURN
    //POP %b
    //POP %a
    //For all in identlist

    //Assign to variable (Should have a specific type?)

    //LOAD UniqueLabel %varname
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

    if(ident == "print"){
        const std::string tmpVar = rs.borrow();
        for(int i = 0; i < parameterCount; i++){
            code += "POP %" + tmpVar + endl;
            code += "OUT %" + tmpVar + endl;
        }
        rs.giveBack(tmpVar);

        return code;
    }

    else {
        //Todo: Call to content of ident
        //CALLV WEN????
    }

    //And here the actual functions

    //POP %varname
    //Pop after call to varname

    throw std::runtime_error("Not implemented " + node.getToken().getText() + BT);
}

std::string Compiler::expression(const ASTNode& node, std::string varname){
    if(node.getTokenType() == INFOP){
        return infop(node, varname);
    }

    if(node.getTokenType() == NUMLIT){
        return numlit(node, varname);
    }

    if(node.getTokenType() == CALL){
        return call(node, varname);
    }

    if(node.getTokenType() == FUNDEF){
        return funDef(node, varname);
    }

    if(node.getTokenType() == IDENT){
        //Todo: This is not the most efficient way
        return "COPY %" + node.getToken().getText() + " %" + varname + endl;
    }

    throw std::runtime_error("Unexpected node: " + node.getToken().getText() + BT);
}

std::string Compiler::emit(const ASTNode& node){
    //std::cout << "COMP" << std::endl;
    return emit(node, "NONE");
}

std::string Compiler::emit(const ASTNode& node, std::string varname){
    //std::cout << "COMP varname" << std::endl;
    int type = node.getTokenType();

    if(type == BLOCK){
        return block(node);
    }

    if(type == INFOP || type == NUMLIT || type == CALL || type == IDENT){
        return expression(node, varname);
    }

    throw std::runtime_error("Unexpected node: " + node.getToken().getText() + BT);
}