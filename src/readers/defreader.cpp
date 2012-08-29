#include "defreader.h"
#include "circuitdb.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include "gate.h"
#include "utils.h"
using namespace std;
//TODO: read this from config
#define NEIGHBOR_DISTANCE  4000

DefReader::DefReader(Configuration* _config, Core* _core, LoggingServices* _log){
    config=_config;
    core=_core;
    log=_log;
}

void DefReader::read(){
    readDEF();
    extractNeighbors();
}

void DefReader::readDEF(){
    string line;
    bool components = false; 
    string inputFile = config->getParameter("daedalus.input.def.filename");
    ifstream myfile (inputFile.c_str());
    if (myfile.is_open()){
        while ( myfile.good() ){
            getline (myfile,line);
            if(line.c_str()[0] =='#'){} //ignoring comments
            
            //We reached the end of components in DEF file, we don't need the rest.
            if(line.compare("END COMPONENTS")==0){ 
                return ;
            }
            
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter<vector<string> >(tokens));
            
            if(tokens.size()>0){
                if(tokens[0].compare("COMPONENTS")==0){
                    components=true;
                }                
            }
            
            if(components){
                if(tokens[0].compare("-")==0){
                    core->db->placement[utils::gateNameUnifier(tokens[1])] = pair<int,int>( atoi(tokens[6].c_str()) , atoi(tokens[7].c_str()) ) ;
                }
            }
        }
        myfile.close();
    }
}

void DefReader::extractNeighbors(){
    cout << "[DefReader.extractNeighbors] start analyze" << endl ;
   
    map<string, pair<int,int> > ::iterator it;
    for ( it=core->db->placement.begin() ; it != core->db->placement.end(); it++ ){
        string gateName = (*it).first ;
        //cout << gateName << endl ; 
        int x = (*it).second.first ;
        int y = (*it).second.second ;
        
        for(int i=0;i<core->db->gates.size();i++){
            if(core->db->gates[i]->name.compare(gateName)==0){
                map<string, pair<int,int> > ::iterator iter;  
                for ( iter=core->db->placement.begin() ; iter != core->db->placement.end(); iter++ ){
                    string gateName2 = (*iter).first ;
                    int x2 = (*iter).second.first ;
                    int y2 = (*iter).second.second ;
                    if ( sqrt( pow(x-x2, 2.0) + pow(y-y2, 2.0) ) < NEIGHBOR_DISTANCE ){
                        if (gateName.compare(gateName2)!=0){
                            for(int j=0;j<core->db->gates.size();j++){
                                if(core->db->gates[j]->name.compare(gateName2)==0){
                                    //we found a neighbor!        
                                    //cout << "****" << gateName << " " << gateName2 << endl ;
                                    //cout << z << " " << x << " " << y << " / " << x2 << " " << y2 << endl ; 
                                    core->db->gates[i]->neighbors.push_back(core->db->gates[j]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "[DefReader.extractNeighbors] end analyze" << endl ;
}

