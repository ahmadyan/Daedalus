#pragma once
#include "gate.h"
#include "io.h"
#include "wire.h"
#include "parasitic.h"
#include "faultsite.h"
#include <map>
using namespace std;


class CircuitDB{
public:
    //Variables:
    int numPI, numPO;
    int nodes;
    
    //Collections:
    vector<Object*> objects ;
    vector<Gate*> gates ;
    vector<Gate*> dffs ;
    vector<IO*> ios;
    vector<Wire*> wires ;
    vector<Parasitic*> parasitics ;
    map<string, pair<int,int> > placement;
    map<string, int> netlist ;
    
    //Methods:
    CircuitDB();
    void generatePlacementReport();
	void link();
    void insert(Object*);
    int getGateNumber();
	int getDFFNumber();
    int getPIONumber();
    
    bool isNeighborXY(Gate* x, Gate* y);
    void levelizeFromPI();
    void levelizeFromPO();
    vector<FaultSite>	gateLevelPILevelDistanceBasedNeighborIdentification(Gate* g, int);
    vector<FaultSite>	gateLevelPOLevelDistanceBasedNeighborIdentification(Gate* g, int);
    vector<FaultSite>	gateLevelDistanceBasedNeighborIdentification(Gate* g, int distance, bool direction);
    vector<FaultSite>	randomFaultSiteSelection(Gate*, int);
    vector<FaultSite> 	layoutFaultSiteExtraction(Gate* g);

	Object* getObjectByID(int objectID);
	Object* getObjectByName(string objectName);
	
	void BFS(void (*f)(Gate*), int ioNumber, bool direction );
};


/*
 short numPi, numPo;
 // fadjlist[i] contains  adjacency list of node i (i ranges 1...(nodes-1))
 // fadjlist[i][j] is a pointer to one of adjacent nodes of node i (j ranges 0 .. # of adjlist[i]->size()-1)
 vector < deque<Gate*> > fadjlist;  // forward graph
 
 // fadjlist_ptr[i] points to the gate whose output adjacency list is fadjlist[i]
 // fadjlist_ptr[i] is not valid for all i's because every node isn't output of a gate
 deque <Gate*> fadjlist_ptr;       // a graph to point to the gate in fadjlist
 
 // radjlist[i] contains reverse adjacency list of node i (i ranges 1...(nodes-1))
 deque <Event*> event_list_ptr;
 
 // radjlist[i][j] is node number of one of reverse adjacent nodes of node i (j ranges 0 .. # of rdjlist[i]->size()-1)
 vector < deque<short> > radjlist;           // reverse graph
 
 // color[i] is color of node i
 vector <char> color;                      // Used for DFS search
 
 // reachOp[i] is list of all reachable outputs from node i
 // Note if ReachNodesFromPOi is called for POj, reachOp is list of all reachable nodes from POj
 vector < deque<short> > reachOp;            // reachable nodes from outputs
 
 // netlist is a one-to-one mapping between <wires,inputs,outputs> and <node numbers>
 // map's items ranges from 1 to nodes-1
 std::map <string, short> netlist;                // Used for mapping net names to short
 
 // PI2POlist is list of all nodes from specific nodes to all outputs or specific output
 // it ranges from 0 to PI2POlist.size()-1
 // In case of primary input PIi, PIi is also added to the list but
 // in case of other inputs (outputs of flip-flops), it is not in the list
 vector < deque<short> > PI2POlist;          // Include all nodes on the paths from PIi to POj
 
 // gate_list is list of all circuit gates including flip-flops. it ranges from 0 to gate_numbers-1
 // Note that this list is not initially levelized but using arange_gates we arrange all gates (not DFFs)
 deque <Gate* > gate_list;  // Gate list
 
 deque <Gate* > po_list;		// Primary output list
 // dfflist_ptr is list of all flip-flops of the circuit. it ranges from 0 to dff_num-1
 // Note: each gate has a pointer to its equivalent gate
 deque <Dff*>  dfflist_ptr;  // DFFs list
 
 struct Inout *headPi, *headPo;
 short* PI_list;				// used for assigning random values to the inputs during logic simulation
 float* PI_list_sp;				// used for input values signal probabilities !
 short nodes; // Number of nodes in the circuit ** Number of nodes of the circuit is nodes-1
 short gate_numbers; // number of gates in the circuit including flop-flops
 short gate_numbers_noff; //number of gates excluding flip-flops
 short sim_gate_numbers; // the number of gates being simulated
 short dff_num;	  // number of flop-flops in the circuit
 float max_delay;
 
 
 //TODO: impelemnt these methods later!
 
 void appendPo(struct inout *headPo, short net);
 void appendPi(struct inout *headPi, short net);
 void report_netlist();
 short readNetlist_verilog(FILE *fp, vcd* VCD,Configuration* config, FILE* resultfp);
 void arange_gates();
 void compute_signal_probability(Configuration* config, vcd* VCD,  FILE*);
 void signal_probability_report(FILE* resultfp);
 short extractField(FILE *fp, char *field);
 void logic_simulation(short run_for_sp, vcd* signals,  configuration*);
 void compute_gate_value(struct gate* gateptr);
 void compute_circuit_delay_from_library(library* lib);
 void compute_circuit_delay(bool slack_resizing);
 void initialize_gate_delay(struct gate *gptr1);
 string gate_name(struct gate* gateptr);
 */
