#pragma once

#include <iostream>

#include "Types.cpp"

#include "ErrorHandling.cpp"

#include "ByteCode.cpp"
#include "Memory.cpp"
#include "OptCodeEngine.cpp"

namespace VirtualMachine{

    void execute(ByteCode code){
        OptCodeEngine e;
        Memory m;
        
        int8* byteArray = code.getRaw();
        bool end = false;
        int nextStatementIndex = 0;

        while(!end){
            int8 opt = byteArray[nextStatementIndex];
            //std::cout << "Before " << std::to_string(opt) << " " << e.disassambleOptCode(opt) << " " << std::to_string(nextStatementIndex) << " " << std::to_string(end) << std::endl;
            e.getOperation(opt)(byteArray + nextStatementIndex + 1, m, nextStatementIndex, end);
            //std::cout << "-> " << std::to_string(opt) << " " << e.disassambleOptCode(opt) << " " << std::to_string(nextStatementIndex) << " " << std::to_string(end) << std::endl;
        }

        std::cout << "Used " << std::to_string(m.getSize()) << " registers" << std::endl;
    }

}
