#include "statisticsmodule.h"
#include "vectorops.h"
#include "dlib/svm.h"

using namespace dlib;
using namespace std;

/*
 Case 1: Using maximum flow algorithm
 
 Case 2: Distance of 1 from a particle strike site (this could be 
 similar to our DATE paper, i.e., GFI, GFO, CFI, CFO)
 
 Case 3: Distance of 2 or less from a particle strike site (this is 
 extended version of the fault sites we have considered in the DATE 
 paper)
 
 Case 4: Level difference of 1 or less (here leveling is counted from PIs)
 
 Case 5: Level difference of 2 or less (here leveling is counted from PIs)
 
 Case 6: Level difference of 3 or less (here leveling is counted from PIs)
 
 Case 7: Level difference of 1 or less (here leveling is counted from POs)
 
 Case 8: Level difference of 2 or less (here leveling is counted from POs)
 
 Case 9: Level difference of 3 or less (here leveling is counted from POs)
 
 Case 10: Case 2 or Case 4
 
 Case 11: Case 2 or Case 5
 
 Case 12: Case 3 or Case 4
 
 Case 13: Case 3 or Case 5

 */
/* write methods for executing this queries:
 * 1. Distance of gate x from gate y in uni/bi directional graph
 * 2. flow from gate x to gate y in uni/bi directional graph
 * 3. Distance of gate x from pi/po y
 * 4. Checking wether gate x & y are neighbors at layout
 */

StatisticsModule::StatisticsModule(Core* _core, Configuration* _config, LoggingServices* _log){
    core=_core;
    config=_config;
    log=_log;
}

void StatisticsModule::run(){
    init();
    execute();
    shutdown();
}

void StatisticsModule::init(){
    cout << "[Execution.StatisticsModule] init" << endl ;
}


void StatisticsModule::execute(){
	core->db->levelizeFromPI();
	core->db->levelizeFromPO();
	
	//dumpCircuit();
	if(config->getParameter("daedalus.mode")=="daedalus.stat.neighbor"){
		extractStatisticalInformationForNeighbors();
		extractStatisticalInformationForNonNeighbors();
	}
	
	learningAdjacencyModelUsingSVM();
	/*
	 * TCAD Experiments on fault-site identification at gate-level
	 *
	GateLevelNeighborIdentificationEngine(X00_PILDBNI_D1);
	GateLevelNeighborIdentificationEngine(X01_PILDBNI_D2);
	GateLevelNeighborIdentificationEngine(X02_PILDBNI_D3);
	GateLevelNeighborIdentificationEngine(X10_POLDBNI_D1);
	GateLevelNeighborIdentificationEngine(X11_POLDBNI_D2);
	GateLevelNeighborIdentificationEngine(X12_POLDBNI_D3);
	GateLevelNeighborIdentificationEngine(X20_GDBNI_D1_Directed);
	GateLevelNeighborIdentificationEngine(X21_GDBNI_D2_Directed);
	GateLevelNeighborIdentificationEngine(X22_GDBNI_D3_Directed);
	GateLevelNeighborIdentificationEngine(X30_GDDBNI_D1);
	GateLevelNeighborIdentificationEngine(X31_GDDBNI_D2);
	GateLevelNeighborIdentificationEngine(X32_GDDBNI_D3);
	GateLevelNeighborIdentificationEngine(X40_RAND);
	*/
	//GateLevelNeighborIdentificationEngine(X31_GDDBNI_D2);




	//These two function will profile the statistical information
	//about neighbor and non-neighbor gates.
	//extractStatisticalInformationForNeighbors();
	//extractStatisticalInformationForNonNeighbors();


	//This function will create a list of adjacent cells in a text file
	//the resulted list can be backwarded to SER tool as an input with -b switch.
	//extractNeighbor(X31_GDDBNI_D2);
}

void StatisticsModule::shutdown(){
      cout << "[Execution.StatisticsModule] Shutting Down" << endl ;
}


void StatisticsModule::GateLevelNeighborIdentificationEngine(GateLevelExperienceType xpType){
	cout << endl << endl ;
	cout << "[Execution.StatisticsModule] Experiment " << xpType << endl ;
	for(int gateNumber=0;gateNumber<core->db->getGateNumber(); gateNumber++){
		Gate* g = core->db->gates[gateNumber];
		std::vector<FaultSite> Nb ;
		switch (xpType){
			case X00_PILDBNI_D1:			Nb = core->db->gateLevelPILevelDistanceBasedNeighborIdentification(g, 1);	break ;
			case X01_PILDBNI_D2: 			Nb = core->db->gateLevelPILevelDistanceBasedNeighborIdentification(g, 2); 	break ;
			case X02_PILDBNI_D3: 			Nb = core->db->gateLevelPILevelDistanceBasedNeighborIdentification(g, 3); 	break ;
			case X10_POLDBNI_D1:			Nb = core->db->gateLevelPOLevelDistanceBasedNeighborIdentification(g, 1);	break ;
			case X11_POLDBNI_D2:	 		Nb = core->db->gateLevelPOLevelDistanceBasedNeighborIdentification(g, 2);	break ;
			case X12_POLDBNI_D3:	 		Nb = core->db->gateLevelPOLevelDistanceBasedNeighborIdentification(g, 3);	break ;
			case X20_GDBNI_D1_Directed:		Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 1, true );	break ;
			case X21_GDBNI_D2_Directed:		Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 2, true );	break ;
			case X22_GDBNI_D3_Directed:		Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 3, true );	break ;
			case X30_GDDBNI_D1:				Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 1, false );	break ;
			case X31_GDDBNI_D2:				Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 2, false );	break ;
			case X32_GDDBNI_D3:				Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 3, false );	break ;
			case X40_RAND:					Nb = core->db->randomFaultSiteSelection(g, 100); break;
			default:						Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 1, true );	break ;

		}

		std::vector<FaultSite> N ;
		for(int i=0;i<g->neighbors.size();i++){
			N.push_back( FaultSite( g, g->neighbors[i])) ;
		}

		VectorOps v = VectorOps(N, Nb) ;
		int n = core->db->getGateNumber() ;
		int M =  n * (n-1) / 2 ;
		int unionSize = v.unionVector.size() ;
		int intersectionSize = v.intersectionVector.size() ;
		int TruePositive = intersectionSize ;			float TruePositiveNorm = (1.0*intersectionSize) / (1.0*Nb.size()) ;
		int TrueNegative = M - unionSize ;				float TrueNegativeNorm =  1.0*(M-unionSize) / (1.0*(M-Nb.size())) ;
		int FalsePositive = v.b_a.size() ;		float FalsePositiveNorm = 1.0*FalsePositive / (1.0*Nb.size()) ;
		int FalseNegative = v.a_b.size() ;		float FalseNegativeNorm = 1.0*FalseNegative / (1.0*(M-Nb.size())) ;

		cout << gateNumber << "\t " << N.size() << " " << Nb.size() << " " << unionSize << " " << intersectionSize << "\t " \
				  << TruePositive << " " << TruePositiveNorm << "\t " \
				  << TrueNegative << " " << TrueNegativeNorm << " \t" \
				  << FalsePositive << " " << FalsePositiveNorm << "\t " \
				  << FalseNegative << " " << FalseNegativeNorm << " " \
				  << endl ;

	}
}

void StatisticsModule::dumpCircuit(){
	cout << core->db->getGateNumber() << endl ;
	for(int i=0;i<core->db->getGateNumber();i++){
		
		cout << core->db->gates[i]->name << endl ;
		cout << core->db->gates[i]->neighbors.size() << endl ;
	}
}

void StatisticsModule::extractStatisticalInformationForNeighbors(){
	cout << "[Execution.StatisticsModule] Extracting statistical data from neighbor cells at layout" << endl ;

	for(int i=0;i<core->db->getGateNumber();i++){
		for(int j=0; j<core->db->gates[i]->neighbors.size();j++){
			int neighborGateNumber = core->db->gates[i]->neighbors[j]->gateNumber ;
			int gateNumber = core->db->gates[i]->gateNumber;
			int gateIDistanceFromPI = core->db->gates[gateNumber]->piDistance ;
			int gateJDistanceFromPI = core->db->gates[neighborGateNumber]->piDistance ;
			int gateIDistanceFromPO = core->db->gates[gateNumber]->poDistance ;
			int gateJDistanceFromPO = core->db->gates[neighborGateNumber]->poDistance ;
			int gateIJDistance = core->distanceXtoY(core->db->gates[i], core->db->gates[i]->neighbors[j], false);
			int gateIJFlow = core->flowXtoY(core->db->gates[i], core->db->gates[i]->neighbors[j],false);
			cerr << "+1 " << //i << " " << j << " " << 
					// gateNumber << " " << neighborGateNumber << " " << 
					gateIDistanceFromPI << " " << gateJDistanceFromPI << " " <<
					gateIDistanceFromPO << " " << gateJDistanceFromPO << " " <<
					gateIJDistance << " " <<
					gateIJFlow  << endl ;
			
		}

	}
}


void StatisticsModule::extractStatisticalInformationForNonNeighbors(){
	cout << "[Execution.StatisticsModule] Extracting statistical data from non neighbor cells at layout" << endl ;
	int xcount=0, sampleNumber = 20 ;

	do{
		Gate* x = core->db->gates[ std::rand()%core->db->getGateNumber() ] ;
		Gate* y = core->db->gates[ std::rand()%core->db->getGateNumber() ] ;

		if(!(core->isNeighborXY(x,y))){
			int neighborGateNumber = x->gateNumber ;
			int gateNumber = y->gateNumber;
			int gateIDistanceFromPI = core->db->gates[gateNumber]->piDistance ;
			int gateJDistanceFromPI = core->db->gates[neighborGateNumber]->piDistance ;
			int gateIDistanceFromPO = core->db->gates[gateNumber]->poDistance ;
			int gateJDistanceFromPO = core->db->gates[neighborGateNumber]->poDistance ;
			int gateIJDistance = core->distanceXtoY(x, y, false);
			int gateIJFlow = core->flowXtoY(x, y,false);
						cerr << "-1 "<< // xcount << " " << 0 << " " <<
						//gateNumber << " " << neighborGateNumber << " " <<
						gateIDistanceFromPI << " " << gateJDistanceFromPI << " " <<
						gateIDistanceFromPO << " " << gateJDistanceFromPO << " " <<
						gateIJDistance << " " <<
						gateIJFlow  << endl ;
		}
		xcount++;
	}while(xcount<sampleNumber);
}

//This function will generate the neighbor list and write it to a text file.
//I use this text file to pipe this to SER-MLET tool
void StatisticsModule::extractNeighbor(GateLevelExperienceType xpType){
	cout << "[Statistics] Generating adjacency list." << endl ;
	cerr << core->db->getGateNumber() << endl ;

	for(int gateNumber=0;gateNumber<core->db->getGateNumber(); gateNumber++){
		cerr << gateNumber << endl ;
		Gate* g = core->db->gates[gateNumber];
		std::vector<FaultSite> Nb ;
		switch (xpType){
			case X00_PILDBNI_D1:			Nb = core->db->gateLevelPILevelDistanceBasedNeighborIdentification(g, 1);	break ;
			case X01_PILDBNI_D2: 			Nb = core->db->gateLevelPILevelDistanceBasedNeighborIdentification(g, 2); 	break ;
			case X02_PILDBNI_D3: 			Nb = core->db->gateLevelPILevelDistanceBasedNeighborIdentification(g, 3); 	break ;				
			case X10_POLDBNI_D1:			Nb = core->db->gateLevelPOLevelDistanceBasedNeighborIdentification(g, 1);	break ;
			case X11_POLDBNI_D2:	 		Nb = core->db->gateLevelPOLevelDistanceBasedNeighborIdentification(g, 2);	break ;
			case X12_POLDBNI_D3:	 		Nb = core->db->gateLevelPOLevelDistanceBasedNeighborIdentification(g, 3);	break ;
			case X20_GDBNI_D1_Directed:		Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 1, true );	break ;
			case X21_GDBNI_D2_Directed:		Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 2, true );	break ;
			case X22_GDBNI_D3_Directed:		Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 3, true );	break ;
			case X30_GDDBNI_D1:				Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 1, false );	break ;
			case X31_GDDBNI_D2:				Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 2, false );	break ;
			case X32_GDDBNI_D3:				Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 3, false );	break ;
			case X40_RAND:					Nb = core->db->randomFaultSiteSelection(g, 100); break;
			case X50_REF:					Nb = core->db->layoutFaultSiteExtraction(g); break;
			default:						Nb = core->db->gateLevelDistanceBasedNeighborIdentification(g, 1, true );	break ;
		}

		cerr << Nb.size() << endl ;
		for(int i=0; i<Nb.size(); i++){
			cerr << Nb[i].toString() << endl ;
		}
	}
}

void StatisticsModule::learningAdjacencyModelUsingSVM(){
	// The svm functions use column vectors to contain a lot of the data on which they 
    // operate. So the first thing we do here is declare a convenient typedef.  

    // This typedef declares a matrix with 2 rows and 1 column.  It will be the
    // object that contains each of our 2 dimensional samples.   (Note that if you wanted 
    // more than 2 features in this vector you can simply change the 2 to something else.
    // Or if you don't know how many features you want until runtime then you can put a 0
    // here and use the matrix.set_size() member function)
    typedef matrix<double, 6, 1> sample_type;

    // This is a typedef for the type of kernel we are going to use in this example.
    // In this case I have selected the radial basis kernel that can operate on our
    // 2D sample_type objects
    typedef radial_basis_kernel<sample_type> kernel_type;


    // Now we make objects to contain our samples and their respective labels.
    std::vector<sample_type> samples;
    std::vector<double> labels;

    // Now lets put some data into our samples and labels objects.  

	for(int i=0;i<core->db->getGateNumber();i++){
		for(int j=0; j<core->db->gates[i]->neighbors.size();j++){
			int neighborGateNumber = core->db->gates[i]->neighbors[j]->gateNumber ;
			int gateNumber = core->db->gates[i]->gateNumber;
			int gateIDistanceFromPI = core->db->gates[gateNumber]->piDistance ;
			int gateJDistanceFromPI = core->db->gates[neighborGateNumber]->piDistance ;
			int gateIDistanceFromPO = core->db->gates[gateNumber]->poDistance ;
			int gateJDistanceFromPO = core->db->gates[neighborGateNumber]->poDistance ;
			int gateIJDistance = core->distanceXtoY(core->db->gates[i], core->db->gates[i]->neighbors[j], false);
			int gateIJFlow = core->flowXtoY(core->db->gates[i], core->db->gates[i]->neighbors[j],false);
			
			
			sample_type samp;
            samp(0) = gateIDistanceFromPI;
            samp(1) = gateJDistanceFromPI;
            samp(2) = gateIDistanceFromPO;
            samp(3) = gateJDistanceFromPO;
            samp(4) = gateIJDistance;
            samp(5) = gateIJFlow;

            samples.push_back(samp);
			labels.push_back(+1);
		}
	}


	int xcount=0, sampleNumber = 200 ;

	do{
		Gate* x = core->db->gates[ std::rand()%core->db->getGateNumber() ] ;
		Gate* y = core->db->gates[ std::rand()%core->db->getGateNumber() ] ;

		if(!(core->isNeighborXY(x,y))){
			int neighborGateNumber = x->gateNumber ;
			int gateNumber = y->gateNumber;
			int gateIDistanceFromPI = core->db->gates[gateNumber]->piDistance ;
			int gateJDistanceFromPI = core->db->gates[neighborGateNumber]->piDistance ;
			int gateIDistanceFromPO = core->db->gates[gateNumber]->poDistance ;
			int gateJDistanceFromPO = core->db->gates[neighborGateNumber]->poDistance ;
			int gateIJDistance = core->distanceXtoY(x, y, false);
			int gateIJFlow = core->flowXtoY(x, y,false);
			
				sample_type samp;
	            samp(0) = gateIDistanceFromPI;
	            samp(1) = gateJDistanceFromPI;
	            samp(2) = gateIDistanceFromPO;
	            samp(3) = gateJDistanceFromPO;
	            samp(4) = gateIJDistance;
	            samp(5) = gateIJFlow;

	            samples.push_back(samp);
				labels.push_back(-1);
				
		}
		xcount++;
	}while(xcount<sampleNumber);





    // Here we normalize all the samples by subtracting their mean and dividing by their standard deviation.
    // This is generally a good idea since it often heads off numerical stability problems and also 
    // prevents one large feature from smothering others.  Doing this doesn't matter much in this example
    // so I'm just doing this here so you can see an easy way to accomplish this with 
    // the library.  
    vector_normalizer<sample_type> normalizer;
    // let the normalizer learn the mean and standard deviation of the samples
    normalizer.train(samples);
    // now normalize each sample
    for (unsigned long i = 0; i < samples.size(); ++i)
        samples[i] = normalizer(samples[i]); 


    // Now that we have some data we want to train on it.  However, there are two parameters to the 
    // training.  These are the nu and gamma parameters.  Our choice for these parameters will 
    // influence how good the resulting decision function is.  To test how good a particular choice 
    // of these parameters is we can use the cross_validate_trainer() function to perform n-fold cross
    // validation on our training data.  However, there is a problem with the way we have sampled 
    // our distribution above.  The problem is that there is a definite ordering to the samples.  
    // That is, the first half of the samples look like they are from a different distribution 
    // than the second half.  This would screw up the cross validation process but we can 
    // fix it by randomizing the order of the samples with the following function call.
    randomize_samples(samples, labels);


    // The nu parameter has a maximum value that is dependent on the ratio of the +1 to -1 
    // labels in the training data.  This function finds that value.
    const double max_nu = maximum_nu(labels);

    // here we make an instance of the svm_nu_trainer object that uses our kernel type.
    svm_nu_trainer<kernel_type> trainer;

    // Now we loop over some different nu and gamma values to see how good they are.  Note
    // that this is a very simple way to try out a few possible parameter choices.  You 
    // should look at the model_selection_ex.cpp program for examples of more sophisticated 
    // strategies for determining good parameter choices.
    cout << "doing cross validation" << endl;
    for (double gamma = 0.00001; gamma <= 1; gamma *= 5)
    {
        for (double nu = 0.00001; nu < max_nu; nu *= 5)
        {
            // tell the trainer the parameters we want to use
            trainer.set_kernel(kernel_type(gamma));
            trainer.set_nu(nu);

            cout << "gamma: " << gamma << "    nu: " << nu;
            // Print out the cross validation accuracy for 3-fold cross validation using the current gamma and nu.  
            // cross_validate_trainer() returns a row vector.  The first element of the vector is the fraction
            // of +1 training examples correctly classified and the second number is the fraction of -1 training 
            // examples correctly classified.
            cout << "     cross validation accuracy: " << cross_validate_trainer(trainer, samples, labels, 3);
        }
    }


    // From looking at the output of the above loop it turns out that a good value for 
    // nu and gamma for this problem is 0.15625 for both.  So that is what we will use.

    // Now we train on the full set of data and obtain the resulting decision function.  We use the
    // value of 0.15625 for nu and gamma.  The decision function will return values >= 0 for samples it predicts
    // are in the +1 class and numbers < 0 for samples it predicts to be in the -1 class.
    trainer.set_kernel(kernel_type(0.15625));
    trainer.set_nu(0.15625);
    typedef decision_function<kernel_type> dec_funct_type;
    typedef normalized_function<dec_funct_type> funct_type;

    // Here we are making an instance of the normalized_function object.  This object provides a convenient 
    // way to store the vector normalization information along with the decision function we are
    // going to learn.  
    funct_type learned_function;
    learned_function.normalizer = normalizer;  // save normalization information
    learned_function.function = trainer.train(samples, labels); // perform the actual SVM training and save the results

    // print out the number of support vectors in the resulting decision function
    cout << "\nnumber of support vectors in our learned_function is " 
         << learned_function.function.basis_vectors.nr() << endl;

    // now lets try this decision_function on some samples we haven't seen before 
    sample_type sample;

	//+1 6 2 1 5 4 1
	//+1 6 4 1 3 3 1
	//+1 2 6 5 1 4 1
	//+1 1 1 4 1 5 1
	
    sample(0) = 6;
    sample(1) = 2;
    sample(2) = 1;
    sample(3) = 5;
    sample(4) = 4;
    sample(5) = 1;
    cout << "This sample should be >= 0 and it is classified as a " << learned_function(sample) << endl;

    sample(0) = 6;
    sample(1) = 4;
    sample(2) = 1;
    sample(3) = 3;
    sample(4) = 3;
    sample(5) = 1;
    cout << "This sample should be >= 0 and it is classified as a " << learned_function(sample) << endl;

//	-1 7 4 3 3 3 2
//	-1 3 6 4 1 4 1
//	-1 2 1 5 5 3 1
//	-1 6 1 1 1 6 1
	
    sample(0) = 7;
    sample(1) = 4;
    sample(2) = 3;
    sample(3) = 3;
    sample(4) = 3;
    sample(5) = 2;
    cout << "This sample should be < 0 and it is classified as a " << learned_function(sample) << endl;

    sample(0) = 6;
    sample(1) = 1;
    sample(2) = 1;
	sample(3) = 1;
    sample(4) = 6;
    sample(5) = 1;
    cout << "This sample should be < 0 and it is classified as a " << learned_function(sample) << endl;


    // We can also train a decision function that reports a well conditioned probability 
    // instead of just a number > 0 for the +1 class and < 0 for the -1 class.  An example 
    // of doing that follows:
    typedef probabilistic_decision_function<kernel_type> probabilistic_funct_type;  
    typedef normalized_function<probabilistic_funct_type> pfunct_type;

    pfunct_type learned_pfunct; 
    learned_pfunct.normalizer = normalizer;
    learned_pfunct.function = train_probabilistic_decision_function(trainer, samples, labels, 3);
    // Now we have a function that returns the probability that a given sample is of the +1 class.  

    // print out the number of support vectors in the resulting decision function.  
    // (it should be the same as in the one above)
    cout << "\nnumber of support vectors in our learned_pfunct is " 
         << learned_pfunct.function.decision_funct.basis_vectors.nr() << endl;

    sample(0) = 6;
    sample(1) = 2;
    sample(2) = 1;
    sample(3) = 5;
    sample(4) = 4;
    sample(5) = 1;
    cout << "This +1 example should have high probability.  Its probability is: " << learned_pfunct(sample) << endl;

    sample(0) = 6;
    sample(1) = 4;
    sample(2) = 1;
    sample(3) = 3;
    sample(4) = 3;
    sample(5) = 1;
    cout << "This +1 example should have high probability.  Its probability is: " << learned_pfunct(sample) << endl;

  sample(0) = 7;
    sample(1) = 4;
    sample(2) = 3;
    sample(3) = 3;
    sample(4) = 3;
    sample(5) = 2;
    cout << "This -1 example should have low probability.  Its probability is: " << learned_pfunct(sample) << endl;

  sample(0) = 6;
    sample(1) = 1;
    sample(2) = 1;
	sample(3) = 1;
    sample(4) = 6;
    sample(5) = 1;
    cout << "This -1 example should have low probability.  Its probability is: " << learned_pfunct(sample) << endl;



    // Another thing that is worth knowing is that just about everything in dlib is serializable.
    // So for example, you can save the learned_pfunct object to disk and recall it later like so:
    ofstream fout("saved_function.dat",ios::binary);
    serialize(learned_pfunct,fout);
    fout.close();

    // now lets open that file back up and load the function object it contains
    ifstream fin("saved_function.dat",ios::binary);
    deserialize(learned_pfunct, fin);

    // Note that there is also an example program that comes with dlib called the file_to_code_ex.cpp
    // example.  It is a simple program that takes a file and outputs a piece of C++ code 
    // that is able to fully reproduce the file's contents in the form of a std::string object.  
    // So you can use that along with the std::istringstream to save learned decision functions
    // inside your actual C++ code files if you want.  




    // Lastly, note that the decision functions we trained above involved well over 200 
    // basis vectors.  Support vector machines in general tend to find decision functions
    // that involve a lot of basis vectors.  This is significant because the more 
    // basis vectors in a decision function, the longer it takes to classify new examples.
    // So dlib provides the ability to find an approximation to the normal output of a
    // trainer using fewer basis vectors.  

    // Here we determine the cross validation accuracy when we approximate the output
    // using only 10 basis vectors.  To do this we use the reduced2() function.  It
    // takes a trainer object and the number of basis vectors to use and returns 
    // a new trainer object that applies the necessary post processing during the creation
    // of decision function objects.
    cout << "\ncross validation accuracy with only 10 support vectors: " 
         << cross_validate_trainer(reduced2(trainer,10), samples, labels, 3);

    // Lets print out the original cross validation score too for comparison.
    cout << "cross validation accuracy with all the original support vectors: " 
         << cross_validate_trainer(trainer, samples, labels, 3);

    // When you run this program you should see that, for this problem, you can reduce 
    // the number of basis vectors down to 10 without hurting the cross validation
    // accuracy. 


    // To get the reduced decision function out we would just do this:
    learned_function.function = reduced2(trainer,10).train(samples, labels);
    // And similarly for the probabilistic_decision_function: 
    learned_pfunct.function = train_probabilistic_decision_function(reduced2(trainer,10), samples, labels, 3);
}
