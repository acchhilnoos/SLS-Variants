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

    return 0 + int(h>=a) + int(abs(h-c)%2==0) + int(h!=d) + int(h!=e+2) + int(h>g)
             + int(g<a) + int(abs(g-c)==1) + int(g<=d) + int(g!=f)
             + int(abs(f-b)==1) + int(f!=c) + int(f!=d+1) + int(abs(f-e)%2==1) + int(f!=h)
             + int(e!=c) + int(e<(d-1))
             + int(d!=c);
}

/*
 * Selects the parents of the next generation.
 */
int** selection(int** population, int* fitnesses, int sumFitnesses) {
    int** newParents = new int*[8];
    // Weighted random selection from population.
    for (int i=0; i<8; i++) {
        int r = rand() % sumFitnesses;
        for (int j=0; j<8; j++) {
            if (r<fitnesses[j]) {
                newParents[i] = new int(*population[j]);
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
int* crossover(int** population, int** parents) {
    int* crossovers = new int[8];
    for (int i=0; i<8; i+=2) {
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
int* mutate(int** population) {
    int* mutations = new int[8];
    for (int i=0; i<8; i++) {
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
             * Note also that m=n changes the variable to n+1.
             */
            *population[i] = (~(3<<ci) & *population[i]) + (m<<ci);
        } else {
            mutations[i] = -1;
        }
    }
    return mutations;
}

void iterate(int gen, int** population) {
    int** parents;
    int*  fitnesses    = new int[8];
    int   sumFitnesses = 0;
    int*  crossovers;
    int*  mutations;
    for (int i=0; i<8; i++) {
        fitnesses[i]  = fitness(*population[i]);
        sumFitnesses += fitnesses[i];
    }
    /*
     * NOTE: PLEASE DO NOT REMOVE THE PRINT FUNCTIONS HERE,
     * THEY CONTAIN THE MATCHING DELETE[]s FOR CROSSOVERS
     * AND MUTATIONS.
     */
    parents = selection(population, fitnesses, sumFitnesses);
    printParentGen(gen, parents, sumFitnesses);

    crossovers = crossover(population, parents);
    mutations  = mutate(population);
    printChildGen(gen, population, crossovers, mutations);


    for (int i=0; i<8; i++) {
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
void printChildGen(int gen, int** population, int* crossovers, int* mutations)
{
    printGeneration(gen, population, false, 0, crossovers, mutations);
}

/*
 * Wrapper function for printing parent generations.
 */
void printParentGen(int gen, int** population, int sumFitnesses)
{
    printGeneration(gen, population, true, sumFitnesses);
}

/*
 * Prints a population.
 */
void printGeneration(int   gen,
                     int** population,
                     bool  parent,
                     int   sumFitnesses,
                     int*  crossovers,
                     int*  mutations)
{
    int* fitnesses = new int[8];
    for (int i=0; i<8; i++) {
        fitnesses[i]  = fitness(*population[i]);
        sumFitnesses += !parent?fitnesses[i]:0;
    }
    if (!crossovers) {
        crossovers = new int[8] {-1, -1, -1, -1, -1, -1, -1, -1 };
    }
    if (!mutations) {
        mutations = new int[8] {-1, -1, -1, -1, -1, -1, -1, -1 };
    }

    cout << (parent?"\t\t   Parent Generation ":"\t\t      Generation ") << gen << endl;
    for (int i=0; i<8; i++) {
        if (parent) {
            cout << (i%2?" ":"\t  ") << "[";
            for (int j=14; j>=0; j-=2) {
                cout << (((*population[i]>>j)&3)+1) << (j>0?",":"]");
            }
            cout << (i%2?"\n":", ");
        } else {
            cout << "[";
            for (int j=14; j>=0; j-=2) {
                cout << (j==mutations[i]?"(":" ")
                     << (((*population[i]>>j)&3)+1)
                     << (j==mutations[i]?")":" ")
                     << (j>0?"":"] ") << (j==crossovers[i]?"|":" ");
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
    srand((unsigned) time(NULL));
    int** population = new int*[8];

    /*
     * NOTE: A state or individual is represented as a string of
     * 16 bits where each variable takes two bits. Because of this
     * restriction, 1 is represented as 00 and 3 as 11.
     */
    population[0] = new int(0b0000000000000000);  // [1,1,1,1,1,1,1,1]
    population[1] = new int(0b0101010101010101);  // [2,2,2,2,2,2,2,2]
    population[2] = new int(0b1010101010101010);  // [3,3,3,3,3,3,3,3]
    population[3] = new int(0b1111111111111111);  // [4,4,4,4,4,4,4,4]
    population[4] = new int(0b0001101100011011);  // [1,2,3,4,1,2,3,4]
    population[5] = new int(0b1110010011100100);  // [4,3,2,1,4,3,2,1]
    population[6] = new int(0b0001000100010001);  // [1,2,1,2,1,2,1,2]
    population[7] = new int(0b1011101110111011);  // [3,4,3,4,3,4,3,4]

    printChildGen(0, population);
    for (int i=1; i<=5; i++) {
        iterate(i, population);
    }

    for (int i=0; i<8; i++) {
        delete population[i];
        population[i] = NULL;
    }
    delete[] population;
    population = NULL;
}
