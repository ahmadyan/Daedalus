#pragma once

#include "configuration.h"
#include "core.h"
#include "loggingservices.h"

class Reader{
protected:
    Configuration* config; 
    Core* core ;
    LoggingServices* log ;
    
public:    
    Reader();
    Reader(Configuration*, Core*, LoggingServices*);
    virtual void read();
    void getInfo();
};
