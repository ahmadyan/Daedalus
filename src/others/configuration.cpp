#include "configuration.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include "gate.h"
#include "utils.h"
#include "circuitdb.h"
#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>	
using namespace std;
Configuration::Configuration(){
    string line;
	string buffer = "" ;
    string inputFile = "daedalus.conf";
    ifstream myfile (inputFile.c_str());
    if (myfile.is_open()){
        while ( myfile.good() ){
            getline (myfile,line);
            if(line.c_str()[0] =='#'){} //ignoring comments
            if(line.size()>0){
				buffer += line ;
                vector<string> tokens;
				boost::split(tokens, buffer, boost::is_any_of("="));
				istringstream iss(buffer);
				copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
				if(tokens.size()>0){
					setParameter(tokens[0], tokens[1]);
				}
				buffer="";
			}
        }
        myfile.close();
    }else{ //  if (myfile.is_open())
    	cout << "[Configuration] Configuration file \"daedalus.conf\" not found." << endl ;
    }
}

string Configuration::getParameter(string parameter){
    return db[parameter];
}

void Configuration::setParameter(string key, string value){
    db[key] = value ;
}
