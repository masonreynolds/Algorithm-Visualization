#include "../../include/simulated-annealing.hpp"

using namespace SimulatedAnnealing;

SimulatedAnnealingGraph::SimulatedAnnealingGraph(nlohmann::basic_json<>::value_type& positions, int size, int maxTemp, double decrement, double threshold) {
    this->start = new Graph(positions, size);
    this->threshold = threshold;
    this->decrement = decrement;
    this->maxTemp = maxTemp;
    this->iterations = 0;
}

void SimulatedAnnealingGraph::runSimAnneal(crow::websocket::connection& conn, bool updatesOn) {
    Graph* currState = start;
    annealResult* result;
    Graph* lastState;

    annealResult* minResult = new annealResult(currState, iterations, maxTemp, false);
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
            result = new annealResult(currState, iterations, temp, false);

            if (result->state->distance < minResult->state->distance) {
                minResult = result;
            }

            if (updatesOn) {
                conn.send_text(result->dump());
            }
        }

        temp *= decrement;
        iterations++;
    }

    minResult->last = true;
    conn.send_text(minResult->dump());
}