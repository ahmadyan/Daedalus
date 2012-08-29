#pragma once
#include <vector>
#include "faultsite.h"

using namespace std;

class VectorOps {
public:
	VectorOps(vector<FaultSite> a, vector<FaultSite> b);
	virtual ~VectorOps();
	vector<FaultSite> unionVector ;
	vector<FaultSite> intersectionVector ;
	vector<FaultSite> a_b ;
	vector<FaultSite> b_a ;
};
