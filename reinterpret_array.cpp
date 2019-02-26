#include <vector>
#include <iostream>
#include <bitset>

typedef unsigned char int8;
typedef unsigned short int16;

template<class t>
void print(t n){
    std::bitset<sizeof(t) * 8> x(n);
    std::cout << x << std::endl;
}

int main() {
    std::cout << sizeof(int8) * 8 << " " << sizeof(int16) * 8 << std::endl;
    
    //int16 num = 255;
    int16 num = 9001;
    std::vector<int8> v;
    
    std::cout << "-> " << num << std::endl;

    print<int16>(num);
    std::cout << "first " << std::endl;
    print<int8>(static_cast<int8>(num));
    v.push_back(static_cast<int8>(num)); 
    
    std::cout << "second " << std::endl;
    print<int8>(static_cast<int8>(num >> 8));
    v.push_back(static_cast<int8>(num >> 8));

    int16 numnum = *((int16*) &*(v.begin() + 0));
    std::cout << "<- " << numnum << std::endl;
    print<int16>(numnum);
}
