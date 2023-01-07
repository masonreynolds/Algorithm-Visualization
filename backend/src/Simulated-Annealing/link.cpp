#include "../../include/link.hpp"

using namespace SimulatedAnnealing;


Link::Link(Position* start, Position* end) {
    this->start = start;
    this->end = end;
    calculateWeight();
}

void Link::calculateWeight() {
    this->weight = 1.609344 * 3963.0 * acos((sin(start->lat*M_PI/180.0) * sin(end->lat*M_PI/180.0)) + 
                                    cos(start->lat*M_PI/180.0) * cos(end->lat*M_PI/180.0) * 
                                    cos(end->lon*M_PI/180.0 - start->lon*M_PI/180.0));
}