#pragma once
#include <vector>
#include "object.h"


using namespace std;
enum GateType {AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF, DFF, UNKNOWN};

class Gate : public Object{
    //Variables:
    static int gateCount ;
public:
    //Variables:
    int gateNumber ;  
    GateType gateType ;

    
	//io
	//inputs, outputs contains the objects connected to this gate's inputs and outputs, The object type can be anything, but usually it's either a wire (from another gate) or an io (input or bidir), this containers will be populated at verilog-reader post processing function
	vector<pair<string, string> > pins ;
	vector<Object*> inputs ;	//inputs, outputs collection will be populated at [verilog-]reader post processing function.
	vector<Object*> outputs ;
	int getInputNumber();
	int getOutputNumber();
	
    //Collections:
	//forwardList and Backward list contains the gates connected to the inputs and outputs of this gate. This collections will be calculated at verilogpostprocessing method. They may contains lesser element that forwardNodes and backwardNodes because some gates may be connected to IOs instead of another gates.
    vector<Gate*> forwardList ;
    vector<Gate*> backwardList ;
    vector<Gate*> neighbors; //Physical neighbors extracted from layout. this collection is populated using DefReader class.
    //vector<FaultSite> physicalFaultSite ;


	//Algorithmic Dependent Variables
	// distance from Primary Input and Primary Output. This values will be calculated at CircuitDB::levelizeFromPI() and CircuitDB::levelizeFromPO() methods using BFS Algorithm.
	int piDistance ;
	int poDistance ;

    //Methods:
    Gate(string,string, vector<pair<string, string> >);
    static int getGateCount();
    static int generateGateNumber();
};


/*
#include <vector>
#include <deque>

struct Inout{
	short num;
	struct Inout *next;
};


struct Event{
	int time;
	float p0,p1,pd,pdn;
	float vomin, vomax;
	int pwi, pwo;
};

// Data structure used for storing gate information
class Gate{
public:
    std::deque<struct event*> event_list;
	short input_count;
	short* input_list;
	short* input_values;
	short out;
	short type; // indicates gate's type including NAND(0),AND(1),NOR(2),OR(3),XOR(4),XNOR(5),NOT(6),DFF(7),LUT(8)
	short value;
	long sum_values;
	short is_dff; // if gate is flip-flop, is_dff=1, otherwise is_dff=0
	float signal_probability, sample_signal_probability;
	float epp,epp_var;
	float p0,p1,pd,pdn;
	float q0,q1,qd,qdn;
	bool is_onpath;
    
	//These variables are used for slack comp., actualy delays
	float gd, gd_min, delay, slack; //gd: gate delay,  delay: delay from input to the gate, slack: gate slack
	float load; //fanout load
	float inputc; //input capacitance; we assume that the input capacitances are the same
	float tplh_fix, tphl_fix; //Intrinsic delay for
	float tplh_load, tphl_load; //Load dependent delay
	float tplh, tphl; //TPLH, TPHL delay
	float rsize; //relative size. Initially rsize=1
	bool is_resized; //Initially is_resized=false
	float area; //W, L
    
    
	float derating; // Systematic Derating
	float sderating; // Simulation Derating
	float sample_variance,total_variance;
	long total_iteration;
    
	float p0_var,p1_var,pd_var,pdn_var; //variances of p0,p1,pd,pdn
	short site_out_level; //number of levels between error site and current gate
	int gate_no, dff_no, po_no;  //po_no is a PO number is po_list
	float vomin, vomax, vimin, vimax;
	//deque <int> event_time;
	//int *event_list
	//short* event_type;
    
	// look-up table struct, can be defined as a gate when we're dealing with fpga.
	bool is_lut ;
	int lut_size; 
	short* lut ;
	short* fault ;
    
    std::vector<Gate*> forward_list ;
    std::vector<Gate*> backward_list ;
    
};

// Data structure used for DFFs
class Dff {
public:
	short d;
	short q;
	struct Gate* dgateptr; // pointer to the its equivalent gate
};

*/
