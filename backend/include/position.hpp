#ifndef POSITION_H
#define POSITION_H

#include <string>

namespace SimulatedAnnealing {
	class Position
	{
		public:
			int id;
			double lat;
			double lon;
			std::string name;

			Position(int id, std::string name, double lat, double lon);
	};
}

#endif