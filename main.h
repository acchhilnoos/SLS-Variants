#include <iostream>

int   getFitness  (uint16_t state);
uint16_t** selectFromPopulation(uint16_t** population, int* fitnesses, int sumFitnesses);
int*  crossover(uint16_t** population, uint16_t** parents);
int*  mutate(uint16_t** population);
void  iterate  (uint16_t** population);
void  printChildGen  (int gen, uint16_t** population, int* crossovers=NULL, int* mutations=NULL);
void  printParentGen (int gen, uint16_t** population, int sumFitnesses);
void  printGeneration(int gen, uint16_t** population, bool parent, int sumFitnesses, int* crossovers, int* mutations);