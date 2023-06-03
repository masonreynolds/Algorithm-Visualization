#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <iostream>

#include "../include/crow_all.h" // crow
#include "../include/simulated-annealing.hpp"
#include "../include/policy-learning.hpp"
#include "../include/checkers.hpp"
#include "../include/minimax.hpp"


using json = nlohmann::json;

int main()
{
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>();

    CROW_ROUTE(app, "/Simulated-Annealing-WS")
        .websocket()
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
            json jsonData = json::parse(data);

            SimulatedAnnealing::SimulatedAnnealingGraph* env = new SimulatedAnnealing::SimulatedAnnealingGraph(
                jsonData["positions"],
                int(jsonData["positions"].size()),
                int(jsonData["temperature"]),
                double(jsonData["decrement"]),
                double(jsonData["threshold"])
            );

            int updatesOn = bool(jsonData["updatesOn"]);
            env->runSimAnneal(conn, updatesOn);
        });

    
    CROW_ROUTE(app, "/Path-Finder-WS")
        .websocket()
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
            json jsonData = json::parse(data);

            PolicyLearning::PolicyLearningEnvironment* env = new PolicyLearning::PolicyLearningEnvironment(
                jsonData["environment"]
            );

            env->runPolicyLearning(conn);
        });


    CROW_ROUTE(app, "/Checkers-Minimax").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req) {
        json request = json::parse(req.body);
        
        Checkers::CheckerBoard* board = new Checkers::CheckerBoard(
            request["board"]["redPoses"],
            request["board"]["blackPoses"],
            request["board"]["redKings"],
            request["board"]["blackKings"]
        );

        Checkers::Minimax* minimax = new Checkers::Minimax(board);
        Checkers::minimaxResult* result = minimax->setupMinimax(int(request["depth"]));

        return result->dump();
    });


    app.port(18080).multithreaded().run();
}