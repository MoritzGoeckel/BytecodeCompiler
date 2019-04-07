#pragma once

#include "../ErrorHandling.cpp"

#include <string>

class LabelManager{
    int labelIndex;

    public:
    LabelManager(){
        labelIndex = 0;
    }

    std::string getNext(){
        return "label" + std::to_string(labelIndex++);
    }
};