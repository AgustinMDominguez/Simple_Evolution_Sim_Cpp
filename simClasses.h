#if !defined(_SIMCLASSES_H)
#define _SIMCLASSES_H
#include <cstdlib>
#include "simConstants.h"

enum Direction {Right, Righdown, Down, Downleft, Left, Leftup, Up, Upright};

/* A gene is number between 1 and GENE_MAX_VAL. This class includes ways to merge them */
class Gene {
private:
    Int value;
    /* Returns a random value between 1 and GENE_MAX_VAL */
    inline Int randomGeneVal() { return (rand()%GENE_MAX_VAL) + 1; }

    /* Performs a roll that returns true with a small probability */
    inline bool radicalMutationRoll() { return (rand()%RADICAL_MUT_FACTOR==0 ? true : false); }

public:
    /* Gene() creates an non-initiated Gene
    Gene(true) generates a random value */
    Gene(bool setvalue=false) { if (setvalue) { value = randomGeneVal(); } }

    Gene(Int v) : value(v) {}

    /* Returns the value of the gene */
    Int geneval() { return value; }

    /* Initiates the value. With no argument it sets a random one*/
    void setvalue(Int val=-1) { value = val==-1 ? randomGeneVal() : val; }

    /* Generates an offpring of two genes*/
    Gene operator+(const Gene&);
};

/* Organism:-:
    adultage:       Amount of cicles that must pass before an organism can reproduce
    reprEneFactor:  The bigger this value the more energy the organism will need before it will be willing to reproduce
    size:           Size affects who wins when two organisms want the same piece of food 
                    but also how much energy each step takes
    sight:          Sight affects how far the organism can look to search for food/mate 
                    but also how much energy each step takes
    offspringratio: Percentage of energy that the child is given by that parent upon birth */
class Organism {
private:
    //Genes: go from 1 to GENE_MAX_VAL
    Gene adulthoodGene;
    Gene reprEnerGene;
    Gene sight;
    Gene size;
    Gene offspringRatio;

    // Specific values set by the given genes that are initiated at the beggining of the organism's life and CANT be changed
    Int moveCost;
    Int matingEnergyThreshold;
    Int offpringEnergy;
    Int adultage;

    // General information about the organism that is updated throughout its life.
    Int age;
    Int generation;
    Int children;
    Int energy;
    bool activeMatingSearch;

    /* Initialices moveCost, matingEnergyThreshold, and others depend of the Genes of the organism */
    void setInternalvalues();

    /* Given the current energy and urges and age, sets if the organism will seek reproduction on the next step */
    void updateMatingUrges();

    /* Returns how much energy this organism would give to its offspring given its current energy */
    inline Int offspringEnergy() {
        return static_cast<float>(energy)*(static_cast<float>(offspringRatio.geneval())/1000.0f);
    }

public:
    Organism(Int baseenergy, bool generaterandom=false);

    /* Modifies the Organism as if it just reproduced.
    NOTE: Call AFTER calling baby=org1+org2 */
    void reproduce();

    /* Modifies the Organisms as if it just moved one step.
    NOTE: Call after calling setMoveCost() for the organism */
    void move();

    /* Modifies the Organisms as if it just ate. */
    void eat(Int);

    /* Modifies the Organisms as if it just spend time doing nothing. */
    void wait();

    /* Returns the offspring of the two organisms */
    Organism operator + (Organism& org);  
    
    /* Shows all details of the organism */
    void showOrganism();

    /* Returns a char that represents the organism */
    char reprChar();
};

/* Full public Class that represents the cells of the map */
class Cell {
public:
    bool food;
    Int foodturn;
    Organism* occupied[3];
    Direction dir[3];
    Cell();
    char cellshow();
};

class Enviroment {
protected:
    Int xsize;
    Int ysize;
    Cell** map;
public:
    Enviroment(Int xs, Int ys);

    /* Creates an un-initialized enviroment, size=0 */
    Enviroment();

    /* Allocates memory */
    void initialize(Int xs, Int ys);

    /* Selects [amount] of random cells of the enviroment and puts food there */
    void sprayFood(Int amount);

    /* Selects a random free place of them map and places the organism there */
    void addOrganism(Organism*);

    /* Creates a new organism if attempt works.
    PRE: map[x][y] has two adult organisms
    POST: If successful attempt, third organism will be on cell*/
    void reproductionAttempt(Int x, Int y);

    /* Simulates a fight and gives the winner the food
    PRE: map[x][y] has two organisms and food */
    void foodScramble(Int x, Int y);

    void showFoodCells();

    void printMap();

    ~Enviroment();
};

class Simulation : private Enviroment {
private:
    Int foodperiod;
    Int foodamount;
public:
    Simulation(Int xs, Int ys, Int initFood, Int foodp, Int fooda, Int initOrganisms, Int baseEnergy);

    void step();

    void run(Int cycles=-1);
};

#endif