#include <glog/logging.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "simulate.h"

using namespace std;
using json = nlohmann::json;

string readInput(const string &path);
void saveOutput(const string &path, const vector<vector<Move>> &moves, Configuration &config);

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
        configName = "base-case.json";
    }

    string result = readInput(dataInputPath + '/' + configName);

    json configurationJson = json::parse(result);
    Configuration config = configurationJson.get<Configuration>();

    // LOG(INFO) << "Iterations: " << config.iterations;
    // LOG(INFO) << "Robot count: " << config.robotCount;
    // LOG(INFO) << "Initial positions: ";
    // for (auto &position : config.robotPositions) {
    //     cout << position.row << " " << position.col << '\n';
    // }
    // cout << '\n';

    // LOG(INFO) << "Orders:";
    // for (auto &order : config.orders) {
    //     for (auto &p : order) {
    //         cout << p << " ";
    //     }
    //     cout << '\n';
    // }

    setRobotsAmount(config.robotCount);
    setMagazineSize(config.magazineHeight, config.magazineWidth);

    vector<vector<int>> magazine = config.magazine;
    vector<Position> robotPositions = config.robotPositions;
    vector<Position> robotEndPositions = config.robotEndPositions;
    vector<set<int>> orders = config.orders;
    
    vector<Solution> population;

    for (int i = 0; i < config.iterations; i++) {
        try {
            vector <vector <Move>> moves = simulate(magazine, robotPositions.data(), robotEndPositions.data(), orders.data());
            population.push_back(Solution{moves});
        } catch (...) {
            i--;
        }
    }

    GeneticAlgorithm *algorithm = new GeneticAlgorithm(
        magazine,
        population,
        10);

    algorithm->run(50);
    Solution *solution = algorithm->bestSolution();

    // vector<vector<Move>> solution = simulate(magazine, robotPositions, robotEndPositions, orders);
    // vector<vector<Move>> mutatedSolution = solution;

    // for (int i = 0; i < 5; i++) {
    //     try {
    //         mutatedSolution = mutate(magazine, mutatedSolution);
    //     } catch(...) {
    //         i -= 1;
    //     }
    // }
    // vector<vector<Move>> mutatedSolution = solution;

    cout << "SOLUTION LENGTH: " << solution->size() << endl;

    LOG(INFO) << "Converting solution to JSON format";
    json j = solution->moves;

    saveOutput(dataOutputPath + '/' + configName, solution->moves, config);

    // LOG(INFO) << "Writing solution in JSON format to stdout";
    // std::cout << std::setw(4) << j << std::endl;

    // for(std::size_t i = 0; i < solution.size(); i++)
    // {
    //     for(std::size_t j = 0; j < solution[i].size(); j++)
    //     {
    //         cout<< "(" << solution[i][j].position.row << ", " <<solution[i][j].position.col << ", " <<solution[i][j].action<< ") ";
    //     }
    //     cout << endl;
    // }

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

void saveOutput(const string &path, const vector<vector<Move>> &moves, Configuration &config)
{
    LOG(INFO) << "Saving solutions to " + path;

    json j;
    j["solution"] = moves;
    j["board"] = config.magazine;
    j["orders"] = config.orders;

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
