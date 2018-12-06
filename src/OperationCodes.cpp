#pragma once

#include "Memory.cpp"
#include <iostream>

enum OptCode {LOAD, INCR, OUT, END, ADD};

class OptCodeEngine{

    public:

    OptCode decodeOptCode(unsigned char c){
        switch(c) {
            case 0   : return END;
            case 1   : return LOAD;
            case 2   : return INCR;
            case 3   : return OUT;
            case 4   : return ADD;
        }
    }

    //General interface
    typedef void (*Operation)(unsigned char* args, Memory& memory, int& nextLine, bool& end);

    Operation getOperation(OptCode code){
        switch(code) {
            case END    : return this->endOp;
            case LOAD   : return this->loadOp;
            case INCR   : return this->incrOp;
            case OUT    : return this->outOp;
            case ADD    : return this->addOp;
        }
    }

    private:

    static void endOp(unsigned char* args, Memory& memory, int& nextLine, bool& end){
        end = true;
    }

    static void loadOp(unsigned char* args, Memory& memory, int& nextLine, bool& end){
        memory.setRegister(args[0], args[1]);
        nextLine++;
    }

    static void incrOp(unsigned char* args, Memory& memory, int& nextLine, bool& end){
        memory.setRegister(args[0], memory.getRegister(args[0]) + 1);
        nextLine++;
    }

    static void outOp(unsigned char* args, Memory& memory, int& nextLine, bool& end){
        std::cout << memory.getRegister(args[0]) << std::endl;
        nextLine++;
    }

    static void addOp(unsigned char* args, Memory& memory, int& nextLine, bool& end){
        memory.setRegister(
            args[0], 
            memory.getRegister(args[0]) + memory.getRegister(args[1])
        );
        nextLine++;
    }

};