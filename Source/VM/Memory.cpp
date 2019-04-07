#pragma once

#include <map>
#include <vector>
#include <iostream>

#include "Types.cpp"
#include "../ErrorHandling.cpp"

//#define SAFE_MODE

class Frame{
    private:
    std::map<int8, int> registers;
    // TODO: Should just use an array
    int returnAddress;

    public:
    Frame(int returnAddress){
        this->returnAddress = returnAddress;
    }

    int getRegister(int8 id){
        #ifdef SAFE_MODE
        if(!containsRegister(id))
            throw std::runtime_error("Register does not exist" + BT);
        #endif

        return registers[id];
    }

    bool containsRegister(int8 id){
        return registers.find(id) != registers.end();
    }

    void createRegister(int8 id, int value){
        #ifdef SAFE_MODE
        if(containsRegister(id))
            throw std::runtime_error("Register already exists" + BT);  
        #endif

        registers[id] = value;
    }

    void freeRegister(int8 id){
        #ifdef SAFE_MODE
        if(!containsRegister(id))
            throw std::runtime_error("Register does not exist" + BT);
        #endif

        registers.erase(id);
    }

    void modifyRegister(int8 id, int value){
        #ifdef SAFE_MODE
        if(!containsRegister(id))
            throw std::runtime_error("Register does not exist" + BT);  
        #endif

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
        //std::cout << "Pushing frame to " << std::to_string(frames.size() - 1) << std::endl;
    }

    int popFrame(){
        int returnAddress = frames.back().getReturnAddress();
        frames.pop_back();
        //std::cout << "Popping frame to " << std::to_string(frames.size() - 1) << std::endl;
        return returnAddress;
    }

    int getRegister(int8 id){
        for(size_t i = frames.size() - 1; i >= 0; i--){
            if(frames[i].containsRegister(id)){
                return frames[i].getRegister(id);
            }
        }
        throw std::runtime_error("Non existing register requested " + BT);        
    }

    void freeRegister(int8 id){
        for(size_t i = frames.size() - 1; i >= 0; i--){
            if(frames[i].containsRegister(id)){
                frames[i].freeRegister(id);
                return;
            }
        }
        throw std::runtime_error("Non existing register requested " + BT);
    }

    void createRegister(int8 id){ // That is an alloc
        frames.back().createRegister(id, 0); // This should happen for a let
        // When no let is present, then the variable deepest in the stack
        //      should be set
    }

    void modifyRegister(int8 id, int value){ // Without let
        for(size_t i = frames.size() - 1; i >= 0; i--){
            //std::cout << "Mod " << std::to_string(id) << " Frame " << i << std::endl;
            if(frames[i].containsRegister(id)){
                //std::cout << ">> Found " << std::to_string(id) << " Frame " << i << std::endl;
                frames[i].modifyRegister(id, value);
                return;
            }
        }
        throw std::runtime_error("Non existing register setting requested " + BT);  
    }

    void print(){
        std::cout << "Frames" << std::endl;
        for (auto& f : frames){
            std::cout << "Next frame:" << std::endl;
            f.print();
        }
    }
};
