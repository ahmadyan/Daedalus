#pragma once
#include "configuration.h"
#include "circuitdb.h"
#include "standardcelllibrary.h"
#include "circuitgraph.h"
#include "signalprobabilitydb.h"


class Core{
public:
    
    Configuration* config ;
    CircuitDB* db ;
    StandardCellLibrary* lib;
    CircuitGraph* graph ;
    SignalProbabilityDB* sp;
  
    
    Core(Configuration*);
    void init();
    void generateCircuitGraphs();
    
    bool isNeighborXY(Gate* x, Gate* y);
    int distanceXtoY(Gate* x, Gate* y, bool direction);
    int flowXtoY(Gate* x, Gate* y, bool direction);
    int distanceXtoP(Gate* x, IO* y, bool direction);
};