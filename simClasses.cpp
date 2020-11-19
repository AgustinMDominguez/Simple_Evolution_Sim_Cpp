#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

#include "simClasses.h"
#include "simConstants.h"

/*///////////////////////////////////////////////////
 ██████  ███████ ███    ██ ███████ 
██       ██      ████   ██ ██      
██   ███ █████   ██ ██  ██ █████   
██    ██ ██      ██  ██ ██ ██      
 ██████  ███████ ██   ████ ███████ 
///////////////////////////////////////////////////*/

Gene Gene::operator+(const Gene& b) {
    if(radicalMutationRoll()) {
        return Gene(true);
    } else {
        Int aux = rand();
        Int mutation = aux % GENE_S_MUTATION;
        mutation *= (aux%2 == 0) ? 1 : -1;
        aux = (aux>>5)%100;
        mutation += (static_cast<float>(aux)/100.0f) * value + (1 - (static_cast<float>(aux)/100.0f)) * b.value;
        mutation = mutation>GENE_MAX_VAL ? GENE_MAX_VAL : mutation;
        return Gene(abs(mutation));
    }
}

/*///////////////////////////////////////////////////
 ██████  ██████   ██████   █████  ███    ██ ██ ███████ ███    ███ 
██    ██ ██   ██ ██       ██   ██ ████   ██ ██ ██      ████  ████ 
██    ██ ██████  ██   ███ ███████ ██ ██  ██ ██ ███████ ██ ████ ██ 
██    ██ ██   ██ ██    ██ ██   ██ ██  ██ ██ ██      ██ ██  ██  ██ 
 ██████  ██   ██  ██████  ██   ██ ██   ████ ██ ███████ ██      ██ 
///////////////////////////////////////////////////*/

Organism::Organism(Int baseenergy, bool generaterandom/*=false*/)
        : age(0), generation(0), children(0), energy(baseenergy) {
    if (generaterandom) {
        adulthoodGene.setvalue();
        reprEnerGene.setvalue();
        size.setvalue();
        sight.setvalue();
        offspringRatio.setvalue();
    }
}

void Organism::setInternalvalues() {
    adultage = (MAX_ORGANISM_AGE*adulthoodGene.geneval())/GENE_MAX_VAL;
    if (adultage<MIN_ORGANISM_ADULTHOOD) {adultage = MIN_ORGANISM_ADULTHOOD;}
    moveCost = (MAX_SIZE_MOVE_COST*size.geneval())/(Int)GENE_MAX_VAL;
    if (moveCost<1) {moveCost = 1;}
    matingEnergyThreshold = moveCost*MAX_REPR_MOVE_STOCKPILE*reprEnerGene.geneval()/GENE_MAX_VAL;
    offpringEnergy = matingEnergyThreshold*offspringRatio.geneval()/GENE_MAX_VAL;
}

void Organism::updateMatingUrges() {
    if(activeMatingSearch) {
        if (energy < matingEnergyThreshold) { activeMatingSearch = false; }
    } else if (energy>matingEnergyThreshold && age>adultage) { activeMatingSearch = true; }
}

void Organism::reproduce() {
    energy -= offpringEnergy;
    children++;
    age++;
    updateMatingUrges();
}

void Organism::move() {
    energy -= moveCost;
    age++;
    updateMatingUrges();
}

void Organism::eat(Int e=DEFAULT_FOOD_AMOUNT) {
    energy+= e;
    age++;
    updateMatingUrges();
}

void Organism::wait() {
    age++;
}

Organism Organism::operator + (Organism& org) {
    Organism baby(offspringEnergy() + org.offspringEnergy());
    baby.generation = (generation > org.generation) ? generation+1 : org.generation+1;
    baby.adultage = adultage + org.adultage;
    baby.reprEnerGene = reprEnerGene + org.reprEnerGene;
    baby.size = size + org.size;
    baby.sight = sight + org.sight;
    baby.offspringRatio = offspringRatio + org.offspringRatio;
    baby.setInternalvalues();
    return baby;
}

void Organism::showOrganism() {
    std::cout << "age: "            << age                      << "\t";
    std::cout << "generation: "     << generation               << "\t";
    std::cout << "children: "       << children                 << "\n";
    std::cout << "energy: "         << energy                   << "\n";
    std::cout << "adultage: "       << adulthoodGene.geneval()       << "\t";
    std::cout << "size: "           << size.geneval()           << "\t";
    std::cout << "offspringRatio: " << offspringRatio.geneval() << "\n" << std::endl;
}

char Organism::reprChar() {
    return 'Y';
}

/*///////////////////////////////////////////////////
 ██████ ███████ ██      ██      
██      ██      ██      ██      
██      █████   ██      ██      
██      ██      ██      ██      
 ██████ ███████ ███████ ███████
///////////////////////////////////////////////////*/
Cell::Cell() : food(false), foodturn(FOOD_TURN_AMOUNT) {
    occupied[0] = nullptr;
    occupied[1] = nullptr;
    occupied[2] = nullptr;
}

char Cell::cellshow() {
    if(occupied[0]==nullptr) {
        return (food ? '~' : ' ');
    } else if(occupied[2]!=nullptr) {
            return ('3');
    } else if (occupied[1]!=nullptr) {
        return (food ? '!' : '2');
    } else {
        return (*occupied[0]).reprChar();
    }
}

/*///////////////////////////////////////////////////
███████ ███    ██ ██    ██ ██ ██████   ██████  ███    ███ ███████ ███    ██ ████████ 
██      ████   ██ ██    ██ ██ ██   ██ ██    ██ ████  ████ ██      ████   ██    ██    
█████   ██ ██  ██ ██    ██ ██ ██████  ██    ██ ██ ████ ██ █████   ██ ██  ██    ██    
██      ██  ██ ██  ██  ██  ██ ██   ██ ██    ██ ██  ██  ██ ██      ██  ██ ██    ██    
███████ ██   ████   ████   ██ ██   ██  ██████  ██      ██ ███████ ██   ████    ██
///////////////////////////////////////////////////*/
Enviroment::Enviroment() : xsize(0), ysize(0), map(nullptr) {}

Enviroment::Enviroment(Int xs, Int ys) : xsize(xs), ysize(ys) {
    map = new Cell*[xsize];
    for (Int i = 0; i < xsize; i++) { map[i] = new Cell[ysize]; }
}

void Enviroment::initialize(Int xs, Int ys) {
    xsize = xs;
    ysize = ys;
    map = new Cell*[xsize];
    for (Int i = 0; i < xsize; i++) { map[i] = new Cell[ysize]; }
}

void Enviroment::showFoodCells() {
    std::cout << "Foods:  ";
    for (Int x=0; x<xsize; x++) {
        for (Int y=0; y<ysize; y++) {
            if(map[x][y].food) {
                std::cout << "[" << x << "," << y << "]" << " ";
            }
        }
    }
    std::cout << std::endl;
}

void Enviroment::printMap() {
    for (Int i=0; i<20; i++) { std::cout << "\n"; }
    std::cout << "|";
    for (Int y=0; y<ysize; y++) { std::cout << "="; }
    std::cout << "|\n|";
    for (Int x=0; x<xsize; x++) {
        for (Int y=0; y<ysize; y++) { std::cout << map[x][y].cellshow(); }
        std::cout << "|\n|";
    }
    for (Int y=0; y<ysize; y++) { std::cout << "="; }
    std::cout << "|" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
}

Enviroment::~Enviroment() {
    for (Int i = 0; i < xsize; i++) { delete[] map[i]; }
    delete[] map;
}

void Enviroment::addOrganism(Organism* org) {
    Int r, x, y;
    Int counter = 0;
    while(counter < xsize*ysize) {
        r = rand();
        for (Int i = 0; i<6; i++) {
            x = r%xsize;
            r = r>>2;
            y = r%ysize;
            r = r>>2;
            if (map[x][y].occupied[0]==nullptr) {
                map[x][y].occupied[0] = org;
                return;
            }
        }
    }
    std::cout << "Failed to add organism to Map.\tOrganism:";
    org->showOrganism();
}

void Enviroment::sprayFood(Int amount) {
    Int count = 0;
    Int randomgen, xcoord, ycoord;
    while(count < amount) {
        randomgen = rand();
        for (Int i=0; i<4; i++) {
            xcoord = randomgen%xsize;            
            randomgen = randomgen>>3;
            ycoord = randomgen%ysize;
            randomgen = randomgen>>3;
            map[xcoord][ycoord].food = true;
            count++;
            if(count>amount) { break; }
        }

    }
}
/*
void Enviroment::reproductionAttempt(Int x, Int y) {

}

void Enviroment::foodScramble(Int x, Int y) {

}*/

/*///////////////////////////////////////////////////
███████ ██ ███    ███ ██    ██ ██       █████  ████████ ██  ██████  ███    ██ 
██      ██ ████  ████ ██    ██ ██      ██   ██    ██    ██ ██    ██ ████   ██ 
███████ ██ ██ ████ ██ ██    ██ ██      ███████    ██    ██ ██    ██ ██ ██  ██ 
     ██ ██ ██  ██  ██ ██    ██ ██      ██   ██    ██    ██ ██    ██ ██  ██ ██ 
███████ ██ ██      ██  ██████  ███████ ██   ██    ██    ██  ██████  ██   ████ 
///////////////////////////////////////////////////*/

Simulation::Simulation(Int xs, Int ys, Int initFood, Int foodp, Int fooda, Int initOrganisms, Int baseEnergy)
          : Enviroment(xs, ys), foodperiod(foodp), foodamount(fooda) {
    sprayFood(initFood);
    for(Int i=0; i<initOrganisms; i++) {
        addOrganism(new Organism(baseEnergy, true));
    }
}

void Simulation::step() {
    for (Int x=0; x<xsize; x++) {
        for (Int y=0; y<ysize; y++) {

        }
    }
}

void Simulation::run(Int cycles/*=-1*/) {
    if (cycles==-1) {
        while(true) {
            step();
            printMap();
        }
    } else {
        while(cycles>0) {
            step();
            printMap();
            cycles--;
        }
    }
}
