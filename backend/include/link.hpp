#ifndef LINK_H
#define LINK_H

#include <math.h>
#include "position.hpp"

namespace SimulatedAnnealing {
    class Link
    {
        public:
            Position* start;
            Position* end;
            double weight;

            Link(Position* start, Position* end);

            void calculateWeight();
    };
}

#endif