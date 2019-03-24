#pragma once

#include "Memory.cpp"
#include <iostream>
#include <string>
#include <vector>

#include "Types.cpp"
#include "../ErrorHandling.cpp"

#define OPTIMIZED

class OptCodeEngine{

    private:

    std::map<std::string, int8> humanReadableCodes;
    std::map<int8, std::string> optToReadable;

    typedef void (*Operation)(int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end);
    std::vector<Operation> operations;

    public:

    OptCodeEngine(){
        int8 optCode = 0;

        //LOAD    L   REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){            
            memory.setRegister(statementPtr[1], statementPtr[0]);
            nextStatementIndex += 3;
        });
        humanReadableCodes["LOAD"] = optCode++;

        //MOVE    REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(statementPtr[1], memory.getRegister(statementPtr[0]));
            nextStatementIndex += 3;
        });
        humanReadableCodes["MOVE"] = optCode++;

        //INCR    REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
                memory.setRegister(statementPtr[0], memory.getRegister(statementPtr[0]) + 1);
                nextStatementIndex += 2;
        });
        humanReadableCodes["INCR"] = optCode++;

        //ADD     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[2], 
                memory.getRegister(statementPtr[0]) + memory.getRegister(statementPtr[1])
            );
            nextStatementIndex += 4;
        });
        humanReadableCodes["ADD"] = optCode++;

        //SUB     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[2], 
                memory.getRegister(statementPtr[0]) - memory.getRegister(statementPtr[1])
            );
            nextStatementIndex += 4;
        });
        humanReadableCodes["SUB"] = optCode++;

        //MUL     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[2], 
                memory.getRegister(statementPtr[0]) * memory.getRegister(statementPtr[1])
            );
            nextStatementIndex += 4;
        });
        humanReadableCodes["MUL"] = optCode++;

        //DIV     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[2], 
                memory.getRegister(statementPtr[0]) / memory.getRegister(statementPtr[1])
            );
            nextStatementIndex += 4;
        });
        humanReadableCodes["DIV"] = optCode++;

        //OUT     REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            std::cout << memory.getRegister(statementPtr[0]) << std::endl;
            nextStatementIndex += 2;
        });
        humanReadableCodes["OUT"] = optCode++;

        //ASK     REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            throw std::runtime_error("Not implemented" + BT); //TODO: Not implemented
            nextStatementIndex += 2;
        });
        humanReadableCodes["ASK"] = optCode++;

        //CMPE     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            int a = memory.getRegister(statementPtr[0]);
            int b = memory.getRegister(statementPtr[1]);
            memory.setRegister(statementPtr[2], a == b ? 1 : 0);
            nextStatementIndex += 4;          
        });
        humanReadableCodes["CMPE"] = optCode++;
        
        //CMPL     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            int a = memory.getRegister(statementPtr[0]);
            int b = memory.getRegister(statementPtr[1]);
            memory.setRegister(statementPtr[2], a < b ? 1 : 0);
            nextStatementIndex += 4;          
        });
        humanReadableCodes["CMPL"] = optCode++;

        //CMLE     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            int a = memory.getRegister(statementPtr[0]);
            int b = memory.getRegister(statementPtr[1]);
            memory.setRegister(statementPtr[2], a <= b ? 1 : 0);
            nextStatementIndex += 4;         
        });
        humanReadableCodes["CMLE"] = optCode++;

        //CMPG     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            int a = memory.getRegister(statementPtr[0]);
            int b = memory.getRegister(statementPtr[1]);
            memory.setRegister(statementPtr[2], a > b ? 1 : 0);
            nextStatementIndex += 4;          
        });
        humanReadableCodes["CMPG"] = optCode++;

        //CMGE     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            int a = memory.getRegister(statementPtr[0]);
            int b = memory.getRegister(statementPtr[1]);
            memory.setRegister(statementPtr[2], a >= b ? 1 : 0);
            nextStatementIndex += 4;          
        });
        humanReadableCodes["CMGE"] = optCode++;

        //JMP     L
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            nextStatementIndex = statementPtr[0];
        });
        humanReadableCodes["JMP"] = optCode++;

        //JMPC     L REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            if(memory.getRegister(statementPtr[1]) > 0)
                nextStatementIndex = statementPtr[0];
            else
                nextStatementIndex += 3;   
        });
        humanReadableCodes["JMPC"] = optCode++;

        //DBG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.print();
            nextStatementIndex += 1;   
        });
        humanReadableCodes["DBG"] = optCode++;

        //END
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end) { 
            end = true; 
        });
        humanReadableCodes["END"] = optCode++;

        //AND     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[0], 
                memory.getRegister(statementPtr[0]) > 0 && memory.getRegister(statementPtr[1]) > 0 ? 1 : 0
            );
            nextStatementIndex += 4;   
        });
        humanReadableCodes["AND"] = optCode++;

        //OR      REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[0], 
                memory.getRegister(statementPtr[0]) > 0 || memory.getRegister(statementPtr[1]) > 0 ? 1 : 0
            );
            nextStatementIndex += 4;   
        });
        humanReadableCodes["OR"] = optCode++;

        //XOR     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[0], 
                (memory.getRegister(statementPtr[0]) > 0) ^ (memory.getRegister(statementPtr[1]) > 0) ? 1 : 0
            );
            nextStatementIndex += 4;
        });
        humanReadableCodes["XOR"] = optCode++;

        //NOT     REG REG REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[0], 
                memory.getRegister(statementPtr[0]) > 0 ? 0 : 1
            );
            nextStatementIndex += 4;
        });
        humanReadableCodes["NOT"] = optCode++;

        //PUSH    REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.pushParameter(memory.getRegister(statementPtr[0]));
            nextStatementIndex += 2;  
        });
        humanReadableCodes["PUSH"] = optCode++;

        //POP     REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(statementPtr[0], memory.popParameter());
            nextStatementIndex += 2;
        });
        humanReadableCodes["POP"] = optCode++;

        //INCR     REG
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.setRegister(
                statementPtr[0], 
                memory.getRegister(statementPtr[0]) + 1
            );
            nextStatementIndex += 2;
        });
        humanReadableCodes["INCR"] = optCode++;

        //CALL L //Thats pushing frame with parameters, setting return address
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            memory.pushFrame(nextStatementIndex + 2);
            nextStatementIndex = statementPtr[0];
        });
        humanReadableCodes["CALL"] = optCode++;

        //CALLV REG //Thats pushing frame with parameters, setting return address
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            int retIndex = nextStatementIndex + 2;
            nextStatementIndex = memory.getRegister(statementPtr[0]);
            memory.pushFrame(retIndex);
        });
        humanReadableCodes["CALLV"] = optCode++;

        //RETURN //Thats setting the return values and poping frame
        operations.push_back([](int8* statementPtr, Memory& memory, int& nextStatementIndex, bool& end){
            nextStatementIndex = memory.popFrame();
        });
        humanReadableCodes["RETURN"] = optCode++;

        // Generate dict for disassembling
        for (auto& t : humanReadableCodes)
            optToReadable[t.second] = t.first;
    }

    int8 encodeOptString(std::string str){
        if(str == "LABEL" || str == ";")
            throw std::runtime_error("Labels should be removed by now" + BT); //Label and comments should be removed by now

        if(humanReadableCodes.count(str) != 1)
            throw std::runtime_error("Did not find command opt" + BT);

        return humanReadableCodes[str];
    }

    std::string disassambleOptCode(int8 code){
        if(optToReadable.find(code) == optToReadable.end())
            throw std::runtime_error("Code not defined for disassambleOptCode: " + std::to_string(code) + " " + BT);

        return optToReadable[code];
    }

    inline Operation getOperation(const int8& code){
        #ifndef OPTIMIZED
        if(code > operations.size() || code < 0)
            throw std::runtime_error("Code not defined for getOperation: " + std::to_string(code) + " " + BT);
        #endif

        return operations[code];
    }
};
