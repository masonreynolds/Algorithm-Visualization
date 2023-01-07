#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <iostream>

#include "crow.h" // crow
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/cors.h"
#include "../include/simulated-annealing.hpp"
#include "../include/graph.hpp"

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

            std::vector<SimulatedAnnealing::annealResult*> results = env->runSimAnneal();
            
            if (bool(jsonData["updatesOn"])) {
                for (auto result : results) {
                    conn.send_text(result->dump());
                }
            }
            else {
                conn.send_text(results.back()->dump());
            }
        });

    app.port(18080).run();
}