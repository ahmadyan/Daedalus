#include "wire.h"

int Wire::wireCount = 0 ;

Wire::Wire(std::string _name){
    //Dealing with object stuff.
    name = _name ;
    wireID = generateWireID();
}

int Wire::generateWireID(){
    return wireCount++;
}