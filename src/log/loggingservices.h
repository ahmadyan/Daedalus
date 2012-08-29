#pragma once
#include "configuration.h"

class LoggingServices{
    Configuration* config;
public:
    LoggingServices(Configuration*);
    //friend ostream &operator <<(ostream &, const string &);
};