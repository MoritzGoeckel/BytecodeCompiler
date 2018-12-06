#include <iostream>

#include <fstream>
#include <assert.h>

#include <vector>

void write(std::vector<unsigned char>& data){
    std::ofstream file("testfile.bytes", std::ios_base::binary);
    assert(file.is_open());
    
    file.write((char*)&data[0], data.size());
    file.close();
}

void read(std::vector<unsigned char>& data){
    std::ifstream is ("testfile.bytes", std::ifstream::binary);
    assert(is);

    // Get length of file
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    char* buffer = new char [length];

    // Actual read
    is.read (buffer,length);

    data.insert(data.end(), buffer, buffer + length);

    assert(is);
    is.close();

    delete[] buffer;
}