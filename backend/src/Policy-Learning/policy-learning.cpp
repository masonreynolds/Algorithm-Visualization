#include "../../include/policy-learning.hpp"

using namespace PolicyLearning;

PolicyLearningEnvironment::PolicyLearningEnvironment(nlohmann::basic_json<>::value_type& environment) {
    for (int i = 0; i < this->agent.size(); i++) {
        this->agent[i] = environment["agent"][i];
    }

    for (int i = 0; i < this->goal.size(); i++) {
        this->goal[i] = environment["goal"][i];
    }

    for (int i = 0; i < environment["blocks"].size(); i++) {
        this->blocks.push_back(std::array<int, 2> {
            environment["blocks"][i][0],
            environment["blocks"][i][1]
        });
    }

    for (int i = 0; i < environment["rewards"].size(); i++) {
        std::vector<double> row;
        for (int k = 0; k < environment["rewards"][i].size(); k++) {
            row.push_back(environment["rewards"][i][k]);
        }
        this->rewards.push_back(row);
    }
}

void PolicyLearningEnvironment::runPolicyLearning(crow::websocket::connection& conn) {
    std::map<std::string, bool> exploredCells;
    std::vector<std::array<int, 2>> path;

    std::array<int, 2> originalAgent = std::array<int, 2> {
        this->agent[0],
        this->agent[1]
    };

    int count = 0;

    while (this->agent[0] != this->goal[0] || this->agent[1] != this->goal[1]) {
        std::vector<std::array<int, 2>> agentNeighbors = getAgentNeighbors(exploredCells);
        double total = 0;

        std::cout << this->agent[0] << ", " << this->agent[1] << std::endl;
        for (int i = 0; i < agentNeighbors.size(); i++) {
            std::array<int, 2> n = agentNeighbors[i];
            total += this->rewards[n[0]][n[1]] + 1;
            std::cout << "  " << n[0] << ", " << n[1] << std::endl;
        }

        float choice = (float)(rand()) / (float)(RAND_MAX);
        float moveChance = 0;

        for (int i = 0; i < agentNeighbors.size(); i++) {
            std::array<int, 2> n = agentNeighbors[i];
            moveChance += (this->rewards[n[0]][n[1]]+1) / total;
            std::cout << choice << n[0] << ": " << moveChance << std::endl;
            if (choice <= moveChance) {
                this->rewards[n[0]][n[1]] -= float(1) / (this->rewards.size() * this->rewards.size());
                std::string key = std::to_string(this->agent[0]) + "|" + std::to_string(this->agent[1]);
                std::cout << key << std::endl;
                exploredCells[key] = true;
                this->agent = std::array<int, 2> {
                    n[0],
                    n[1]
                };
                break;
            }
        }

        std::cout << this->agent[0] << ", " << this->agent[1] << std::endl;
        std::cout << std::endl;

        conn.send_text(this->dump());
        // std::cout << "WAITING " << count << std::endl;
        // sleep(1);
        // std::cout << "WAITED " << count << std::endl;
        count++;
        break;
    }

    // this->agent = originalAgent;
    // conn.send_text(this->dump());
}

std::vector<std::array<int, 2>> PolicyLearningEnvironment::getAgentNeighbors(std::map<std::string, bool> exploredCells) {
    std::vector<std::array<int, 2>> neighbors;
    std::string key;

    key = std::to_string(this->agent[0]+1) + "|" + std::to_string(this->agent[1]);
    if (this->agent[0] < (this->rewards.size()-1) && exploredCells.find(key) == exploredCells.end()) {
        neighbors.push_back(std::array<int, 2> {
            this->agent[0]+1,
            this->agent[1]
        });
    }
    key = std::to_string(this->agent[0]-1) + "|" + std::to_string(this->agent[1]);
    if (this->agent[0] > 0 && exploredCells.find(key) == exploredCells.end()) {
        neighbors.push_back(std::array<int, 2> {
            this->agent[0]-1,
            this->agent[1]
        });
    }
    key = std::to_string(this->agent[0]) + "|" + std::to_string(this->agent[1]+1);
    if (this->agent[1] < (this->rewards.size()-1) && exploredCells.find(key) == exploredCells.end()) {
        neighbors.push_back(std::array<int, 2> {
            this->agent[0],
            this->agent[1]+1
        });
    }
    key = std::to_string(this->agent[0]) + "|" + std::to_string(this->agent[1]-1);
    if (this->agent[1] > 0 && exploredCells.find(key) == exploredCells.end()) {
        neighbors.push_back(std::array<int, 2> {
            this->agent[0],
            this->agent[1]-1
        });
    }

    return neighbors;
}