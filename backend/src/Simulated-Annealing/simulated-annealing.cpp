#include "../../include/simulated-annealing.hpp"

using namespace SimulatedAnnealing;

SimulatedAnnealingGraph::SimulatedAnnealingGraph(nlohmann::basic_json<>::value_type& positions, int size, int maxTemp, double decrement, double threshold) {
    this->start = new Graph(positions, size);
    this->threshold = threshold;
    this->decrement = decrement;
    this->maxTemp = maxTemp;
    this->iterations = 0;
}

std::vector<annealResult*> SimulatedAnnealingGraph::runSimAnneal() {
    std::vector<annealResult*> results;
    Graph* currState = start;
    Graph* lastState;
    double temp = maxTemp;

    while (temp >= threshold) {
        Graph* nextState = currState->generateRandomNeighbor();
        double deltaEnergy = currState->distance - nextState->distance;
        double r = (double)rand() / RAND_MAX;

        if (deltaEnergy > 0) {
            currState = nextState;
        }
        else if (deltaEnergy < 0 && exp(deltaEnergy / temp) > r) {
            currState = nextState;
        }

        if (currState != lastState) {
            lastState = currState;
            annealResult* result = new annealResult();
            result->iterations = iterations;
            result->state = currState;
            result->temp = temp;
            results.push_back(result);
        }

        temp *= decrement;
        iterations++;
    }

    return results;
}