#include <iostream>
#include <queue>
#include <cmath>
#include <cstdlib>
#include "circuitdb.h"

using namespace std;


#ifdef _WIN32
	void srandom(int seed) {srand(seed);} //comment this line for SUN systems
	int random() { return rand();}		//comment this line for SUN systems
#endif

CircuitDB::CircuitDB(){
    numPI=0;
    numPO=0;
    nodes=0;
}

/** generatePlacementReport will traverse the placment map of all gates in the design.
 *  this map has been generated from DEF file and contains the (x,y) pair of physical   
 *   coordinates of the gate on the layout.
 */
void CircuitDB::generatePlacementReport(){
    map<string, pair<int,int> > ::iterator it;
        for ( it=placement.begin() ; it != placement.end(); it++ )
            cout << (*it).first << " => " << (*it).second.first << "," <<  (*it).second.second << endl;
        
}

/** insert a new object into circuit database, this object can be a wire, io or gate.
 *  it also insert the object into it's class container. to do so, we need to use runtime type identification
 *  by using dynamic_cast. another way to do this is to use the ObjectType field in the object class. however this way is better.
 */
void CircuitDB::insert(Object* object){
    objects.push_back(object);
    netlist[object->name] = object->id;
    if(dynamic_cast<Wire*>(object)){
        wires.push_back((Wire*)object);   
        nodes++;
    }
    if(dynamic_cast<IO*>(object)){
        ios.push_back((IO*)object);
        nodes++;
        if (((IO*)object)->ioType==input) numPI++;
        if (((IO*)object)->ioType==output) numPO++;
    }
    if(dynamic_cast<Parasitic*>(object)) parasitics.push_back((Parasitic*)object);
    if(dynamic_cast<Gate*>(object)){
		Gate* g = (Gate*)object ;
		if( g->gateType == DFF ){ 
			dffs.push_back((Gate*)object); 
		}
		gates.push_back((Gate*)object);
	}
}

Object* CircuitDB::getObjectByID(int objectID){
	for(int i=0;i<objects.size();i++){
		if(objects.at(i)->id==objectID)
			return objects.at(i);
	}
	return NULL;
}

//Warning: Object's name may not be unique. Be carefull.
Object* CircuitDB::getObjectByName(string objectName){
	for(int i=0;i<objects.size();i++){
		if(objects.at(i)->name.compare(objectName)==0)
			return objects.at(i);
	}
	return NULL;
}


int CircuitDB::getGateNumber(){
    return gates.size();
}


int CircuitDB::getDFFNumber(){
    return dffs.size();
}


int CircuitDB::getPIONumber(){
    return ios.size();
}

bool CircuitDB::isNeighborXY(Gate* x, Gate* y){
    for(int i=0;i<gates[x->gateNumber]->neighbors.size();i++){
        if( gates[x->gateNumber]->neighbors[i]->gateNumber==y->gateNumber ) return true;
    }
    return false;
}

void updatePILevel(Gate* g){
	if ( g->getInputNumber() > g->backwardList.size() ){
		//This gate is connected to some primary inputs.
		g->piDistance=1; 
		return ;
	}else{
		int maxLevel = 0 ;
		for(int i=0;i<g->backwardList.size();i++){
			if( g->backwardList[i]->piDistance>maxLevel ){
				if( g->backwardList[i]->gateType != DFF){
					//TODO: This code is potentially buggy. Find a better algorithm
					maxLevel = g->backwardList[i]->piDistance ;
				}	
			}	
		}		
		g->piDistance = maxLevel+1; 
	}			
}


void updatePOLevel(Gate* g){
	if ( g->getOutputNumber() > g->forwardList.size() ){
		//This gate is connected to some primary output.
		g->poDistance=1; 
		return ;
	}else{
		int maxLevel = 0 ;
		for(int i=0;i<g->forwardList.size();i++){
			if( g->forwardList[i]->poDistance>maxLevel ){
				if( g->forwardList[i]->gateType != DFF){
					//TODO: This code is potentially buggy. Find a better algorithm
					maxLevel = g->forwardList[i]->poDistance ;
				}	
			}	
		}		
		g->poDistance = maxLevel+1; 
	}			
}

void CircuitDB::BFS(void (*f)(Gate*), int ioNumber, bool direction ){
	deque<Gate*> q ;
	vector<bool> mark = vector<bool>(getGateNumber());
	for(int i=0;i<getGateNumber();i++) mark[i]=false;
	
	//BFS initial nodes
	for(int j=0; j<ios[ioNumber]->gateList.size(); j++){
		Gate* g = ios[ioNumber]->gateList[j];
		q.push_back(g);
		mark[g->gateNumber] = true ;
		(*f)(g);
	}
	
	while(q.size()>0){
		Gate* currentGate = q.front(); q.pop_front();
		for(int i=0; i< (direction ? currentGate->forwardList.size() : currentGate->backwardList.size() ) ; i++){
			Gate* g = direction ? currentGate->forwardList[i] : currentGate->backwardList[i] ;
			if ( mark[g->gateNumber]==false){
				q.push_back(g); 
				mark[g->gateNumber]=true;
				(*f)(g);
			}
    	}
	}
}

/**
 *  Levelize will find the level of each gates from the primary inputs and saves them in gateLevelFromPI collection.
 *  Here we do a variation of breath-first-search algorithm from multiple initial start vertexes (primary-inputs) and continues on.
 *  Level of gate_i = max (level of inputs to gate i ) + 1
 *  we do not support bidir ios yet.
 */
void CircuitDB::levelizeFromPI(){
	if( getGateNumber()==0 ){ cout << "[CircuitDB] Database is empty." << endl ; return ; }
	
	//main loop
    for(int i=0;i<ios.size();i++){
    	if((ios[i]->ioType==input) || (ios[i]->ioType==bidir)){
			BFS(updatePILevel, i, true);			
		}
    }

}

/**
 *  Levelize will find the level of each gates from the primary outputs and saves them in gateLevelFromPO collection.
 *  Here we do a variation of breath-first-search algorithm from multiple initial start vertexes (primary-inputs) and continues on.
 *  Level of gate_i = max (level of inputs to gate i ) + 1
 *  we do not support bidir ios yet.
 */
void CircuitDB::levelizeFromPO(){
	if( getGateNumber()==0 ){ cout << "[CircuitDB] Database is empty." << endl ; return ; }
	
	//main loop
    for(int i=0;i<ios.size();i++){
    	if((ios[i]->ioType==output)){
			BFS(updatePOLevel, i, false);			
		}
    }
}


vector<FaultSite> CircuitDB::gateLevelPILevelDistanceBasedNeighborIdentification(Gate* g, int levelDifference){
	vector<FaultSite> results ;
	int gateNumber = g->gateNumber ;
	for(int i=0; i<gates.size();i++){
		if(i!=gateNumber){
			if( abs( gates[i]->piDistance  -  gates[gateNumber]->piDistance  ) <= levelDifference ){
				results.push_back( FaultSite( g, gates[i]) );
			}
		}
	}
	return results ;
}

vector<FaultSite> CircuitDB::gateLevelPOLevelDistanceBasedNeighborIdentification(Gate* g, int levelDifference){
	vector<FaultSite> results ;
	int gateNumber = g->gateNumber ;
	for(int i=0; i<gates.size();i++){
		if(i!=gateNumber){
			if( abs( gates[i]->poDistance  -  gates[gateNumber]->poDistance ) <= levelDifference ){
				results.push_back( FaultSite( g, gates[i]) );
			}
		}
	}
	return results ;
}

vector<FaultSite> CircuitDB::gateLevelDistanceBasedNeighborIdentification(Gate* g, int distance, bool direction){
	vector<FaultSite> site ;

	if(distance>=3){
		//include gates three levels ahead
		for(int i=0;i<g->forwardList.size();i++){
			for(int j=0;j<g->forwardList[i]->forwardList.size();j++){
				for(int k=0; k<g->forwardList[i]->forwardList[j]->forwardList.size(); k++){
					site.push_back( FaultSite( g, g->forwardList[i]->forwardList[j]->forwardList[k]));
				}
			}
		}

		//include gates three level before
		for(int i=0;i<g->backwardList.size(); i++){
			for(int j=0;j<g->backwardList[i]->backwardList.size();j++){
				for(int k=0; k<g->backwardList[i]->backwardList[j]->backwardList.size(); k++){
					site.push_back( FaultSite( g, g->backwardList[i]->backwardList[j]->backwardList[k] ));
				}
			}
		}

		//for each backward gates, include gates two level ahead
		for(int i=0; i<g->backwardList.size(); i++){
			for(int j=0; j<g->forwardList.size(); j++){
				for(int k=0; k<g->forwardList[j]->forwardList.size(); k++){
					site.push_back( FaultSite( g->backwardList[i], g->forwardList[j]->forwardList[k]));
				}
			}
		}

		//for each backward gates two level before, include gates one level ahead
		for(int i=0; i<g->backwardList.size();i++){
			for(int j=0; j<g->backwardList[i]->backwardList.size(); j++){
				for(int k=0; k<g->forwardList.size(); k++){
					site.push_back( FaultSite( g->backwardList[i]->backwardList[j], g->forwardList[k]));
				}
			}
		}

		//Now this is the part where it gets wierd!
		if( direction==false ){
			//guess what I'm doing right now!
			for(int i=0;i<g->backwardList.size();i++){
				for(int j=0;j<g->backwardList[i]->backwardList.size();j++){
					for(int k=0;k<g->backwardList.size();k++){
						if( i != k ){
							site.push_back( FaultSite(g->backwardList[i]->backwardList[j], g->backwardList[k]) );
						}
					}
				}
			}

			for(int i=0;i<g->forwardList.size();i++){
				for(int j=0;j<g->forwardList[i]->forwardList.size();j++){
					for(int k=0;k<g->forwardList.size();k++){
						if( i != k ){
							site.push_back( FaultSite(g->forwardList[i]->forwardList[j], g->forwardList[k]) );
						}
					}
				}
			}
		}
	}

	if(distance>=2){
		for(int i=0;i<g->forwardList.size(); i++){
			for(int j=0;j<g->forwardList[i]->forwardList.size();j++){
				site.push_back( FaultSite( g, g->forwardList[i]->forwardList[j]) );
			}
		}

		for(int i=0;i<g->backwardList.size(); i++){
			for(int j=0;j<g->backwardList[i]->backwardList.size();j++){
				site.push_back( FaultSite( g, g->backwardList[i]->backwardList[j]));
			}
		}

		for(int i=0;i<g->backwardList.size(); i++){
			for(int j=0; j<g->forwardList.size(); j++){
				site.push_back( FaultSite( g->backwardList[i], g->forwardList[j]) ) ;
			}
		}
		if(direction==false){
			for(int i=0;i<g->backwardList.size();i++){
				for(int j=0; j<g->backwardList.size();j++){
					if( i != j ){
					site.push_back( FaultSite(  g->backwardList[i], g->backwardList[j]) );
					}
				}
			}
			for(int i=0;i<g->forwardList.size();i++){
				for(int j=0; j<g->forwardList.size();j++){
					if( i != j ){
						site.push_back( FaultSite(  g->forwardList[i], g->forwardList[j]) );
					}
				}
			}
		}
	}

	if(distance>=1){
		for(int i=0;i<g->forwardList.size();i++){
			site.push_back( FaultSite(g, g->forwardList[i])) ;
		}
		for(int i=0;i<g->backwardList.size();i++){
			site.push_back( FaultSite(g, g->backwardList[i])) ;
		}
	}
	return site;
}


vector<FaultSite> CircuitDB::randomFaultSiteSelection(Gate* g, int howManyRandomFaultSite){
	vector<FaultSite> site ;
	for(int i=0;i<howManyRandomFaultSite; i++){
		int randomGate = random() % getGateNumber() ;
		if ( randomGate == g->gateNumber ){
			if(randomGate<getGateNumber()-10)
				randomGate = randomGate++ ;
			else
				randomGate = randomGate-- ;
		}
		site.push_back(FaultSite( g, gates[randomGate]));
	}

	return site;
}


// This function will extract adjacent gates from layout.
// to use these, the DEF reader should be enabled in configuration first.
vector<FaultSite> CircuitDB::layoutFaultSiteExtraction(Gate* g){
	vector<FaultSite> site ;
	//cout << "[Statistics] Extracting Neighbor List" << endl ;
	for(int i=0; i<g->neighbors.size();i++){
		site.push_back(FaultSite( g, g->neighbors[i] ));
	}
	return site;
}



// link method 
// Purpose:  Arranges all gates Except DFFs
// Returns none
// Parts of this code contains the [modified] legacy code from SER tool, arange_gate function
void CircuitDB::link(){

	int last_swap=0;
    for (int snum=0; snum<(getGateNumber()+2); snum++){
		for (int i=getDFFNumber(); i<getGateNumber(); i++){
            for (int j=i+1; j<getGateNumber(); j++){
				Gate* gate1 = gates[i];
				Gate* gate2 = gates[j];
                for (int k=0; k<gate1->getInputNumber(); k++)
                    if (gate1->inputs[k]->id == gate2->outputs[0]->id){
						gates[i] = gate2;
                        gates[j] = gate1;
                        last_swap = snum;
                        break;
                    }
            }
        }
        if (last_swap != snum)
            break;
	}
	if (last_swap >= getGateNumber()){
        cout << "[Warning] circuit is not arranged yet, probably asyncronized loops or floated IO. check input netlist." << endl;
    }

	for(int i=0;i<getGateNumber();i++){
		Gate* g = gates[i];
		cout << g->name << endl ;
		for(int j=0;j<g->getInputNumber();j++){
			cout << "i "<< g->inputs[j]->id << endl ;
		}
		for(int j=0;j<g->getOutputNumber();j++){
			cout << "o " << g->outputs[j]->id << endl ;
		}
		
	}


}//arange_gates
