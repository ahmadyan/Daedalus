#include "verilogreader.h"
#include "circuitdb.h"
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
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>	

using namespace std;

VerilogReader::VerilogReader(Configuration* _config, Core* _core, LoggingServices* _log){
    config=_config;
    core=_core;
    log=_log;
}

void VerilogReader::read(){
	if(config->getParameter("daedalus.input.verilog.type").compare("gate")==0){
		readVerilogNetlist();
	}else if (config->getParameter("daedalus.input.verilog.type").compare("iscas89")==0){
		readVerilogNetlistIscasFormatted();
	}
	verilogPostProcessing();
}

typedef adjacency_list<vecS, vecS, bidirectionalS> BoostGraph;

void VerilogReader::readVerilogNetlist(){
    CircuitDB* db = core->db;
	string buffer="";
    db->nodes=0;
    string line;
    string inputFile = config->getParameter("daedalus.input.verilog.filename");
    ifstream myfile (inputFile.c_str());
    if (myfile.is_open()){
        while ( myfile.good() ){
            getline (myfile,line);
            if(line.c_str()[0] =='#'){} //ignoring comments
            if(line.size()>0){
                if(line.at(line.length()-1)==';'){
                    buffer += line ;
                    vector<string> tokens;
                    //boost::split(tokens, buffer, boost::is_any_of(" "));
                    istringstream iss(buffer);
                    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
                    
                    //analyze the line
                    if(tokens.size()>0){
                        if(tokens[0].compare("input")==0){
                            if(tokens[1].at(0)=='['){
                                int l=0,r=0;
                                sscanf(tokens[1].c_str(), "[%d:%d]", &l, &r);
                                for(int i=2;i<tokens.size();i++){
                                    for(int j=r;j<l;j++){
                                        stringstream ss ;
                                        ss << utils::trimed(tokens[i]) + "[" << j << "]" ;
                                        if (utils::trimed(tokens[i]).size()>0){
                                            db->insert(new IO(ss.str(), input));
                                        }
                                    }
                                }
                            }else{
                                for(int i=1;i<tokens.size();i++){
                                    char_separator<char> sep(" \t,;");
									tokenizer<char_separator<char> > tkns(tokens[i], sep);
									BOOST_FOREACH(string t, tkns){
										if (t.size()>0){
											//ignoring VDD,GND and Clock input on ISCAS testbenches.
											if( (t.compare("GND")!=0) && (t.compare("VDD")!=0) && (t.compare("CK")!=0) ){
	                                        	db->insert(new IO(t, input));													
											} 
	                                    }	
									}
                                }
                            }
                        }else if(tokens[0].compare("output")==0){
                            if(tokens[1].at(0)=='['){
                                int l=0,r=0;
                                sscanf(tokens[1].c_str(), "[%d:%d]", &l, &r);
                                for(int i=2;i<tokens.size();i++){
                                    for(int j=r;j<l;j++){
                                        stringstream ss ;
                                        ss << utils::trimed(tokens[i]) + "[" << j << "]" ;
                                        if (utils::trimed(tokens[i]).size()>0){
                                            db->insert(new IO(ss.str(), output));
                                        }
                                    }
                                    
                                }
                            }else{
                                for(int i=1;i<tokens.size();i++){
	
									char_separator<char> sep(" \t,;");
									tokenizer<char_separator<char> > tkns(tokens[i], sep);
									BOOST_FOREACH(string t, tkns){
										if (t.size()>0){
											//ignoring VDD,GND and Clock input on ISCAS testbenches.
											if( (t.compare("GND")!=0) && (t.compare("VDD")!=0) && (t.compare("CK")!=0) ){
	                                        	db->insert(new IO(t, output));													
											} 
	                                    }	
									}
									
									
                                }
                            }
                        }else if(tokens[0].compare("wire")==0){
                            if(tokens[1].at(0)=='['){
                                int l=0,r=0;
                                sscanf(tokens[1].c_str(), "[%d:%d]", &l, &r);
                                for(int i=2;i<tokens.size();i++){
                                    for(int j=r;j<l;j++){
                                        stringstream ss ;
                                        ss << utils::trimed(tokens[i]) + "[" << j << "]" ;
                                        if (utils::trimed(tokens[i]).size()>0){
                                            db->insert(new Wire(ss.str()));
                                        }
                                    }
                                }
                            }else{
                                for(int i=1;i<tokens.size();i++){
                                    char_separator<char> sep(" \t,;");
									tokenizer<char_separator<char> > tkns(tokens[i], sep);
									BOOST_FOREACH(string t, tkns){
										if (t.size()>0){
									        db->insert(new Wire(t));												
	                                    }	
									}
									
                                }
                            }
                        }else if(tokens[0].compare("endmodule")==0){
                            return ;
                        }else if(tokens[0].compare("module")==0){
                            //   cout << line << endl ;
                        }else if(tokens[0].compare("assign")==0){   //TODO: do something for the assignments
                               size_t assignP = buffer.find_first_of("assign");
                               size_t eqP = buffer.find_first_of("=");
                               size_t semiColonP = buffer.find_first_of(";");
                               string targetName = buffer.substr(assignP+7, eqP-assignP-8);
                               string sourceName = buffer.substr(eqP+2, semiColonP-eqP-2);
                               //cout << line  << " [" << targetName << "] [" << sourceName << "]" <<  endl ;

                               string name = "AB" + targetName ;
                               vector<pair<string, string> > pins ;
                               pins.push_back( pair<string, string>(targetName, "Y") ) ;
                               pins.push_back( pair<string, string>(sourceName, "A") ) ;

                               db->insert(new Gate(utils::gateNameUnifier(name),  "buf" /* gate type*/, pins ));
                        }else{
                            size_t x = buffer.find_first_of("(");
                            string s = buffer.substr(x+1,buffer.size()-x-3);
                            string s2 ;
                            for(int i=0;i<s.size();i++){
                                if( (s.at(i)!=' ') && ((s.at(i)!=','))  && ((s.at(i)!='\n'))  && ((s.at(i)!='\t'))  && ((s.at(i)!=' ')) ){
                                    s2 += s.at(i);
                                }
                            }
                            //Now, we have something like this string to process:
                            //.B(\round/mx[45]).A(\round/sb[109]).S(n1648).Y(\round/n374)
                            buffer=s2;
                            string nodeName = "" ;
                            string pinName = "" ;
                            vector<pair<string, string> > pins ;
                            bool readNode=false, readPin=false; 
                            for(int i=0;i<buffer.size();i++){
                                if(buffer.at(i)=='.'){
                                    readPin=true ;
                                    readNode=false; 
                                    pinName = "" ;
                                }else if(buffer.at(i)=='('){
                                    readNode=true;
                                    readPin=false ;
                                    nodeName = "" ;
                                }else if(buffer.at(i)==')'){
                                    readNode=false;
                                    pins.push_back( pair<string, string>(nodeName, pinName) ) ;
                                    nodeName = "" ;
                                }else{
                                    if(readPin) pinName  += buffer.at(i);
                                    if(readNode) nodeName += buffer.at(i);
                                }
                            }
                            

                            db->insert(new Gate(utils::gateNameUnifier(tokens[1]),  tokens[0] /* gate type*/, pins ));

                        }
                    }
                    
                    //finally, empty the buffer
                    buffer="";
                }else{
                    buffer += line ;
                }
            }
            
        }
        myfile.close();
    }else{ //  if (myfile.is_open())
    	cout << "[Reader.Verilog] Verilog input file not found." << endl ;
    }
}


void VerilogReader::readVerilogNetlistIscasFormatted(){
	ofstream vout;
 	vout.open (config->getParameter("daedalus.input.verilog.outputFileName").c_str());
	cout << "[Reader.Verilog] starting. input format is [iscas89], input file is " <<  config->getParameter("daedalus.input.verilog.filename")<< endl ;
	CircuitDB* db = core->db;
	string buffer = "" ;
	db->nodes=0;
	string line;
	string inputFile = config->getParameter("daedalus.input.verilog.filename");
	ifstream myfile (inputFile.c_str());
	if (myfile.is_open()){
		while ( myfile.good() ){
			getline (myfile,line);
			if(line.c_str()[0] =='/'){} //ignoring comments
	            if(line.size()>0){
	                if(line.at(line.length()-1)==';'){
	                    buffer += line ;
	                    vector<string> tokens;

	                    //boost::split(tokens, buffer, boost::is_any_of(" "));
	                    istringstream iss(buffer);
	                    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));

	                    //analyze the line
	                    if(tokens.size()>0){
	                        if(tokens[0].compare("input")==0){
	                        	vout << line << endl ;
	                            if(tokens[1].at(0)=='['){
	                                int l=0,r=0;
	                                sscanf(tokens[1].c_str(), "[%d:%d]", &l, &r);
	                                for(int i=2;i<tokens.size();i++){
	                                    for(int j=r;j<l;j++){
	                                        stringstream ss ;
	                                        ss << utils::trimed(tokens[i]) + "[" << j << "]" ;
	                                        if (utils::trimed(tokens[i]).size()>0){
	                                            db->insert(new IO(ss.str(), input));
	                                        }
	                                    }
	                                }
	                            }else{
	                                for(int i=1;i<tokens.size();i++){
	                                    //string s = utils::trimed(tokens[i]) ;	
										char_separator<char> sep(" \t,;");
										tokenizer<char_separator<char> > tkns(tokens[i], sep);
										BOOST_FOREACH(string t, tkns){
											if (t.size()>0){
												//ignoring VDD,GND and Clock input on ISCAS testbenches.
												if( (t.compare("GND")!=0) && (t.compare("VDD")!=0) && (t.compare("CK")!=0) ){
		                                        	db->insert(new IO(t, input));													
												} 
		                                    }	
										}
										
	                                }
	                            }
	                        }else if(tokens[0].compare("output")==0){
	                        	vout << line << endl ;
	                            if(tokens[1].at(0)=='['){
	                                int l=0,r=0;
	                                sscanf(tokens[1].c_str(), "[%d:%d]", &l, &r);
	                                for(int i=2;i<tokens.size();i++){
	                                    for(int j=r;j<l;j++){
	                                        stringstream ss ;
	                                        ss << utils::trimed(tokens[i]) + "[" << j << "]" ;
	                                        if (utils::trimed(tokens[i]).size()>0){
	                                            db->insert(new IO(ss.str(), output));
	                                        }
	                                    }

	                                }
	                            }else{
	                                for(int i=1;i<tokens.size();i++){
	                                   	char_separator<char> sep(" \t,;");
										tokenizer<char_separator<char> > tkns(tokens[i], sep);
										BOOST_FOREACH(string t, tkns){
											if (t.size()>0){
												//ignoring VDD,GND and Clock input on ISCAS testbenches.
												if( (t.compare("GND")!=0) && (t.compare("VDD")!=0) && (t.compare("CK")!=0) ){
		                                        	db->insert(new IO(t, output));													
												} 
		                                    }	
										}
	                                }
	                            }
	                        }else if(tokens[0].compare("wire")==0){
	                        		vout << line << endl ;
	                            if(tokens[1].at(0)=='['){
	                                int l=0,r=0;
	                                sscanf(tokens[1].c_str(), "[%d:%d]", &l, &r);
	                                for(int i=2;i<tokens.size();i++){
	                                    for(int j=r;j<l;j++){
	                                        stringstream ss ;
	                                        ss << utils::trimed(tokens[i]) + "[" << j << "]" ;
	                                        if (utils::trimed(tokens[i]).size()>0){
	                                            db->insert(new Wire(ss.str()));
	                                        }
	                                    }
	                                }
	                            }else{
	                                for(int i=1;i<tokens.size();i++){
										char_separator<char> sep(" \t,;");
										tokenizer<char_separator<char> > tkns(tokens[i], sep);
										BOOST_FOREACH(string t, tkns){
											if (t.size()>0){
		                                        db->insert(new Wire(t));												
		                                    }	
										}
										
	                                }
	                            }
	                        }else if(tokens[0].compare("endmodule")==0){
	                        	    vout << line << endl ;
	                            return ;
	                        }else if(tokens[0].compare("module")==0){
	                               vout << line << endl ;
	                        }else if(tokens[0].compare("assign")==0){   //TODO: do something for the assignments
	                               size_t assignP = buffer.find_first_of("assign");
	                               size_t eqP = buffer.find_first_of("=");
	                               size_t semiColonP = buffer.find_first_of(";");
	                               string targetName = buffer.substr(assignP+7, eqP-assignP-8);
	                               string sourceName = buffer.substr(eqP+2, semiColonP-eqP-2);
	                               //cout << line  << " [" << targetName << "] [" << sourceName << "]" <<  endl ;

	                               string name = "AB" + targetName ;
	                               vector<pair<string, string> > pins ;
	                               pins.push_back( pair<string, string>(targetName, "Y") ) ;
	                               pins.push_back( pair<string, string>(sourceName, "A") ) ;

	                               db->insert(new Gate(utils::gateNameUnifier(name),  "buf" /* gate type*/, pins ));
	                        }else{
	                        //	cout << "* " << buffer << endl;
							size_t x = buffer.find_first_of("(");
							string s = buffer.substr(x + 1, buffer.size() - x- 3);
							string s2;
							for (int i = 0; i < s.size(); i++) {
								if ((s.at(i) != ' ') && ((s.at(i) != '\n'))
										&& ((s.at(i) != '\t')) && ((s.at(i)
										!= ' '))) {
									s2 += s.at(i);
								}
							}
							//cout << " --> " << s2 << endl ;


							vector<string> px ;
							string pxN ;
							for(int i=0;i<s2.size();i++){
								if(s2.at(i)==','){
									px.push_back(pxN);
									pxN = "" ;
								}else{
								pxN += s2.at(i);
								}
							}
							px.push_back(pxN);

							bool isDFF = false;
							string gateType ;
							string gateName ;
							if(tokens[0].compare("dff")==0){
								gateType = "DFFPOSX1" ;
								isDFF = true ;
							}else if(tokens[0].compare("and")==0){
								switch(px.size()){
									case 3: gateType = "AND2X1" ;	break;
									case 4: gateType = "AND3X1" ;	break;
									case 5: gateType = "AND4X1" ;	break;
									default: gateType = "ERROR" ;	break;
								}

							}else if(tokens[0].compare("or")==0){
								switch(px.size()){
									case 3: gateType = "OR2X1" ;	break;
									case 4: gateType = "OR3X1" ;	break;
									case 5: gateType = "OR4X1" ;	break;
									default: gateType = "ERROR" ;	break;
								}

							}else if(tokens[0].compare("nand")==0){
								switch(px.size()){
									case 3: gateType = "NAND2X1" ;	break;
									case 4: gateType = "NAND3X1" ;	break;
									case 5: gateType = "NAND4X1" ;	break;
									default: gateType = "ERROR" ;	break;
								}

							}else if(tokens[0].compare("nor")==0){
								switch(px.size()){
									case 3: gateType = "NOR2X1" ;	break;
									case 4: gateType = "NOR3X1" ;	break;
									case 5: gateType = "NOR4X1" ;	break;
									default: gateType = "ERROR" ;	break;
								}

							}else if(tokens[0].compare("xor")==0){
								switch(px.size()){
									case 3: gateType = "XOR2X1" ;	break;
									case 4: gateType = "XOR3X1" ;	break;
									case 5: gateType = "XOR4X1" ;	break;
									default: gateType = "ERROR" ;	break;
								}

							}else if(tokens[0].compare("xnor")==0){
								switch(px.size()){
									case 3: gateType = "XNOR2X1" ;	break;
									case 4: gateType = "XNOR3X1" ;	break;
									case 5: gateType = "XNOR4X1" ;	break;
									default: gateType = "ERROR" ;	break;
								}

							}else if(tokens[0].compare("not")==0){
								gateType = "INVX1" ;
							}

							vout << gateType << " " ;
							gateName = tokens[1].substr(0, tokens[1].find_first_of("("));
							vout << gateName << " " ;
							vout << "( " ;

							//vector<pair<string, string> > pins ;
                            //pins.push_back( pair<string, string>(targetName, "Y") ) ;
                            //pins.push_back( pair<string, string>(sourceName, "A") ) ;
							



							vector<string> pins ;
							string pinName ;
							for(int i=0;i<s2.size();i++){
								if(s2.at(i)==','){
									pins.push_back(pinName);
									pinName = "" ;
								}else{
									pinName += s2.at(i);
								}
							}
							pins.push_back(pinName);
							
						
							if(isDFF){
								string q = pins.at(1);
								string d = pins.at(2);
								vout << ".D(" << d << ")," ;
								vout << ".CLK(CK)," ;
								vout << ".Q(" << q << ")" ;
							}else{

								//TODO: write a better code for this part
								for(int i=1;i<pins.size();i++){
									switch(i){
									case 1:
										vout << ".A(" ;
										vout << pins.at(i);
										break ;
									case 2:
										vout << ".B(" ;
										vout << pins.at(i);
										break;
									case 3:
										vout << ".C(" ;
										vout << pins.at(i);
										break;
									case 4:
										vout << ".D(" ;
										vout << pins.at(i);
										break;
									}

									vout << "), " ;
								}
								vout << ".Y(";
								vout << pins.at(0);
								vout << ")";
							}
							vout << " );" ;
							vout << "//" << line << endl ;
							
							//this is a glue code for converting the previous pins data structure to a gate class format.
							vector<pair<string, string> > pin2 ;
							if(isDFF){
								//DFF has three pins, first is CLK which we ignore, second is input, third is output
								//pin2.push_back( pair<string, string>(pins.at(0), "CK") ) ;
								pin2.push_back( pair<string, string>(pins.at(1), "Y") ) ;
								pin2.push_back( pair<string, string>(pins.at(2), "A") ) ;
							}else{
								//For every other gate, first pin is output and next pins are input
								pin2.push_back( pair<string, string>(pins.at(0), "Y") );
								for(int i=1;i<pins.size();i++){
									pin2.push_back( pair<string, string>(pins.at(i), utils::to_string(char('A'+i-1)) ) ) ;
								}
							}
							
	                       	//Warning: This is a beta code:
							//cout << gateType << " " << gateName << endl ;
							//for(int i=0;i<pin2.size();i++){
							//	cout << pin2.at(i).first << " " << pin2.at(i).second << endl ;
							//}
							
						 	db->insert(new Gate(utils::gateNameUnifier(gateName),  tokens[0]/* gate type*/, pin2 ));
	                        }
	                    }

	                    //finally, empty the buffer
	                    buffer="";
	                }else{
	                    buffer += line ;
	                }
	            }

	        }
	        myfile.close();

	        vout << "endmodule" << endl ;
		 	vout.close();
	    }else{ //  if (myfile.is_open())
	    		cout << "[Reader.Verilog] Verilog input file not found." << endl ;
	    }
}





// This method will populate the forward and backward adjacency list for each gates.
// It is usually called after a verilog-reader.
void VerilogReader::verilogPostProcessing(){
    //for(int i=0;i<objects.size();i++){
    //    cout << objects[i]->name << endl ;
    //}
    CircuitDB* db = core->db;
    BoostGraph g = BoostGraph(db->getGateNumber());
    cout << "[Reader.Verilog] start post processing..." << endl ;

	/*
    for(int i=0;i<db->gates.size();i++){
        for(int j=0;j<db->gates[i]->pins.size();j++){
            int id = db->netlist[db->gates[i]->pins[j].first];
            for(int k=0;k<db->objects.size();k++){
                if( db->objects[k]->id == id ){
                    if(determinePinType(db->gates[i]->pins[j].second)==output){
                        db->gates[i]->forwardNodes.push_back(db->objects[k]); 
                    }else{
                        db->gates[i]->backwardNodes.push_back(db->objects[k]);
                    }
                }
            }
        }
    }*/
    


    for(int i=0;i<db->gates.size();i++){
        for(int j=0;j<db->gates[i]->pins.size();j++){
            string pinString = db->gates[i]->pins[j].second;
            string pinName = db->gates[i]->pins[j].first;
            if(determinePinType(pinString)==output){
                //find where output is connected!
                for(int m=0;m<db->gates.size();m++){
                    for(int n=0;n<db->gates[m]->pins.size();n++){
						if( db->gates[m]->pins[n].first.compare( db->gates[i]->pins[j].first )==0 ){
					        //Does not include the gate itself
                            if(determinePinType(db->gates[m]->pins[n].second)==input){
                                db->gates[i]->forwardList.push_back( db->gates[m] );
                                add_edge(i, m , g); 
                            }
                        }
                    }
                }
            }else{
                for(int m=0;m<db->gates.size();m++){
                    for(int n=0;n<db->gates[m]->pins.size();n++){
                        if(determinePinType(db->gates[m]->pins[n].second)==output){
                            if( db->gates[m]->pins[n].first == db->gates[i]->pins[j].first ){
                                db->gates[i]->backwardList.push_back( db->gates[m] );
                            }                            
                        }
                    }
                }                
            }            
        }
    }    

	/*
	for(int i=0;i<db->gates.size();i++){
		Gate* g = db->gates[i];
		cout << g->name << endl ;
		for(int j=0;j<g->forwardList.size();j++){
			cout << "   > " << g->forwardList[j]->name << endl ;
		}
		
		for(int j=0;j<g->backwardList.size();j++){
			cout << "   + " << g->backwardList[j]->name << endl ;			
		}
		cout << "%%%%%%%%%%%%" << endl ;
	}*/


    for(int i=0;i<db->ios.size();i++){
        for(int j=0;j<db->gates.size();j++){
            for(int k=0;k<db->gates[j]->pins.size();k++){
                string y = db->gates[j]->pins[k].second;
                string x = db->gates[j]->pins[k].first;;
                if( x.compare(db->ios[i]->name)==0 ){
                    db->ios[i]->addGate(db->gates[j]);
                }
            }
        }
    }
    
	cout << "[Reader.Verilog] connecting gates" << endl ;
	
	for(int i=0;i<db->getGateNumber();i++){
		for(int j=0;j<db->gates.at(i)->pins.size(); j++){
			// actual pin name: db->gates.at(i)->pins.at(j).first (like G1, G10)
			// pin name on the gate (like A, Y)
			int objID = db->netlist[ db->gates.at(i)->pins.at(j).first ] ;
			if( determinePinType(db->gates.at(i)->pins.at(j).second) == output ){
				db->gates.at(i)->outputs.push_back( db->getObjectByID(objID) ) ;
			}else{ //input
				db->gates.at(i)->inputs.push_back( db->getObjectByID(objID) ) ;		
			}
		}
	}	
	

    cout << "[Reader.Verilog] finished post processing" << endl ;
    
}

		
//TODO: Determine the input, output pins from standard cell library, DO NOT HARDCODE
IOType VerilogReader::determinePinType(string y){
    if((y.compare("Y")==0) || (y.compare("Q")==0) || (y.compare("YC")==0) || (y.compare("ZN")==0) ||  (y.compare("Z")==0)){
        return output;
    }else{
        return input;
    }
}
