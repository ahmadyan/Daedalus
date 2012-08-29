#include "circuitgraph.h"


/* Distance of gate x from gate y in uni/bi directional graph
 */
int CircuitGraph::distanceXtoY(Gate* x, Gate* y, bool direction){
    if(direction){
        return graph_directed->distance(x->gateNumber,y->gateNumber);
    }else{
        return graph->distance(x->gateNumber,y->gateNumber);
    }
}


/* flow from gate x to gate y in uni/bi directional graph
 */
int CircuitGraph::flowXtoY(Gate* x, Gate* y, bool direction){
    if(direction){
        return graph_directed->networkFlow(x->gateNumber,y->gateNumber);
    }else{
        return graph->networkFlow(x->gateNumber,y->gateNumber);
    }    
}

/* Distance of gate x from pi/po y
 */
int CircuitGraph::distanceXtoP(Gate* x, IO* y, bool direction){
    if(direction){
        return graph_gates_pi_directed->distance(x->gateNumber, x->getGateCount() + y->ioNumber);
    }else{
        return graph_gates_pi->distance(x->gateNumber, x->getGateCount() + y->ioNumber);        
    }
}

