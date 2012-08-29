#pragma once

#include "gate.h"
#include "timingextension.h"
#include "layoutextension.h"
#include "probabilityextension.h"

class Cell : public Gate, public TimingExtension, public LayoutExtension, public ProbabilityExtension {
    
};
