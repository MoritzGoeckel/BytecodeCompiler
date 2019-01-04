#pragma once

#include <map>
#include <iostream>

#include "Types.cpp"
#include "../ErrorHandling.cpp"

class Frame{
    private:
    std::map<int8, int> registers;
    // TODO: Should just use an array

    int returnAddress;

    public:
    Frame(int returnAddress){
        this->returnAddress = returnAddress;
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

    int getReturnAddress(){
        return returnAddress;
    }
};

class Memory{

    private:
    std::vector<int8> parameters;
    std::vector<Frame> frames;

    public:
    Memory(){
        parameters = std::vector<int8>();
        frames = std::vector<Frame>();
        pushFrame(-1);
    }

    void pushParameter(int param){
        //std::cout << "Pushing " << std::to_string(param) << std::endl;
        parameters.push_back(param);
    }

    int popParameter(){
        int ret = parameters.back();
        //std::cout << "Poping " << std::to_string(ret) << std::endl;
        parameters.pop_back();
        return ret;
    }

    void pushFrame(int returnAddress){
        frames.push_back(Frame(returnAddress));
    }

    int popFrame(){
        int returnAddress = frames.back().getReturnAddress();
        frames.pop_back();
        return returnAddress;
    }

    int getRegister(int8 id){
        return frames.back().getRegister(id);
    }

    void setRegister(int8 id, int value){
        frames.back().setRegister(id, value);
    }

    void print(){
        std::cout << "Frames" << std::endl;
        for (auto& f : frames){
            std::cout << "Next frame:" << std::endl;
            f.print();
        }
    }
};
