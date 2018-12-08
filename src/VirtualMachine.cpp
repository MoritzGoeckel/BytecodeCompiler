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
        
        unsigned char opt, param_one, param_two;
        bool end = false;
        int lineIndex = 0;

        while(!end && lineIndex < code.size() / 3){
            //std::cout << "Line " << lineIndex << " " << code.size() / 3 << std::endl;
            code.getStatement(lineIndex, opt, param_one, param_two);
            unsigned char params[] = {param_one, param_two};
            e.getOperation(opt)(params, m, lineIndex, end);
        }
    }

}
