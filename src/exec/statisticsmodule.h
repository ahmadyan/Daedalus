#pragma once
#include "module.h"
#include "gate.h"
#include "io.h"
#include "core.h"

class StatisticsModule : public Module{
	enum GateLevelExperienceType {
		X00_PILDBNI_D1,	//Description: Gate-Level PI Level Distance Based Neighbor Identification with D=1
		X01_PILDBNI_D2,	//Description: Gate-Level PI Level Distance Based Neighbor Identification with D=2
		X02_PILDBNI_D3,	//Description: Gate-Level PI Level Distance Based Neighbor Identification with D=3
		X10_POLDBNI_D1,	//Description: Gate-Level PO Level Distance Based Neighbor Identification with D=1
		X11_POLDBNI_D2,	//Description: Gate-Level PO Level Distance Based Neighbor Identification with D=2
		X12_POLDBNI_D3,	//Description: Gate-Level PO Level Distance Based Neighbor Identification with D=3
		X20_GDBNI_D1_Directed,	//Description: Gate-Level Gate Distance Based Neighbor Identification in Directed Circuit with D=1
		X21_GDBNI_D2_Directed,	//Description: Gate-Level Gate Distance Based Neighbor Identification in Directed Circuit with D=2
		X22_GDBNI_D3_Directed,	//Description: Gate-Level Gate Distance Based Neighbor Identification in Directed Circuit with D=3
		X30_GDDBNI_D1,	//Description: Gate-Level Gate Distance Based Neighbor Identification in Directed Circuit with D=1
		X31_GDDBNI_D2,		//Description: Gate-Level Gate Distance Based Neighbor Identification in Directed Circuit with D=2
		X32_GDDBNI_D3,		//Description: Gate-Level Gate Distance Based Neighbor Identification in Directed Circuit with D=3
		X40_RAND,				//Description: Randomized Neighbor Identification, It randomly selects a gate as neighbor. We use this for comparison with other experiments
		X50_REF				//Description: Extract Gates from Layout. for comparison only.
	};
public:
    StatisticsModule(Core*, Configuration*, LoggingServices*);
    virtual void run();
    virtual void init();
    virtual void execute();
    virtual void shutdown();
    void GateLevelNeighborIdentificationEngine(GateLevelExperienceType xpType);
    void extractStatisticalInformationForNeighbors();
    void extractStatisticalInformationForNonNeighbors();
    void extractNeighbor(GateLevelExperienceType xpType);
	void dumpCircuit();
	void learningAdjacencyModelUsingSVM();
};
