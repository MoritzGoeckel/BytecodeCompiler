#pragma once

#include <iostream>

#include "ErrorHandling.cpp"

#include "ByteCode.cpp"
#include "Memory.cpp"
#include "OptCodeEngine.cpp"

namespace VirtualMachine{

    void execute(ByteCode code){
        OptCodeEngine e;
        Memory m;
        
        unsigned char* byteArray = code.getRaw();
        bool end = false;
        int nextStatementIndex = 0;

        while(!end){
            unsigned char opt = byteArray[nextStatementIndex];

            std::cout << "Before " << std::to_string(opt) << " " << e.disassambleOptCode(opt) << " " << std::to_string(nextStatementIndex) << " " << std::to_string(end) << std::endl;
            e.getOperation(opt)(byteArray + nextStatementIndex + 1, m, nextStatementIndex, end);
            std::cout << "After  " << std::to_string(opt) << " " << e.disassambleOptCode(opt) << " " << std::to_string(nextStatementIndex) << " " << std::to_string(end) << std::endl;
        }
    }

}
