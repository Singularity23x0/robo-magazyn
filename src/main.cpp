#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "simulate.h"

using namespace std;
using json = nlohmann::json;

string readInput(const string &path);
void saveOutput(const string &path, const vector<vector<Move>> &moves, const Configuration &config, const GeneticAlgorithm &alg);

int main(int argc, char const *argv[])
{
    google::InitGoogleLogging(argv[0]);
    fLB::FLAGS_logtostderr = true;

    LOG(INFO) << "Execution started";

    const string dataInputPath = "data/in";
    const string dataOutputPath = "data/out";

    string configName;
    if (argc > 1) {
        configName = string(argv[1]);
    } else {
        configName = "test-case.json";
    }

    string result = readInput(dataInputPath + '/' + configName);

    json configurationJson = json::parse(result);
    Configuration config = configurationJson.get<Configuration>();

    setRobotsAmount(config.robotCount);
    setMagazineSize(config.magazineHeight, config.magazineWidth);

    vector<vector<int>> magazine = config.magazine;
    vector<Position> robotPositions = config.robotPositions;
    vector<Position> robotEndPositions = config.robotEndPositions;
    vector<set<int>> orders = config.orders;

    vector<Solution> population;

    for (int i = 0; i < config.populationCount; i++) {
        try {
            vector<vector<Move>> moves = simulate(magazine, robotPositions.data(), robotEndPositions.data(), orders.data());
            population.push_back(Solution{moves});
        } catch (...) {
            i--;
        }
    }

    GeneticAlgorithm *algorithm = new GeneticAlgorithm(
        magazine,
        population,
        config.mutationsFromSolution);

    algorithm->run(config.generationLimit);
    Solution *solution = algorithm->bestSolution();

    cout << "SOLUTION LENGTH: " << solution->size() << endl;

    LOG(INFO) << "Converting solution to JSON format";
    json j = solution->moves;

    saveOutput(dataOutputPath + '/' + configName, solution->moves, config, *algorithm);

    delete algorithm;
    return 0;
}

string readInput(const string &path)
{
    LOG(INFO) << "Reading configuration from: " + path;
    string content;
    ifstream filestream(path, ios::in);

    if (!filestream.good()) {
        LOG(ERROR) << "Failed to open file " + path;
        return "";
    }

    string line;
    while (!filestream.eof()) {
        getline(filestream, line);
        content.append(line + '\n');
        line.clear();
    }

    filestream.close();
    return content;
}

void saveOutput(const string &path, const vector<vector<Move>> &moves, const Configuration &config, const GeneticAlgorithm &alg)
{
    LOG(INFO) << "Saving solutions to " + path;

    json j;
    j["solution"] = moves;
    j["board"] = config.magazine;
    j["orders"] = config.orders;
    j["best-in-generation"] = alg.generationBestScores;
    j["worst-in-generation"] = alg.generationWorstScores;
    j["median-in-generation"] = alg.generationMedianScores;

    ofstream filestream(path, ios::out);

    if (!filestream.good()) {
        LOG(ERROR) << "Failed to open file " + path;
        return;
    }

    istringstream ss(j.dump());
    string line;

    while (ss >> line) {
        filestream << line;
    }
    filestream.close();
}
