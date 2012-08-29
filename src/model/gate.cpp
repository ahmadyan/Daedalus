#include "gate.h"
#include <iostream>
int Gate::gateCount = 0;
using namespace std;
Gate::Gate(string _name, string _type, vector<pair<string, string> > _pins){
    gateNumber = generateGateNumber();
    name = _name ;              
	pins=_pins;
    //This is buggish! We should read the input count from library not from netlist. maybe synthesizer did not connect and wrote some pins on the netlist.
    //inputNumber = (int)(pins.size())-1;
    type = gate; //Object type (gate, wire, etc)
    //TODO: extract gate type from it's type string
	int x = _type.compare("dff") ;
	cout << _type << " " << x <<endl ;
	if(_type.compare( "nand" ) == 0)
		gateType = NAND; 
	else if(_type.compare( "and") == 0)
		gateType = AND; 
	else if(_type.compare( "nor") == 0)
		gateType = NOR; 
	else if(_type.compare( "or") == 0)
		gateType = OR; 
	else if(_type.compare( "xor") == 0)
		gateType = XOR; 
	else if(_type.compare( "xnor") == 0)
		gateType = XNOR; 
	else if(_type.compare( "not") == 0)
		gateType = NOT; 
	else if(_type.compare( "dff") == 0)
		gateType = DFF; 
	else if(_type.compare(  "buf") == 0)
		gateType = BUF; 
	else{
		//cout << endl << "Unrecognized gate ==> Exit" << endl;
		//cout << "gate number: " << gate_numbers << endl;
		//cout << "gate name: " << field << endl;
		gateType = UNKNOWN ; //Gate type, (AND, OR, NOR, etc)
	}
	
	//initial value for PI and PO distance, This values will be calculated at CircuitDB::levelizeFromPI() and CircuitDB::levelizeFromPO() methods using BFS Algorithm.
	piDistance = poDistance = -1 ;
	
    
}

int Gate::generateGateNumber(){
    return gateCount++;
}

int Gate::getGateCount(){
    return gateCount;
}

int Gate::getInputNumber(){
	return inputs.size();
}

int Gate::getOutputNumber(){
	return outputs.size();	
}

/*
vector<FaultSite> Gate::GetPhysicalNeighbor(){
	if ( neighbors.size() == physicalFaultSite.size() ) return physicalFaultSite ;
	else{
		for(int i=0;i<neighbors.size(); i++){
			physicalFaultSite.push_back( FaultSite( this, neighbors[i]) ) ;
		}
		return physicalFaultSite ;
	}
}
*/
