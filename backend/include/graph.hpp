#ifndef GRAPH_H
#define GRAPH_H

#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <string>
#include <vector>
#include "position.hpp"
#include "link.hpp"

namespace SimulatedAnnealing {
    class Graph
    {
        public:
            std::vector<Position*> positions;
            std::vector<Link*> links;
            double distance;

            Graph(nlohmann::basic_json<>::value_type& positions, int size);
            Graph(Graph& graph);

            void generateLinks();
            void evaluateDistance();
            Graph* generateRandomNeighbor();
    };
}

#endif