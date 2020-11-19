#include <iostream>
#include <cstdlib>
#include <ctime>

#include "simClasses.h"

int main() {
    srand(time(NULL));
    Simulation mainSim(20, 40, 100, 5, 4, 20, 100);
    mainSim.run();
}