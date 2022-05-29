//
// Created by Admin on 29.05.2022.
//

#ifndef ROBO_MAGAZYN_GENETICALGORITHM_H
#define ROBO_MAGAZYN_GENETICALGORITHM_H

#include "simulate.h"
#include <vector>

struct Solution {
    vector<vector<Move>> moves;
    long size();
};


class GeneticAlgorithm {
private:
   Solution topSolution;
   vector<Solution> population;
   int mutationsFromSolution;
   void init();
   Solution findBestSolution();
   void newGeneration();
   void mutateSolutions();
   void pickNewPopulation();

public:
    GeneticAlgorithm(vector<Solution> population, int mutationsFromSolution);
    void run(int generationsAmount);

};


#endif//ROBO_MAGAZYN_GENETICALGORITHM_H
