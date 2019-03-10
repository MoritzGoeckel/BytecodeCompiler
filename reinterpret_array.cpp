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
    int16 input = 9001;
    std::vector<int8> v;
    
    int index = 0;

    //Writing
    v.reserve(2);
    *((int16*)(v.data() + index)) = input;

    //Reading
    int16 output = *((int16*)(v.data() + index));

    std::cout << output << std::endl;
}
