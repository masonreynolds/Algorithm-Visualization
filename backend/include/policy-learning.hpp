#ifndef POLICY_LEARNING_H
#define POLICY_LEARNING_H


#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <chrono>
#include <thread>
#include "unistd.h"
#include "crow_all.h" // crow

namespace PolicyLearning {
	class PolicyLearningEnvironment
	{
		public:
			std::array<int, 2> agent;
			std::array<int, 2> goal;
			std::vector<std::array<int, 2>> blocks;
			std::vector<std::vector<double>> rewards;

			PolicyLearningEnvironment(nlohmann::basic_json<>::value_type& environment);
			void runPolicyLearning(crow::websocket::connection& conn);
			std::vector<std::array<int, 2>> getAgentNeighbors(std::map<std::string, bool> exploredCells);
			std::string dump() {	
				std::string dump = "";

				dump += "{\n";
				dump += "	\"environment\": {\n";
				dump += "		\"agent\": [" + std::to_string(agent[0]) + ", " + std::to_string(agent[1]) + "],\n";
				dump += "		\"rewards\": [";
				for (int i = 0; i < rewards.size(); i++) {
					dump += "[";
					for (int k = 0; k < rewards.size(); k++) {
						dump += std::to_string(rewards[i][k]);

						if (k != rewards.size() - 1) {
							dump += ",";
						}
						else {
							dump += "]";
						}
					}

					if (i != rewards.size() - 1) {
						dump += ",";
					}
					else {
						dump += "]\n";
					}
				}

				dump += "	}\n";
				dump += "}\n";

				return dump;
			}
	};
}

#endif