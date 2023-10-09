#include <iostream>

int   fitness  (int state);
int** selection(int** population, int* fitnesses, int sumFitnesses);
int*  crossover(int** population, int** parents);
int*  mutate(int** population);
void  iterate  (int** population);
void  printChildGen  (int gen, int** population, int* crossovers=NULL, int* mutations=NULL);
void  printParentGen (int gen, int** population, int sumFitnesses);
void  printGeneration(int gen, int** population, bool parent, int sumFitnesses, int* crossovers=NULL, int* mutations=NULL);