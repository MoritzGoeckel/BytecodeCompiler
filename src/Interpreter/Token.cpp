#include <string>
#include <iostream>
#include <vector>


int OCBR = 0;
int CCBR = 1;
int OBR = 2;
int CBR = 3;
int IOP = 4;
int RET = 5;
int COM = 6;
int SEMIC = 7;
int COMP = 8;
int ASSIGN = 9;
int IDENT = 10;
int BRANCH = 10;

class Token{
    
    public:
    
    Token(int type, std::string text){
        this->type = type;
        this->text = text;
    }

    int getType(){
        return this->type;
    }

    std::string getText(){
        return this->text;
    }

    void print(){
        std::cout << "TOKEN('" << typeToString(type) << "', '" << text << "') "; 
    }

    private: 
    
    static std::string typeToString(int type){
        std::vector<std::string> types;
        types.push_back("OCBR");
        types.push_back("CCBR");
        types.push_back("OBR");
        types.push_back("CBR");
        types.push_back("IOP");
        types.push_back("RET");
        types.push_back("COM");
        types.push_back("SEMIC");
        types.push_back("COMP");
        types.push_back("ASSIGN");
        types.push_back("IDENT");
        types.push_back("BRANCH");
        
        return types[type];
    }

    int type;
    std::string text;
};
