
#include "vectorops.h"

VectorOps::VectorOps(vector<FaultSite> a, vector<FaultSite> b) {
	for(int i=0; i<a.size(); i++){
		bool exists = false ;
		for(int j=0; j<b.size(); j++){
			if( a[i].compare(b[j]) ){
				exists=true;
			}
		}
		if(exists){
			intersectionVector.push_back(a[i]);
		}else{
			unionVector.push_back(a[i]);
			a_b.push_back(a[i]);
		}
	}

	for(int i=0; i<b.size(); i++){
			bool exists = false ;
			for(int j=0; j<a.size(); j++){
				if( b[i].compare(a[j]) ){
					exists=true;
				}
			}
			if(!exists){
				b_a.push_back(b[i]);
			}
		}
	for(int i=0;i<b.size();i++) unionVector.push_back(b[i]);

}

VectorOps::~VectorOps() {}
