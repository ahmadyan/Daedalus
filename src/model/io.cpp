#include "io.h"

int IO::inputCount = 0;
int IO::outputCount = 0 ;
int IO::bidirCount = 0 ;
int IO::ioCount = 0;

IO::IO(string _name, IOType _iotype){
    //Dealing with object stuff.
    name = _name ;
    
    //IO class:
    ioNumber = generateIOID();
    ioType = _iotype;
    switch (ioType) {
        case input:
            pioNumber=generateInputID();
            break;
        case output:
            pioNumber=generateOutputID();
            break;
        case bidir:
            pioNumber=generateBidirID();
            break;
        default:
            break;
    }
}

void IO::addGate(Gate* g){
    gateList.push_back(g);
}

int IO::getInputCount(){
    return inputCount; 
}

int IO::getOutputCount(){
    return outputCount;
}

int IO::getBidirCount(){
    return bidirCount ;
}

int IO::getIOCount(){
    return ioCount ;
}

int IO::generateInputID(){
    return inputCount++;
}

int IO::generateOutputID(){
    return outputCount++;
}

int IO::generateBidirID(){
    return bidirCount++;
}

int IO::generateIOID(){
    return ioCount++;
}