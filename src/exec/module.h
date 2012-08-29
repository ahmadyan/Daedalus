#pragma once

#include "configuration.h"
#include "loggingservices.h"
#include "core.h"

class Module{
protected:
    Configuration* config ;
    Core* core ;
    LoggingServices* log ;
public:
    Module();
    ~Module();
    virtual void run();
    virtual void init();
    virtual void execute();
    virtual void shutdown();
};