cmake_minimum_required(VERSION 3.10)
project(algorithm_visualization)

FILE(GLOB sources ./src/*.cpp)
FILE(GLOB otherSources ./src/Simulated-Annealing/*.cpp)
FILE(GLOB headers ./include/*.h)

find_package(Crow)
add_executable( main ${sources} ${otherSources} ${headers})
target_link_libraries(main PUBLIC Crow::Crow)