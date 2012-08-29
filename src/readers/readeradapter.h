#pragma once
#include "configuration.h"
#include "core.h"
#include "loggingservices.h"
#include <vector>
#include "reader.h"

class ReaderAdapter{
    Configuration* config ;
    Core* core; 
    LoggingServices* log ;
    std::vector<Reader*> readers ;
public:
    ReaderAdapter(Configuration*, Core*, LoggingServices*);
    void read();
    void registerReaders();
};