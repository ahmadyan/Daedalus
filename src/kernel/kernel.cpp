//
//  kernel.cpp
//  Daedalus
//
//  Created by adel on 4/10/2011
//  (C) 2011 Seyed Nematollah Ahmadyan.
//

#include "kernel.h"
#include "readeradapter.h"
#include "statisticsmodule.h"

Kernel::Kernel(int argc, char** argv){
    config = new Configuration();
    core = new Core(config);
    log = new LoggingServices(config);
}

void Kernel::main(){
    read();
	//core->db->link();
    execute();
    shutdown();
}

void Kernel::read(){
    //show a Welcome message:
    cout << "Daedalus Kernel booting up." << endl ;
    
    
    //-------------------------------------------
    //	Initializing ReaderAdapter and Readers and start reading input files
    //  readerAdapter will automatically starts reading various input formats and files based from configuration, it can also read the configuration xml file and build up the configuration class if necessary.
    //-------------------------------------------
    ReaderAdapter* readerAdapter = new ReaderAdapter(config, core, log);
    readerAdapter->read();
    
    
    //----------------------------------------------------------------------------
    //	Generate a graph database from the circuit. 
    //  Used for statistical and graph algorithm module
    //TODO: We should generate circuit graphs only when we need them, not always.
    //----------------------------------------------------------------------------
    core->generateCircuitGraphs();
    
    
    
    /* core->gate_numbers_noff = core->gate_numbers - core->dff_num;
     config->max_iteration = 2<<(core->numPi+core->dff_num-1);
     core->arange_gates();
     */
    
    
    /*
     //----------------------------------------------------------------------------
     //	Handling input arguments and building up configuration class.
     //----------------------------------------------------------------------------
     config = new configuration();
     ArgumentHandler* argHandler = new ArgumentHandler();
     argHandler->parse(argc, argv, config);
     argHandler->initial_checking(config);
     
     
     
     //----------------------------------------------------------------------------
     //	Creating and loading technology library
     //----------------------------------------------------------------------------
     library* lib = new library();
     if(config->use_technoloy_library){
     lib->read_library(config->library_file_name);
     //lib->analyze();
     }
     
     //----------------------------------------------------------------------------
     //	loading VCD dump for application derating
     //----------------------------------------------------------------------------
     vcd* VCD  = new vcd();
     if( config->use_vcd ){
     VCD->read_vcd(config->vcd_file_name);
     VCD->setClk(config->clk);
     if(config->debug) VCD->dump();
     }
     
     
     //----------------------------------------------------------------------------
     //	Core initialization, reading netlist, initial arrangement, etc.
     //----------------------------------------------------------------------------
     fp = fopen(config->input_file_name, "r");    
     resultfp = fopen(config->output_file_name, "w");
     // Exit if error in opening file
     if(fp==NULL) {
     cout << "input netlist file not found" << endl;
     exit(1);
     }
     
     Core* core = new Core(config);
     
     utils::tick();
     // Read netlist and generate a graph of the circuit
     if(config->fpga_enable){	// the input netlist is a fpga, blif format
     fpga* f = new fpga(config);
     f->read_blif(fp, core);
     f->convert_blif_to_core(resultfp, VCD, core);
     }else{				// the input netlist for asic in verilog format
     core->readNetlist_verilog(fp, VCD, config, resultfp);
     }
     //core->report_netlist();
     //alloc_init_mem();	// Allocate initial memory
     core->gate_numbers_noff = core->gate_numbers - core->dff_num;
     config->max_iteration = 2<<(core->numPi+core->dff_num-1);
     core->arange_gates();
     
     
     if(config->verbose){
     cout << "Number of inputs:" << core->numPi << endl;
     fprintf(resultfp,"\nNumber of inputs: %d \n", core->numPi);
     cout << "Number of outputs:" << core->numPo << endl;
     fprintf(resultfp,"Number of outputs: %d \n", core->numPo);
     cout << "Number of flip-flops:" << core->dff_num << endl;
     fprintf(resultfp,"Number of flip-flops: %d \n", core->dff_num);
     cout << "Number of gates (including FFs):" << core->gate_numbers << endl;
     fprintf(resultfp,"Number of gates (including FFs): %d \n", core->gate_numbers);
     }
     utils::tock("extract netlist and arrange it", resultfp);
     
     
     //----------------------------------------------------------------------------
     //	Computing SPs 
     //----------------------------------------------------------------------------
     utils::tick();
     core->compute_signal_probability(config,VCD,resultfp);
     //core->signal_probability_report();
     utils::tock("signal probability", resultfp);
     
     
     //----------------------------------------------------------------------------
     //	Compute circuit delay from either default (0.25u) technology or the library
     //	file provided by the user
     //----------------------------------------------------------------------------
     // Computing gate delay ( based on technology ) and circuit delay.
     if(config->use_technoloy_library){
     core->compute_circuit_delay_from_library(lib);
     }else{
     core->compute_circuit_delay(false);
     }
     config->tperiod = (int)core->max_delay + config->t_setup + config->t_hold;
     fprintf(resultfp, "\n Circuit period = %d \n", config->tperiod);
     delete lib;
     
     
     //simulate all gates!
     core->sim_gate_numbers = core->gate_numbers;
     
     //----------------------------------------------------------------------------
     //	Performing monte carlo simulation for calculating system soft error rate
     //  this is for Single event transint only
     //----------------------------------------------------------------------------
     if(config->simulation_enable){
     utils::tick();
     SEU* seu = new SEU(core);
     seu->system_failure_simulation_MultiCycle_LogicalDerating(config,core,resultfp);
     seu->report(core,resultfp);
     delete seu;
     utils::tock("Simulation", resultfp);
     }
     
     
     // DSN-2011 & DATE-2010 MBU code:
     MBU* mbu = new MBU(core, config);
     FaultGenerator* fg = new FaultGenerator(core);
     FaultSiteGenerator* fsg = new FaultSiteGenerator(core, 0);
     //FaultSiteGenerator* fsg2= new FaultSiteGenerator(core, 1);
     //cout <<"done!" << endl ;
     //fsg->reportFaultSiteCorrelation(core, fsg->faultSiteList, fsg2->faultSiteList);
     //simulate all gates!
     core->sim_gate_numbers = core->gate_numbers;
     config->clock=1; //single cycle
     utils::tick();
     mbu->system_failure_simulation_single_cycle(core, config, fg, fsg, resultfp);
     //mbu->system_failure_simulation(core, config, fg, fsg, resultfp);
     //mbu->system_failure_simulation_report_ff_po(core, config, fg, fsg, resultfp);
     //mbu->system_failure_simulation_single(core, config, fg, fsg, resultfp);
     utils::tock("MBU-Simulation", resultfp);
     
     utils::tick();
     mbu->system_failure_analysis_single_cycle(core, config, fg, fsg, resultfp);
     //mbu->system_failure_analysis(core, config, fg, fsg, resultfp);
     //mbu->system_failure_analysis_single(core, config, fg, fsg, resultfp);
     utils::tock("MBU-Analytical", resultfp);
     delete mbu ;
     
     
    fp = fopen(config->input_file_name, "r");    
	resultfp = fopen(config->output_file_name, "w");
	// Exit if error in opening file
	if(fp==NULL) {
		cout << "input netlist file not found" << endl;
		exit(1);
	}
	
	Core* core = new Core(config);
    
	utils::tick();
    // Read netlist and generate a graph of the circuit
    if(config->fpga_enable){	// the input netlist is a fpga, blif format
        fpga* f = new fpga(config);
        f->read_blif(fp, core);
        f->convert_blif_to_core(resultfp, VCD, core);
    }else{				// the input netlist for asic in verilog format
        core->readNetlist_verilog(fp, VCD, config, resultfp);
    }
    //core->report_netlist();
    //alloc_init_mem();	// Allocate initial memory
    core->gate_numbers_noff = core->gate_numbers - core->dff_num;
    config->max_iteration = 2<<(core->numPi+core->dff_num-1);
    core->arange_gates();
    
    
    if(config->verbose){
        cout << "Number of inputs:" << core->numPi << endl;
        fprintf(resultfp,"\nNumber of inputs: %d \n", core->numPi);
        cout << "Number of outputs:" << core->numPo << endl;
        fprintf(resultfp,"Number of outputs: %d \n", core->numPo);
        cout << "Number of flip-flops:" << core->dff_num << endl;
        fprintf(resultfp,"Number of flip-flops: %d \n", core->dff_num);
        cout << "Number of gates (including FFs):" << core->gate_numbers << endl;
        fprintf(resultfp,"Number of gates (including FFs): %d \n", core->gate_numbers);
    }
	utils::tock("extract netlist and arrange it", resultfp);
     */
}

void Kernel::execute(){
    cout << "[Kernel] Executing ..." << endl ;
    
    StatisticsModule* m = new StatisticsModule(core,config,log);
    m->run();
}

void Kernel::shutdown(){
    cout << "[Kernel] Shutting down ..." << endl ;
}
