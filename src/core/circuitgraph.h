#pragma once
#include "graph.h"
#include "gate.h"
#include "io.h"
#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;
using namespace std;

typedef adjacency_list<vecS, vecS, bidirectionalS> BoostGraph;

class CircuitGraph{
public:
    Graph* graph ; // standard undirected graph, contains only gates
    Graph* graph_directed ;  // directed graph of gates
    Graph* graph_gates_pi ;  // undirected graph of gates and primary inputs/outputs
    Graph* graph_gates_pi_directed ; //directed graph of gates and primary inputs/outputs
    //Collection:
    BoostGraph boostGraph;
    
    int distanceXtoY(Gate* x, Gate* y, bool direction);
    int flowXtoY(Gate* x, Gate* y, bool direction);
    int distanceXtoP(Gate* x, IO* y, bool direction);

};