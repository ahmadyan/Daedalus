#pragma once
#include "configuration.h"
#include "core.h"
#include "loggingservices.h"

class Kernel {
    Configuration* config;
    Core* core;
    LoggingServices* log;
    
public:
    Kernel(int, char**);
    void main();
    void read();
    void execute();
    void shutdown();
};