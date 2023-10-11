#include <iostream>
#include <iomanip>

using namespace std;

// Global population variable.
uint32_t *population;

// Gets a specific parent individual from population.
uint16_t getParent(int index) {
    return uint16_t(population[index]>>16);
}
// Gets a specific child from population.
uint16_t getChild(int index) {
    return uint16_t(population[index]);
}
// Gets all parents from population.
uint16_t* getParents() {
    uint16_t* parents = new uint16_t[8];
    for (int i=0; i<8; i++) { parents[i] = getParent(i); }
    return parents;
}
// Gets all children from population.
uint16_t* getChildren() {
    uint16_t* children = new uint16_t[8];
    for (int i=0; i<8; i++) { children[i] = getChild(i); }
    return children;
}

// Determines the fitness score of an individual.
int getFitness(int index, bool parent) {
    uint16_t individual = parent ? getParent(index) : getChild(index);
    int a = ((individual>>14)&3)+1;
    int b = ((individual>>12)&3)+1;
    int c = ((individual>>10)&3)+1;
    int d = ((individual>> 8)&3)+1;
    int e = ((individual>> 6)&3)+1;
    int f = ((individual>> 4)&3)+1;
    int g = ((individual>> 2)&3)+1;
    int h = ((individual>> 0)&3)+1;

    return 0 + int(h>=a) + int(abs(h-c)%2==0) + int(h!=d) + int(h!=e+2) + int(h>g)
             + int(g<a) + int(abs(g-c)==1) + int(g<=d) + int(g!=f)
             + int(abs(f-b)==1) + int(f!=c) + int(f!=d+1) + int(abs(f-e)%2==1) + int(f!=h)
             + int(e!=c) + int(e<(d-1))
             + int(d!=c);
}
// Gets the fitness scores for all parents or all children.
int* getFitnesses(bool parents) {
    int* fitnesses = new int[8];
    for (int i=0; i<8; i++) {
        fitnesses[i] = getFitness(i, parents);
    }
    return fitnesses;
}

// Prints one individual.
void printIndividual(int index, bool parent, int* crossovers, int*mutations) {
    uint16_t individual = parent ? getParent(index) : getChild(index);
    cout << "[" << (parent?" ":"");
    {
        for (int i=14; i>=0; i-=2) {
        if (parent) {
            cout << ((individual>>i)&3)+1
                 << (i==0?" ]":((crossovers&&7-i/2==crossovers[index])?"|":" "));
        } else {
            if (mutations) {
                cout << ((7-i/2==mutations[index]||6-i/2==mutations[index])?"~":" ")
                     << ((individual>>i)&3)+1
                     << (i==0?(7-i/2==mutations[index]?"~]":" ]"):"");
            } else {
                cout << " " << ((individual>>i)&3)+1 << (i==0?" ]":"");
            }
        }
    }
    }
}
// Prints all individuals with the ability to specify parents, children, none, or both.
void printPopulation(int gen, bool includeParents, bool includeChildren, int* crossovers, int* mutations) {
    uint16_t* parents           = getParents();
    uint16_t* children          = getChildren();
    int*      parentFitnesses   = getFitnesses(true);
    int*      childFitnesses    = getFitnesses(false);
    int       sumChildFitnesses = 0;
    for (int i=0; i<8; i++) { sumChildFitnesses += childFitnesses[i]; }

    cout << "\nGeneration " << gen << endl;
    for (int i=-1; i<8; i++) {
        if (includeParents) {
            if (i==-1) { cout << left << setw(20) << "Parents:" << setw(12) << "(f)"; }
            else {
            printIndividual(i, true, crossovers, mutations);
            cout << right << setw(3) << parentFitnesses[i] << "\t\t";
            }
        }
        if (includeChildren) {
            if (i==-1) { cout << left << setw(20) << "Children:" << setw(9) << "(f)" << "(p)"; }
            else {
            printIndividual(i, false, crossovers, mutations);
            cout << right << setw(3) << childFitnesses[i];
            printf("\t(%.3f%%)", float(childFitnesses[i])/float(sumChildFitnesses));
            }
        }
        cout << endl;
    }

    delete[] parents;
    delete[] children;
    delete[] parentFitnesses;
    delete[] childFitnesses;
}

void selectionStage() {
    int* fitnesses   = getFitnesses(false);
    int sumFitnesses = 0;
    for (int i=0; i<8; i++) { sumFitnesses += fitnesses[i]; }

    for (int i=0; i<8; i++) {
        int r = rand()%sumFitnesses;
        for (int j=0; j<8; j++) {
            if (r<fitnesses[j]) {
                population[i] = (population[i]&((1<<16)-1)) + (population[j]<<16);
                if (i%2&&getParent(i)==getParent(i-1)) { i--; }
                break;
            } else {
                r -= fitnesses[j];
            }
        }
    }
    delete[] fitnesses;
}

int* crossoverStage() {
    int* crossovers = new int[8];
    for (int i=0; i<8; i+=2) {
        int r = rand()%7;
        crossovers[i]   = r;
        crossovers[i+1] = r;
        uint32_t clearing = ~((1<<(((6-r)<<1)+2))-1);
        uint32_t reading  = (~clearing)<<16;
        uint32_t storing  = population[i];
        population[i]   = (population[i]&~((1<<16)-1))   + (population[i]>>16);
        population[i+1] = (population[i+1]&~((1<<16)-1)) + (population[i+1]>>16);
        population[i]   = (clearing&population[i])       + ((reading&population[i+1])>>16);
        population[i+1] = (clearing&population[i+1])     + ((reading&storing)>>16);
    }
    return crossovers;
}

int* mutationStage() {
    int* mutations = new int[8];
    for (int i=0; i<8; i++) {
        int r = rand();
        if (r%10<3) {
            r = r%16;
            mutations[i] = r/2;
            if ((population[i]>>(15-r))&1) {
                population[i] = population[i]&~(1<<(15-r));
            } else {
                population[i] += 1<<(15-r);
            }
        } else {
            mutations[i] = -2;
        }
    }
    return mutations;
}

void iterate(int gen) {
    selectionStage();
    int* crossovers = crossoverStage();
    int* mutations  = mutationStage();
    printPopulation(gen, true, true, crossovers, mutations);
    delete[] crossovers;
    delete[] mutations;
}

int main(int argc, char const *argv[])
{
    srand((unsigned) time(nullptr));
    /* 
     * NOTE: population stores 8 parents and 8 children
     * in 8 32-bit unsigned integers. Parents are the first
     * 16 bits, and children are the remaining 16 bits. Each
     * individual's variables take 2 bits; 00 represents 1,
     * 01 represents 2, 10 represents 3, and 11 represents 4.
     * Variables are in alphabetical order, i.e., AABBCCDDEEFFGGHH.
     */
    // Initialize children to their pre-determined values.
    population = new uint32_t[8]{0b0000000000000000,
                                 0b0101010101010101,
                                 0b1010101010101010,
                                 0b1111111111111111,
                                 0b0001101100011011,
                                 0b1110010011100100,
                                 0b0001000100010001,
                                 0b1011101110111011};

    printPopulation(0, false, true, nullptr, nullptr);

    for (int i=1; i<=5; i++) { iterate(i); }

    delete[] population;
    return 0;
}
