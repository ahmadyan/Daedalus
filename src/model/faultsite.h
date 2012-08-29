#pragma once
#include <vector>
#include "gate.h"
using namespace std;



class FaultSite {
	vector<Gate*> sites ;
public:
	FaultSite(Gate*, Gate*);
	FaultSite();
	virtual ~FaultSite();
	bool have(Gate*);
	void add(Gate*);
	bool compare(FaultSite);
	string toString();
};


