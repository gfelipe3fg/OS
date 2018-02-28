#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simulator.h"

int main(int argc, char const *argv[]) {
    Options opts;
    Simulator *sim; 

    opts = argsOptions(argc, argv);

    sim = simInit(opts);
    runSim(sim);
    printSimResult(sim);
    freeSim(sim);

    return 0;
}
