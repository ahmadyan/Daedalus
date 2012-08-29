#pragma once
#include "reader.h"
#include "io.h"

class VerilogReader: public Reader{
    public:
    VerilogReader(Configuration*, Core*, LoggingServices*);
    virtual void read();
    void readVerilogNetlist();
    void verilogPostProcessing();
    IOType determinePinType(string);
    void readVerilogNetlistIscasFormatted();
};
