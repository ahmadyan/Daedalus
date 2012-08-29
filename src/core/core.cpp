#include "core.h"
#include "graph.h"
#include "io.h"

Core::Core(Configuration* _config){
    config = _config ;
    db = new CircuitDB();
    lib = new StandardCellLibrary();
    graph = new CircuitGraph();
    sp = new SignalProbabilityDB();
}

void Core::init(){
}

/* generateCircuitGraphs will generate four type of adjacency graph datastructure we need
 * for our analysis later. they include directed and undirected gate adjancency lists and directed and undirected gates and PI/PO adjacency lists.
 */
void Core::generateCircuitGraphs(){
    cout << "[Core] start generating circuit graphs..." << endl ;
    int ngates=db->getGateNumber(), npio=db->getPIONumber();
    
    graph->graph = new Graph(ngates, false); // standard undirected graph, contains only gates
    graph->graph_directed = new Graph(ngates, false);  // directed graph of gates
    graph->graph_gates_pi = new Graph(ngates+npio, false);  // undirected graph of gates and primary inputs/outputs
    graph->graph_gates_pi_directed = new Graph(ngates+npio, false); // directed graph of gates and primary inputs/outputs
    for(int i=0;i<db->gates.size();i++){
        //cout << i << " " << db->gates[i]->gateNumber << " " << db->gates[i]->forwardList.size() << " " << db->gates[i]->name << endl ;
        for(int j=0;j<db->gates[i]->forwardList.size();j++){
            graph->graph->addEdge(db->gates[i]->gateNumber, db->gates[i]->forwardList[j]->gateNumber);
            graph->graph_directed->addEdge(db->gates[i]->gateNumber, db->gates[i]->forwardList[j]->gateNumber);
            graph->graph_gates_pi->addEdge(db->gates[i]->gateNumber, db->gates[i]->forwardList[j]->gateNumber);
            graph->graph_gates_pi_directed->addEdge(db->gates[i]->gateNumber, db->gates[i]->forwardList[j]->gateNumber);
        }
    }
    
    for(int i=0;i<npio;i++){
        //determine which gate this pio is connected to
        //cout << i << " " << db->ios[i]->name << " " << db->ios[i]->gateList.size() ;
        for(int j=0;j<db->ios[i]->gateList.size();j++){
            graph->graph_gates_pi->addEdge(ngates+i, db->ios[i]->gateList[j]->gateNumber);
            if(db->ios[i]->ioType==input){
                graph->graph_gates_pi_directed->addEdge(ngates+i, db->ios[i]->gateList[j]->gateNumber);
            }else{
                graph->graph_gates_pi_directed->addEdge(npio+db->ios[i]->gateList[j]->gateNumber, ngates+i);
            }
        }
    }
    
    //graph->graph->bfs(32);
    //graph->graph->findPath(8,120);
    //cout << "flow=" << graph->graph->networkFlow(8,120);
    //graph->graph_gates_pi->printGraph();
    cout << "[Core] finished generating circuit graphs..." << endl ;
}


/* Checking wether gate x & y are neighbors at layout
  * the actual method is implemented in circuit db class, however the DEFReader should populate the neighbor collection first.
 */
bool Core::isNeighborXY(Gate* x, Gate* y){
    return db->isNeighborXY(x,y); 
}

/* Distance of gate x from gate y in uni/bi directional graph
  * the actual method is implemented in circuit graph class
 */
int Core::distanceXtoY(Gate* x, Gate* y, bool direction){
    return graph->distanceXtoY(x,y,direction);
}

/* flow from gate x to gate y in uni/bi directional graph
  * the actual method is implemented in circuit graph class
 */
int Core::flowXtoY(Gate* x, Gate* y, bool direction){
    return graph->flowXtoY(x,y,direction); 
}

/* Distance of gate x from pi/po y
 * the actual method is implemented in circuit graph class
 */
int Core::distanceXtoP(Gate* x, IO* y, bool direction){
    return graph->distanceXtoP(x, y, direction);   
}


