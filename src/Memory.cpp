#pragma once

#include <map>
#include <iostream>

#include "ErrorHandling.cpp"

class Memory{

    private:
    std::map<unsigned int, int> registers;
    // TODO: Should just use an array

    bool equal, greater, less; 

    public:
    Memory(){
        equal = false;
        greater = false;
        less = false;
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

    void setEqual(){
        this->equal = true;
    }

    void setGreater(){
        this->greater = true;
    }

    void setLess(){
        this->less = true;
    }

    bool getEqual(){
        bool tmp = this->equal;
        this->equal = false;
        return tmp;
    }

    bool getGreater(){
        bool tmp = this->greater;
        this->greater = false;
        return tmp;
    }

    bool getLess(){
        bool tmp = this->less;
        this->less = false;
        return tmp;
    }

    void resetCompare(){
        this->less = false;
        this->greater = false;
        this->equal = false;
    }

};