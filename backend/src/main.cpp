#include <iostream>
#include "crow.h" // crow
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/cors.h"
#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include "../include/simulated-annealing.hpp"
#include "../include/graph.hpp"

using json = nlohmann::json;

int main()
{
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>();

    CROW_ROUTE(app, "/Simulated-Annealing").methods("POST"_method)
    ([&](const crow::request& req) {
        json request = json::parse(req.body);

        SimulatedAnnealing::SimulatedAnnealingGraph* env = new SimulatedAnnealing::SimulatedAnnealingGraph(
            request["positions"],
            int(request["positions"].size()),
            int(request["temperature"]),
            double(request["decrement"]),
            double(request["threshold"])
        );

        SimulatedAnnealing::annealResult* result = env->runSimAnneal().back();

        return *result;
    });

    app.port(18080).run();
}