#pragma once

#include <iostream>

#include "Types.cpp"
#include "../ErrorHandling.cpp"

#include "ByteCode.cpp"
#include "Memory.cpp"
#include "OptCodeEngine.cpp"

#include <iostream>

//#define EXEC_INTERACTIVE

void execute(ByteCode code){
    OptCodeEngine e;
    Memory m;
    
    int8* byteArray = code.getRaw();
    bool end = false;
    int nextStatementIndex = 0;

    while(!end){

        #ifdef EXEC_INTERACTIVE
        int8 opt = byteArray[nextStatementIndex];
        std::cout << "Executing: " 
            << " In: " << std::to_string(nextStatementIndex)
            << " Op: " << std::to_string(opt) << "(" << e.disassambleOptCode(opt) << ")"
            << " P1: " << std::to_string(*(byteArray + nextStatementIndex + 1)) 
            << " P2: " << std::to_string(*(byteArray + nextStatementIndex + 2))
            << " P3: " << std::to_string(*(byteArray + nextStatementIndex + 3)) 
            << std::endl;

        e.getOperation(opt)(byteArray + nextStatementIndex + 1, m, nextStatementIndex, end);
        std::cin.get();
        #else
        e.getOperation(byteArray[nextStatementIndex])(byteArray + nextStatementIndex + 1, m, nextStatementIndex, end);
        #endif
    }
}
