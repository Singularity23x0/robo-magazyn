//
// Created by Admin on 29.05.2022.
//

#include "GeneticAlgorithm.h"
#include <random>

long Solution::size()
{
    return moves[0].size();
}

GeneticAlgorithm::GeneticAlgorithm(vector<Solution> population, int mutationsFromSolution)
{
    this->population = population;
    this->mutationsFromSolution = mutationsFromSolution;
}

void GeneticAlgorithm::run(int generationsAmount)
{
    init();
    for (int i = 0; i < generationsAmount; ++i) {
        newGeneration();
    }
}

void GeneticAlgorithm::init()
{
    topSolution = findBestSolution();
}

Solution GeneticAlgorithm::findBestSolution()
{

}

void GeneticAlgorithm::newGeneration()
{
    mutateSolutions();
    Solution topSolutionContender = findBestSolution();
    if (topSolutionContender.size() < topSolution.size())
        topSolution = topSolutionContender;
    pickNewPopulation();
}

void GeneticAlgorithm::mutateSolutions()
{

}

void GeneticAlgorithm::pickNewPopulation()
{
    int targetSize =  population.size() / (mutationsFromSolution + 1);
    std::vector<long> sizes;
    for (int i = 0; i < population.size(); ++i) {
        sizes.push_back(population[i].size());
    }
    discrete
}
