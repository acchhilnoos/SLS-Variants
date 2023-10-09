#include "main.h"
#include <iostream>

using namespace std;

/*
 * Determines the fitness score of any given state.
 */
int fitness(int state) {
    char a = (state >> 14) & 3;
    char b = (state >> 12) & 3;
    char c = (state >> 10) & 3;
    char d = (state >>  8) & 3;
    char e = (state >>  6) & 3;
    char f = (state >>  4) & 3;
    char g = (state >>  2) & 3;
    char h = (state >>  0) & 3;

    return 0 + int(h>=a)                    + int(abs(h-c)%2==0) + int(h!=d)   + int(h!=e+2)                    + int(h>g)
             + int(g<a)                     + int(abs(g-c)==1)   + int(g<=d)                        + int(g!=f)
             +             int(abs(f-b)==1) + int(f!=c)          + int(f!=d+1) + int(abs(f-e)%2==1)                        + int(f!=h)
             +                                int(e!=c)          + int(e<(d-1))
             +                                int(d!=c);
}

/*
 * Selects the parents of the next generation.
 */
int** selection(int** population, int* fitnesses, int sumFitnesses) {
    int** newGen = new int*[8];
    for (int i=0; i<8; i++) {
        int r = rand() % sumFitnesses;
        for (int j=0; j<8; j++) {
            if (r<fitnesses[j]) {
                newGen[i] = new int(*population[j]);
                break;
            } else {
                r -= fitnesses[j];
            }
        }
    }
    return newGen;
}

void iterate(int gen, int** population) {
    int** parents;
    int*  fitnesses    = new int[8];
    int   sumFitnesses = 0;
    for (int i=0; i<8; i++) {
        fitnesses[i]  = fitness(*population[i]);
        sumFitnesses += fitnesses[i];
    }
    printChildGen(gen, population, fitnesses, sumFitnesses);

    // Select and print parents from population.
    parents = selection(population, fitnesses, sumFitnesses);
    printParentGen(gen, parents, sumFitnesses);

    for (int i=0; i<8; i++) {
        delete parents[i];
        parents[i] = NULL;
    }
    delete[] parents;
    delete[] fitnesses;
    parents   = NULL;
    fitnesses = NULL;
}

void printChildGen(int gen, int** population, int* fitnesses, int sumFitnesses, int crossovers[8], int mutations [8])
{
    printGeneration(gen, population, false, fitnesses, sumFitnesses, crossovers, mutations);
}

void printParentGen(int gen, int** population, int sumFitnesses)
{
    printGeneration(gen, population, true, NULL, sumFitnesses);
}

void printGeneration(int   gen,
                     int** population,
                     bool  parent,
                     int*  fitnesses,
                     int   sumFitnesses,
                     int   crossovers[8],
                     int   mutations[8])
{
    int* printedFitnesses;
    if (fitnesses!=NULL) { printedFitnesses = fitnesses; }
    else {
        printedFitnesses = new int[8];
        for (int i=0; i<8; i++) {
            printedFitnesses[i]  = fitness(*population[i]);
            sumFitnesses += !parent?printedFitnesses[i]:0;
        }
    }
    
    cout << (parent?"\tParent ":"") << "Generation " << gen << endl;
    for (int i=0; i<8; i++) {
        cout << (parent?"\t":"") << "[";
        for (int j=14; j>=0; j-=2) {
            cout << (((*population[i]>>j)&3)+1) << (j>0?",":"] ");
        }
        cout << "Fitness: " << printedFitnesses[i];
        printf("\t(%.3f%%)\n",(float(printedFitnesses[i])/float(sumFitnesses)));
    }
    if (!fitnesses) { delete[] printedFitnesses; }
    printedFitnesses = NULL;
}

int main(int argc, char const *argv[])
{
    srand((unsigned) time(NULL));
    int** population = new int*[8];

    population[0] = new int(0b0000000000000000);  // [1,1,1,1,1,1,1,1]
    population[1] = new int(0b0101010101010101);  // [2,2,2,2,2,2,2,2]
    population[2] = new int(0b1010101010101010);  // [3,3,3,3,3,3,3,3]
    population[3] = new int(0b1111111111111111);  // [4,4,4,4,4,4,4,4]
    population[4] = new int(0b0001101100011011);  // [1,2,3,4,1,2,3,4]
    population[5] = new int(0b1110010011100100);  // [4,3,2,1,4,3,2,1]
    population[6] = new int(0b0001000100010001);  // [1,2,1,2,1,2,1,2]
    population[7] = new int(0b1011101110111011);  // [3,4,3,4,3,4,3,4]

    for (int i=0; i<5; i++) {
        iterate(i, population);
    }

    for (int i=0; i<8; i++) {
        delete population[i];
        population[i] = NULL;
    }
    delete[] population;
    population = NULL;
}
