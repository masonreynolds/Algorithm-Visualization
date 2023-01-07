#include "../../include/position.hpp"

using namespace SimulatedAnnealing;


Position::Position(int id, std::string name, double lat, double lon) {
    this->id = id;
    this->name = name;
    this->lat = lat;
    this->lon = lon;
}