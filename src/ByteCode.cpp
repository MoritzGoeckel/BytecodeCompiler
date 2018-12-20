#pragma once

#include <iostream>

#include <fstream>
#include <assert.h>

#include <vector>
#include <string>

#include "Types.cpp"
#include "ErrorHandling.cpp"

class ByteCode{

    private:

    std::vector<int8> data;

    public:

    ByteCode(){
        this->data = std::vector<int8>();
    }

    ByteCode(std::vector<int8>& data){
        this->data = data;
    }

    void write(std::string path){
        std::ofstream file(path, std::ios_base::binary);
        assert(file.is_open());
        
        file.write((char*)&data[0], data.size());
        file.close();
    }

    void read(std::string path){
        std::ifstream is (path, std::ifstream::binary);
        assert(is);

        // Get length of file
        is.seekg (0, is.end);
        int length = is.tellg();

        // Go back to the beginning
        is.seekg (0, is.beg);

        char* buffer = new char [length];

        is.read (buffer, length);
        data.insert(data.end(), buffer, buffer + length);

        assert(is);
        is.close();

        delete[] buffer;
    }

    void add(int8 code){
        //std::cout << "Got code: " << std::to_string(code) << std::endl;
        data.push_back(code);
    }

    int8 getAt(int index){
        return data.at(index);
    }

    int8* getRaw(){
        return &data[0];
    }

    int size(){
        return data.size();
    }
};

