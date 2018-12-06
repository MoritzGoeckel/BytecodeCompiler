#pragma once

#include "Memory.cpp"
#include <iostream>
#include <string>
#include <vector>

class OptCodeEngine{

    private:

    typedef void (*Operation)(unsigned char* args, Memory& memory, int& nextLine, bool& end);
    std::map<std::string, unsigned char> humanReadableCodes;
    std::vector<Operation> operations;

    public:

    OptCodeEngine(){
        int optCode = 0;

        //LOAD    L   REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(args[0], args[1]);
            nextLine++;
        });
        humanReadableCodes["LOAD"] = optCode++;

        //MOVE    REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(args[1], memory.getRegister(args[0]));
            nextLine++;
        });
        humanReadableCodes["MOVE"] = optCode++;

        //INCR     REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
                memory.setRegister(args[0], memory.getRegister(args[0]) + 1);
                nextLine++;
        });
        humanReadableCodes["INCR"] = optCode++;

        //ADD     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) + memory.getRegister(args[1])
            );
            nextLine++;
        });
        humanReadableCodes["ADD"] = optCode++;

        //SUB     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) - memory.getRegister(args[1])
            );
            nextLine++;
        });
        humanReadableCodes["SUB"] = optCode++;

        //MUL     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) * memory.getRegister(args[1])
            );
            nextLine++;
        });
        humanReadableCodes["MUL"] = optCode++;

        //DIV     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) / memory.getRegister(args[1])
            );
            nextLine++;
        });
        humanReadableCodes["DIV"] = optCode++;

        //OUT     REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            std::cout << memory.getRegister(args[0]) << std::endl;
            nextLine++;
        });
        humanReadableCodes["OUT"] = optCode++;

        //ASK         REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            //TODO: Not implemented
            throw;
        });
        humanReadableCodes["ASK"] = optCode++;

        //CMP     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            int a = memory.getRegister(args[0]);
            int b = memory.getRegister(args[1]);
            if(a == b)
                memory.setEqual();
            if(a > b)
                memory.setGreater();
            if(a < b)
                memory.setLess();
        });
        humanReadableCodes["CMP"] = optCode++;

        //JMPE    L
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            if(memory.getEqual()){
                memory.resetCompare();
                nextLine = args[0];
            }
            else{
                nextLine++;
            }
        });
        humanReadableCodes["JMPE"] = optCode++;
        
        //JMPL    L 
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            if(memory.getLess()){
                memory.resetCompare();
                nextLine = args[0];
            }
            else{
                nextLine++;
            }
        });
        humanReadableCodes["JMPL"] = optCode++;

        //JMPG    L 
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            if(memory.getGreater()){
                memory.resetCompare();
                nextLine = args[0];
            }
            else{
                nextLine++;
            }
        });
        humanReadableCodes["JMPG"] = optCode++;

        //JPGE    L 
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            if(memory.getGreater() && memory.getEqual()){
                memory.resetCompare();
                nextLine = args[0];
            }
            else{
                nextLine++;
            }
        });
        humanReadableCodes["JPGE"] = optCode++;

        //JPLE    L 
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            if(memory.getLess() && memory.getEqual()){
                memory.resetCompare();
                nextLine = args[0];
            }
            else{
                nextLine++;
            }
        });
        humanReadableCodes["JPLE"] = optCode++;

        //JMP     L
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            nextLine = args[0];
        });
        humanReadableCodes["JMP"] = optCode++;

        //DBG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.print();
        });
        humanReadableCodes["DBG"] = optCode++;

        //END
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end) { 
            end = true; 
        });
        humanReadableCodes["END"] = optCode++;

        //AND     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) > 0 && memory.getRegister(args[1]) > 0 ? 1 : 0
            );
            nextLine++;
        });
        humanReadableCodes["AND"] = optCode++;

        //OR      REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) > 0 || memory.getRegister(args[1]) > 0 ? 1 : 0
            );
            nextLine++;
        });
        humanReadableCodes["OR"] = optCode++;

        //XOR     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) > 0 ^ memory.getRegister(args[1]) > 0 ? 1 : 0
            );
            nextLine++;
        });
        humanReadableCodes["XOR"] = optCode++;

        //NOT     REG REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            memory.setRegister(
                args[0], 
                memory.getRegister(args[0]) > 0 ? 0 : 1
            );
            nextLine++;
        });
        humanReadableCodes["NOT"] = optCode++;

        //PUSH    REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            //TODO: Not implemented
            throw;
        });
        humanReadableCodes["PUSH"] = optCode++;

        //POP         REG
        operations.push_back([](unsigned char* args, Memory& memory, int& nextLine, bool& end){
            //TODO: Not implemented
            throw;
        });
        humanReadableCodes["POP"] = optCode++;

        //LABEL   L
        //humanReadableCodes["LABEL"] = optCode++;

        //; COMMENT
        //humanReadableCodes[";"] = optCode++;
    }

    unsigned char encodeOptString(std::string str){
        if(str == "LABEL" || str == ";")
            throw; //Label and comments should be removed by now

        return humanReadableCodes[str];
    }

    Operation getOperation(unsigned char code){
        return operations[code];
    }

};