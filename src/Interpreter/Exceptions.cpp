#include <exception>
#include <string>
#include <iostream>

class ParsingException : public std::exception{
    public:
    ParsingException(std::string expected, std::string found, std::string where){
        this->expected = expected;
        this->found = found;
        this->where = where;
    }

    void print(){
        std::cout << "ParsingException: " << expected << " expected, " << found << " found in " << where << std::endl;
    }

    void printShort(){
        std::cout << "ParsingException: " << expected << " expected, " << found << " found " << std::endl;
    }

    private:
    std::string expected;
    std::string found;
    std::string where;
};