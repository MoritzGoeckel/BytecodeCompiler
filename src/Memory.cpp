#pragma once

#include <map>
#include <iostream>

#include "Types.cpp"

#include "ErrorHandling.cpp"

class Memory{

    private:
    std::map<int8, int> registers;
    // TODO: Should just use an array

    public:
    Memory(){
        registers = std::map<int8, int>();
    }

    int getRegister(int8 id){
        return registers[id];
    }

    void setRegister(int8 id, int value){
        registers[id] = value;
    }

    void print(){
        std::cout << "First\t\tSecond" << std::endl;
        for (auto& t : registers)
            std::cout << t.first << "\t\t" << t.second << std::endl;
    }

    int getSize(){
        return registers.size();
    }
};