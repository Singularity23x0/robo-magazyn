#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdlib>

#include "simulate.h"

using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

string readInput(const fs::path &path);
void saveOutput(const fs::path &path, const vector<vector<Move>> &moves, const Configuration &config, const GeneticAlgorithm &alg);

int main(int argc, char const *argv[])
{
    google::InitGoogleLogging(argv[0]);
    fLB::FLAGS_logtostderr = true;

    LOG(INFO) << "Execution started";

		const fs::path inputDirPath = "data/in";
		const fs::path outputDirPath = "data/out";

		if (!fs::is_directory(inputDirPath)) {
			LOG(FATAL) << "Data directory under " << inputDirPath << " does not exist";
			std::exit(EXIT_FAILURE);
		}

		if (!fs::is_directory(outputDirPath)) {
			LOG(WARNING) << "Output directory under " << outputDirPath << " does not exist. Creating...";

			if (fs::create_directories(outputDirPath)) {
				LOG(INFO) << outputDirPath << " directory created";
			} else {
				LOG(FATAL) << "Failed to create directory for output files!";
				std::exit(EXIT_FAILURE);
			}
		}

		const fs::path configPath = inputDirPath / (argc > 1 ? std::string(argv[1]) : "base-case.json");
		const fs::path dataOutputPath = outputDirPath / (argc > 1 ? std::string(argv[1]) : "base-case.json");

		if (!fs::is_regular_file(configPath)) {
			LOG(FATAL) << configPath << " path does not point to a regular file where config was expected!";
			std::exit(EXIT_FAILURE);
		} else {
			LOG(INFO) << "Config found: " << configPath;
		}

    string result = readInput(configPath);

    json configurationJson = json::parse(result);
    Configuration config = configurationJson.get<Configuration>();

    setRobotsAmount(config.robotCount);
    setMagazineSize(config.magazineHeight, config.magazineWidth);

    vector<vector<int>> magazine = config.magazine;
    vector<Position> robotPositions = config.robotPositions;
    vector<Position> robotEndPositions = config.robotEndPositions;
    vector<set<int>> orders = config.orders;

    vector<Solution> population;

    for (int i = 0; i < config.initialPopulationSize; i++) {
        try {
            vector<vector<Move>> moves = simulate(magazine, robotPositions.data(), robotEndPositions.data(), orders.data());
            population.push_back(Solution{moves});
        } catch (...) {
            i--;
        }
    }

		std::unique_ptr<GeneticAlgorithm> algorithm = 
				std::make_unique<GeneticAlgorithm>(magazine, population, config.mutationsFromSolution);

    algorithm->run(config.generationLimit);

		Solution solution = std::move(algorithm->bestSolution());

    cout << "SOLUTION LENGTH: " << solution.size() << endl;

    saveOutput(dataOutputPath, solution.moves, config, *algorithm);

    return 0;
}

string readInput(const fs::path &path)
{
    LOG(INFO) << "Reading configuration from: " << path;
    string content;
    ifstream filestream(path, ios::in);

    if (!filestream.good()) {
        LOG(ERROR) << "Failed to open file " << path;
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

void saveOutput(const fs::path &path, 
								const vector<vector<Move>> &moves, 
								const Configuration &config, 
								const GeneticAlgorithm &alg)
{
    LOG(INFO) << "Saving solutions to " << path;

    json j;
    j["solution"] = moves;
    j["board"] = config.magazine;
    j["orders"] = config.orders;
		j["best-in-generation"] = alg.generationBestScores;

    ofstream filestream(path, ios::out);

    if (!filestream.good()) {
        LOG(ERROR) << "Failed to open file " << path;
        return;
    }

    istringstream ss(j.dump());
    string line;

    while (ss >> line) {
        filestream << line;
    }
    filestream.close();
}
