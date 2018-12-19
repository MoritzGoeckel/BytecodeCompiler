#pragma once

#include <map>
#include <iostream>

#include "ErrorHandling.cpp"

class Memory{

    private:
    std::map<unsigned int, int> registers;
    // TODO: Should just use an array

    public:
    Memory(){
        registers = std::map<unsigned int, int>();
    }

    int getRegister(unsigned int id){
        return registers[id];
    }

    void setRegister(unsigned int id, int value){
        registers[id] = value;
    }

    void print(){
        std::cout << "First\t\tSecond" << std::endl;
        for (auto& t : registers)
            std::cout << t.first << "\t\t" << t.second << std::endl;
    }
};