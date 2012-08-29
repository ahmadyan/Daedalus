#include "reader.h"
#include <iostream>

Reader::Reader(){}

Reader::Reader(Configuration* _config, Core* _core, LoggingServices* _log): config(_config), core(_core), log(_log){
}

void Reader::getInfo(){
    std::cout << "test1" << std::endl ;
}

void Reader::read(){
    std::cout << "This function has not been implemented yet!" << std::endl ;
}