#include "FaultSite.h"
#include <sstream>
#include <iostream>
FaultSite::FaultSite(Gate* g0, Gate* g1) {
	sites.push_back(g0);
	sites.push_back(g1);
}

FaultSite::FaultSite() {}

FaultSite::~FaultSite() {
	// TODO Auto-generated destructor stub
}

bool FaultSite::have(Gate* g){
	bool result = false ;
	for(int i=0;i<sites.size(); i++){
		if(sites[i]==g) result = true ;
	}
	return result ;
}

void FaultSite::add(Gate* g){
	if (!have(g)) sites.push_back(g);
}

bool FaultSite::compare(FaultSite f){
	if( (f.sites[0] == sites[0])&&(f.sites[1] == sites[1]) ){
		return true ;
	}else if( (f.sites[0] == sites[1])&&(f.sites[1] == sites[0]) ){
		return true ;
	}else{
		return false ;
	}
}

string FaultSite::toString(){
	stringstream ss ;
	for(int i=0;i<sites.size();i++) ss << sites[i]->gateNumber << " " ;
	//ss << "[ " ;
	//for(int i=0;i<sites.size();i++) ss << sites[i]->name << "," ;
	//ss << "] " ;
	return ss.str();
}

