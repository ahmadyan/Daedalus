#pragma once
#include "object.h"
#include <vector>
#include "gate.h"
using namespace std;

enum IOType {input, output, bidir};

class IO : public Object{
    public:
    //Variables:
    IOType ioType ;
    //ioNumber is a unique ordered ID assigned to this IO.
    int ioNumber ;
    //pioNumber is a unique ordered ID assigned to this IO in it's own class. like input, output, etc. Two different IO class can has the same pioNumber.
    int pioNumber ;
    
    static int inputCount;
    static int outputCount;
    static int bidirCount;
    static int ioCount;

    //Collections:
    vector<Gate*> gateList ;
    
    //Methods:
    IO(string, IOType);
    void addGate(Gate*);
    static int getInputCount();
    static int getOutputCount();
    static int getBidirCount();
    static int getIOCount();
    static int generateInputID();
    static int generateOutputID();
    static int generateBidirID();
    static int generateIOID();
};