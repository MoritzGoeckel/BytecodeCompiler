#pragma once

#include "../ErrorHandling.cpp"

#include <string>
#include <vector>

class RegisterManager{
    private:
    std::vector<std::string> unusedVars;

    int varIndex;

    void addNewVars(){
        for(int i = 0; i < 5; i++){
            unusedVars.push_back("var"+std::to_string(varIndex));
            varIndex++;
        }
        //Todo: generate them without numbers
    }

    public:
    RegisterManager(){
        varIndex = 0;
    }

    std::string borrow(){
        if(unusedVars.size() == 0)
            addNewVars();
        
        std::string var = unusedVars.back();
        unusedVars.pop_back();

        return var;
    }

    void giveBack(std::string var){
        unusedVars.push_back(var);
    }
};