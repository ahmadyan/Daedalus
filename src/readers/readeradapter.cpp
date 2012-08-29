#include "readeradapter.h"
#include <iostream>
#include "verilogreader.h"
#include "defreader.h"

ReaderAdapter::ReaderAdapter(Configuration* _config, Core* _core, LoggingServices* _log):config(_config), core(_core), log(_log){
    registerReaders();
}

void ReaderAdapter::read(){
    for(int i=0;i<readers.size();i++){
        readers[i]->read();
    }
}

void ReaderAdapter::registerReaders(){
    //First, check the 
    if ( config->getParameter("daedalus.input.verilog.enable").compare("true")==0 ){
        //reading verilog input
        std::cout << "[Reader] reading verilog" << std::endl ;
        readers.push_back(new VerilogReader(config, core, log));
    }
    if ( config->getParameter("daedalus.input.def.enable").compare("true")==0 ){
        //reading def input
        std::cout << "[Reader] reading Def" << std::endl  ;
        readers.push_back(new DefReader(config, core, log));
    }
}