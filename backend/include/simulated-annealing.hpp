#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H


#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <vector>
#include <string>
#include "crow.h" // crow
#include "graph.hpp"

namespace SimulatedAnnealing {
	struct annealResult : public crow::returnable {
		Graph* state;
		int iterations;
		double temp;

		annealResult() : crow::returnable::returnable("application/json") {}

		std::string dump() const override {
			std::string dump = "";

			dump += "{\n";
			dump += "	\"links\": [\n";

			for (int i = 0; i < state->links.size(); i++) {
				dump += "		{\n";
				dump += "			\"start\": {\n";
				dump += "				\"lat\": " + std::to_string(state->links[i]->start->lat) + ",\n";
				dump += "				\"lon\": " + std::to_string(state->links[i]->start->lon) + "\n";
				dump += "			},\n";
				dump += "			\"end\": {\n";
				dump += "				\"lat\": " + std::to_string(state->links[i]->end->lat) + ",\n";
				dump += "				\"lon\": " + std::to_string(state->links[i]->end->lon) + "\n";
				dump += "			}\n";
				dump += "		}";

				if (i != state->links.size() - 1) {
					dump += ",\n";
				}
				else {
					dump += "\n";
				}
			}

			dump += "	],\n";
			dump += "	\"temperature\": " + std::to_string(temp) + ",\n";
			dump += "	\"iterations\": " + std::to_string(iterations) + ",\n";
			dump += "	\"distance\": " + std::to_string(state->distance) + "\n";
			dump += "}\n";

			return dump;
		}
	};

	class SimulatedAnnealingGraph
	{
		public:
			Graph* start;
			double threshold;
			double decrement;
			int maxTemp;
			int iterations;

			SimulatedAnnealingGraph(nlohmann::basic_json<>::value_type& positions, int size, int maxTemp, double decrement, double threshold);
			std::vector<annealResult*> runSimAnneal();
	};
}

#endif