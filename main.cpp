#include "main.h"
#include <iostream>

using namespace std;

// For playing around with population size.
int populationSizeMultiplier = 1;
int populationSize;

/*
 * Determines the fitness score of any given state.
 */
int getFitness(uint16_t state) {
    char a = ((state >> 14) & 3) + 1;
    char b = ((state >> 12) & 3) + 1;
    char c = ((state >> 10) & 3) + 1;
    char d = ((state >>  8) & 3) + 1;
    char e = ((state >>  6) & 3) + 1;
    char f = ((state >>  4) & 3) + 1;
    char g = ((state >>  2) & 3) + 1;
    char h = ((state >>  0) & 3) + 1;

    return 0 + int(h>=a) + int(abs(h-c)%2==0) + int(h!=d) + int(h!=e+2) + int(h>g)
             + int(g<a) + int(abs(g-c)==1) + int(g<=d) + int(g!=f)
             + int(abs(f-b)==1) + int(f!=c) + int(f!=d+1) + int(abs(f-e)%2==1) + int(f!=h)
             + int(e!=c) + int(e<(d-1))
             + int(d!=c);
}

/*
 * Selects the parents of the next generation.
 */
uint16_t** selectFromPopulation(uint16_t** population, int* fitnesses, int sumFitnesses) {
    uint16_t** newParents = new uint16_t*[populationSize];
    // Weighted random selection from population.
    for (int i=0; i<populationSize; i++) {
        int r = rand() % sumFitnesses;
        for (int j=0; j<populationSize; j++) {
            if (r<fitnesses[j]) {
                newParents[i] = new uint16_t(*population[j]);
                // Prevents "asexual" reproduction.
                if (i%2 && *newParents[i]==*newParents[i-1]) {
                    delete newParents[i];
                    newParents[i] = NULL;
                    i--;
                }
                break;
            } else {
                r -= fitnesses[j];
            }
        }
    }
    return newParents;
}

/* 
 * Performs the "genetic crossover" at a random point.
 */
int* crossover(uint16_t** population, uint16_t** parents) {
    int* crossovers = new int[populationSize];
    for (int i=0; i<populationSize; i+=2) {
        int r  = rand();
        int ci = (r%7+1)*2;
        crossovers [i]   = 16-ci;
        crossovers [i+1] = 16-ci;
        /* 
         * NOTE: (((1<<ci)-1)<<(16-ci) & *parents[i])
         * gives only the first ci bits of *parents[i],
         * (((1<<(16-ci))-1) & *parents[i+1]) gives only
         * the last 16-ci bits of *parents[i+1].
         * */
        *population[i]   = (((1<<ci)-1)<<(16-ci) & *parents[i]) 
                            + (((1<<(16-ci))-1) & *parents[i+1]);
        *population[i+1] = (((1<<ci)-1)<<(16-ci) & *parents[i+1])
                            + (((1<<(16-ci))-1) & *parents[i]);
    }
    return crossovers;
}

/*
 * "Mutates" a random variable in about 30% of the population.
 */
int* mutate(uint16_t** population) {
    int* mutations = new int[populationSize];
    for (int i=0; i<populationSize; i++) {
        int  r  = rand();
        if (r%10<3) {
            int ci = (r%8)*2;
            mutations[i] = ci;
            // Sets o to bits 16-ci-1 and 16-ci-2 of *population[i].
            int o = (*population[i]>>ci) & 3;
            int m = r%3;
            while (m==o) {
                r = rand();
                m = r%3;
            }
            /*
             * NOTE: (~(3<<ci) & *population[i]) replaces
             * bits 16-ci-1 and 16-ci-2 with 00 
             * (i.e., xxxxxxxx00xxxxxx with ci=6). 
             * Adding (m<<ci) changes the specified variable in *population[i]
             * to m (i.e., xxxx10xxxxxxxxxx with ci=10, m=2).
             */
            *population[i] = (~(3<<ci) & *population[i]) + (m<<ci);
        } else {
            mutations[i] = -1;
        }
    }
    return mutations;
}

void iterate(int gen, uint16_t** population) {
    uint16_t** parents;
    int*  fitnesses    = new int[populationSize];
    int   sumFitnesses = 0;
    int*  crossovers;
    int*  mutations;
    for (int i=0; i<populationSize; i++) {
        fitnesses[i]  = getFitness(*population[i]);
        sumFitnesses += fitnesses[i];
    }
    /*
     * NOTE: DO NOT REMOVE THE PRINT FUNCTIONS HERE,
     * THEY CONTAIN THE MATCHING DELETE[]s FOR CROSSOVERS
     * AND MUTATIONS.
     */
    parents = selectFromPopulation(population, fitnesses, sumFitnesses);
    crossovers = crossover(population, parents);
    mutations  = mutate(population);

    printParentGen(gen, parents, sumFitnesses, crossovers);
    printChildGen(gen, population, mutations);

    for (int i=0; i<populationSize; i++) {
        delete parents[i];
        parents[i] = NULL;
    }
    delete[] parents;
    delete[] fitnesses;
    parents   = NULL;
    fitnesses = NULL;
}

/*
 * Wrapper function for printing child generations.
 */
void printChildGen(int gen, uint16_t** population, int* mutations)
{
    printGeneration(gen, population, false, 0, NULL, mutations);
}

/*
 * Wrapper function for printing parent generations.
 */
void printParentGen(int gen, uint16_t** population, int sumFitnesses, int* crossovers)
{
    printGeneration(gen, population, true, sumFitnesses, crossovers, NULL);
}

/*
 * Prints a population.
 */
void printGeneration(int        gen,
                     uint16_t** population,
                     bool       parent,
                     int        sumFitnesses,
                     int*       crossovers,
                     int*       mutations)
{
    int* fitnesses = new int[populationSize];
    for (int i=0; i<populationSize; i++) {
        fitnesses[i]  = getFitness(*population[i]);
        sumFitnesses += !parent?fitnesses[i]:0;
    }
    if (!crossovers) {
        crossovers = new int[populationSize];
        for (int i=0; i<populationSize; i++) { crossovers[i] = -1; }
    }
    if (!mutations) {
        mutations = new int[populationSize];
        for (int i=0; i<populationSize; i++) { mutations[i] = -1; }
    }

    cout << (parent?"\tParent ":"Descendant ") << "Generation " << gen << endl;
    for (int i=0; i<populationSize; i++) {
        if (parent) {
            cout << "\t[ ";
            for (int j=14; j>=0; j-=2) {
                cout << (((*population[i]>>j)&3)+1)
                     << (j==crossovers[i]?"|":" ")
                     << (j>0?"":"]");
            }
            cout << " (" << fitnesses[i] << ")" << endl;
        } else {
            cout << "[";
            for (int j=14; j>=0; j-=2) {
                cout << (j+2==mutations[i]?")":(j==mutations[i]?"(":" "))
                     << (((*population[i]>>j)&3)+1)
                     << (j==0?(j==mutations[i]?")] ":" ] "):"");
            }
            cout << "Fitness: " << fitnesses[i];
            printf("\t(%.3f%%)\n",(float(fitnesses[i])/float(sumFitnesses)));
        }
    }
    delete[] fitnesses;
    delete[] crossovers;
    delete[] mutations;
    fitnesses  = NULL;
    crossovers = NULL;
    mutations  = NULL;
}

int main(int argc, char const *argv[])
{
    if (argc > 1) { populationSizeMultiplier = int(*argv[1])-48; }
    populationSize = 8 * populationSizeMultiplier;

    srand((unsigned) time(NULL));
    uint16_t** population = new uint16_t*[populationSize];

    /*
     * NOTE: A state or individual is represented as a string of
     * 16 bits where each variable takes two bits. Because of this
     * restriction, 1 is represented as 00 and 3 as 11.
     */
    for (int i=0; i<populationSize; i+=8) {
        population[i+0] = new uint16_t(0b0000000000000000);  // [1,1,1,1,1,1,1,1]
        population[i+1] = new uint16_t(0b0101010101010101);  // [2,2,2,2,2,2,2,2]
        population[i+2] = new uint16_t(0b1010101010101010);  // [3,3,3,3,3,3,3,3]
        population[i+3] = new uint16_t(0b1111111111111111);  // [4,4,4,4,4,4,4,4]
        population[i+4] = new uint16_t(0b0001101100011011);  // [1,2,3,4,1,2,3,4]
        population[i+5] = new uint16_t(0b1110010011100100);  // [4,3,2,1,4,3,2,1]
        population[i+6] = new uint16_t(0b0001000100010001);  // [1,2,1,2,1,2,1,2]
        population[i+7] = new uint16_t(0b1011101110111011);  // [3,4,3,4,3,4,3,4]
    }

    printChildGen(0, population);
    for (int i=1; i<=5; i++) {
        iterate(i, population);
    }

    for (int i=0; i<populationSize; i++) {
        delete population[i];
        population[i] = NULL;
    }
    delete[] population;
    population = NULL;
}
