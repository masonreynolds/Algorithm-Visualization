#ifndef MINIMAX_H
#define MINIMAX_H

#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <climits>
#include <string>
#include <vector>
#include <cmath>
#include "checkers.hpp"

namespace Checkers {
	const char xAxis[8] = { 'H', 'G', 'G', 'E', 'D', 'C', 'B', 'A' };


    struct minimaxResult {
        CheckerBoard* board;
		std::string move;
        int heuristic;
        int states;

        minimaxResult(CheckerBoard* board, int heuristic, int states, std::string move) {
            this->board = board;
            this->heuristic = heuristic;
            this->states = states;
			this->move = move;
        }

        std::string dump() {
            std::string dump = "";

			dump += "{\n";
			dump += "	\"blackPoses\": [\n";

			for (int i = 0; i < board->blackPoses.size(); i++) {
				dump += "		[\n";
                dump += "			" + std::to_string(7 - floor(board->blackPoses[i] / 8)) + ",\n";
                dump += "			" + std::to_string(board->blackPoses[i] % 8) + "\n";
				dump += "		]";

				if (i != board->blackPoses.size() - 1) {
					dump += ",\n";
				}
				else {
					dump += "\n";
				}
			}

			dump += "	],\n";
			dump += "	\"redPoses\": [\n";

            for (int i = 0; i < board->redPoses.size(); i++) {
				dump += "		[\n";
                dump += "			" + std::to_string(7 - floor(board->redPoses[i] / 8)) + ",\n";
                dump += "			" + std::to_string(board->redPoses[i] % 8) + "\n";
				dump += "		]";

				if (i != board->redPoses.size() - 1) {
					dump += ",\n";
				}
				else {
					dump += "\n";
				}
			}

			dump += "	],\n";
			dump += "	\"blackKings\": [\n";

			for (int i = 0; i < board->blackKingPoses.size(); i++) {
				dump += "		[\n";
                dump += "			" + std::to_string(7 - floor(board->blackKingPoses[i] / 8)) + ",\n";
                dump += "			" + std::to_string(board->blackKingPoses[i] % 8) + "\n";
				dump += "		]";

				if (i != board->blackKingPoses.size() - 1) {
					dump += ",\n";
				}
				else {
					dump += "\n";
				}
			}

			dump += "	],\n";
			dump += "	\"redKings\": [\n";

            for (int i = 0; i < board->redKingPoses.size(); i++) {
				dump += "		[\n";
                dump += "			" + std::to_string(7 - floor(board->redKingPoses[i] / 8)) + ",\n";
                dump += "			" + std::to_string(board->redKingPoses[i] % 8) + "\n";
				dump += "		]";

				if (i != board->redKingPoses.size() - 1) {
					dump += ",\n";
				}
				else {
					dump += "\n";
				}
			}

			dump += "	],\n";
			dump += "	\"move\": \"" + move + "\",\n";
			dump += "	\"heuristic\": " + std::to_string(heuristic) + ",\n";
			dump += "	\"currentHeuristic\": " + std::to_string(board->heuristic) + ",\n";
			dump += "	\"states\": " + std::to_string(states) + "\n";
			dump += "}\n";

			return dump;
        }
    };


    class Minimax {
        public: 
            CheckerBoard* start;
            int states;

            Minimax(CheckerBoard* start);

            minimaxResult* setupMinimax(int depth);
            int runMinimax(CheckerBoard* current, int AB, int depth);
    };
}

#endif