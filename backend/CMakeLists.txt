cmake_minimum_required(VERSION 3.10)
project(algorithm_visualization)

FILE(GLOB sources ./src/*.cpp)
FILE(GLOB simulatedAnnealing ./src/Simulated-Annealing/*.cpp)
FILE(GLOB minimax ./src/Minimax/*.cpp)
FILE(GLOB policyLearning ./src/Policy-Learning/*.cpp)
FILE(GLOB headers ./include/*.h)

# find_package(Crow)
add_executable( main ${sources} ${simulatedAnnealing} ${minimax} ${policyLearning} ${headers})
# target_link_libraries(main PUBLIC Crow::Crow)