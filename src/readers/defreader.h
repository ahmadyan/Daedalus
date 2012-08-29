#pragma once
#include "reader.h"

class DefReader: public Reader{
public:
    DefReader(Configuration*, Core*, LoggingServices*);
    virtual void read();
    void extractNeighbors();
    void readDEF();
};
