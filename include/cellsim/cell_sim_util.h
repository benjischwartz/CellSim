#pragma once

#include <iostream>
#include <libcellml>

class dummyClass {
    int x;

public:
        void set(int x) {
            this->x = x;
        }
        void printValue() {
            std::cout<<"Value: "<<x<<std::endl;
        }
};
