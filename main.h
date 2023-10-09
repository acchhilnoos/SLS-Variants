#include <iostream>

int   fitness(int state);
int** selection(int** population, int* fitnesses, int sumFitnesses);
void  iterate(int** population);
void  printChildGen  (int gen, int** population, int* fitnesses, int sumFitnesses, int crossovers [8]={}, int mutations [8]={});
void  printParentGen (int gen, int** population, int sumFitnesses);
void  printGeneration(int gen, int** population, bool parent, int* fitnesses, int sumFitnesses, int crossovers[8] = {}, int mutations[8] = {});