//
// Created by Admin on 29.05.2022.
//

#include "GeneticAlgorithm.h"
#include <random>

random_device dev;
mt19937 rng(dev());

long Solution::size()
{
    return moves[0].size();
}

GeneticAlgorithm::GeneticAlgorithm(vector<vector<int>> &magazine, vector<Solution> population, int mutationsFromSolution)
{
    this->magazine = magazine;
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
    topSolution = population[0];
    findBestSolution();
}

void GeneticAlgorithm::newGeneration()
{
    mutateSolutions();
    findBestSolution();
    pickNewPopulation();
}

void GeneticAlgorithm::mutateSolutions()
{
    int originalSize = population.size();
    for (int i = 0; i < originalSize; i++)
    {
        population.push_back(Solution {mutate(magazine, population[i].moves)});
    }
    
}

void GeneticAlgorithm::findBestSolution()
{
    for (int i = 0; i < population.size(); ++i) {
        if (population[i].size() < topSolution.size())
            topSolution = population[i];
    }
}

void GeneticAlgorithm::pickNewPopulation()
{
    int targetSize =  population.size() / (mutationsFromSolution + 1);
    vector<Solution> newPopulation;
    vector<long> sizes;
    long maxSize = 0;
    vector<int> weights;
    vector<bool> used(population.size(), false);

    for (int i = 0; i < population.size(); ++i) {
        sizes.push_back(population[i].size());
        maxSize = max(maxSize, population[i].size());
    }
    maxSize++;
    for (int i = 0; i < sizes.size(); ++i) {
        weights.push_back(maxSize - sizes[i]);
    }

    discrete_distribution<long> dist {weights.begin(), weights.end()};
    while (newPopulation.size() < targetSize)
    {
        int i = dist(rng);
        if(!used[i]) {
            used[i] = true;
            newPopulation.push_back(population[i]);
        }
    }
    population = newPopulation;
}

Solution *GeneticAlgorithm::bestSolution() {
    return &topSolution;
}
