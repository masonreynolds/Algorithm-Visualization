#include "../../include/graph.hpp"

using namespace SimulatedAnnealing;


Graph::Graph(nlohmann::basic_json<>::value_type& positions, int size) {
    for (int i = 0; i < size; i++) {
        this->positions.push_back(new Position(
            positions[i]["id"],
            positions[i]["name"],
            positions[i]["lat"],
            positions[i]["lon"]
        ));
    }

    generateLinks();
}

Graph::Graph(Graph& graph) {
    this->positions = graph.positions;
    
    for (int i = 0; i< graph.links.size(); i++) {
        this->links.push_back(new Link(graph.links[i]->start, graph.links[i]->end));
    }

    this->distance = graph.distance;
}

void Graph::generateLinks() {
    int current = 0;

    while (links.size() < positions.size()) {
        int index;

        do {
            index = rand() % positions.size();

            if (links.size() == positions.size() - 1) {
                index = 0;
                break;
            }
        } while (index == current || std::any_of(links.begin(), links.end(), [&](const Link* l) { return l->start == positions[index]; }));

        Link* link = new Link(positions[current], positions[index]);
        links.push_back(link);
        current = index;
    }

    evaluateDistance();
}

void Graph::evaluateDistance() {
    double distance = 0;

    for (int i = 0; i < links.size(); i++) {
        distance += links[i]->weight;
    }

    this->distance = distance;
}

Graph* Graph::generateRandomNeighbor() {
    Graph* neighbor = new Graph(*this);
    Position* pos1;
    Position* pos2;

    do {
        pos1 = positions[rand() % positions.size()];
        pos2 = positions[rand() % positions.size()];
    } while (pos2 == pos1);

    for (int i = 0; i < neighbor->links.size(); i++) {
        if (neighbor->links[i]->start == pos1) {
            neighbor->links[i]->start = pos2;
        }
        else if (neighbor->links[i]->start == pos2) {
            neighbor->links[i]->start = pos1;
        }

        if (neighbor->links[i]->end == pos1) {
            neighbor->links[i]->end = pos2;
        }
        else if (neighbor->links[i]->end == pos2) {
            neighbor->links[i]->end = pos1;
        }
        
        neighbor->links[i]->calculateWeight();
    }

    neighbor->evaluateDistance();

    return neighbor;
}