#pragma once

#include <iostream>
#include "object.h"


class Wire : public Object{
public:
    //Variables:
    int wireID;
    static int wireCount ;
    //Methods:
    Wire(std::string);
    static int generateWireID();
};