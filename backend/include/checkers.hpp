#ifndef CHECKERS_H
#define CHECKERS_H

#include <nlohmann/json.hpp> // nlohmann-json3-dev
#include <bitset>
#include <iostream>
#include <cstddef>
#include <vector>
#include <string>

/*
56 57 58 59 60 61 62 63
48 49 50 51 52 53 54 55
40 41 42 43 44 45 46 47
32 33 34 35 36 37 38 39 
24 25 26 27 28 29 30 31
16 17 18 19 20 21 22 23
8  9  10 11 12 13 14 15
0  1  2  3  4  5  6  7 
*/

namespace Checkers {
    typedef std::size_t length_t, position_t;

    class CheckerBoard {
        public:
            std::bitset<64> pieces;
            std::bitset<64> blacks;
            std::bitset<64> reds;
            std::bitset<64> blackKings;
            std::bitset<64> redKings;

            std::vector<int> blackPoses;
            std::vector<int> redPoses;
            std::vector<int> blackKingPoses;
            std::vector<int> redKingPoses;

            int heuristic;
            bool redTurn;

            CheckerBoard(
                nlohmann::basic_json<>::value_type& redPoses,
                nlohmann::basic_json<>::value_type& blackPoses,
                nlohmann::basic_json<>::value_type& redKings,
                nlohmann::basic_json<>::value_type& blackKings
            );
            CheckerBoard(CheckerBoard& board);

            std::vector<CheckerBoard*> generateNeighbors();
            std::vector<CheckerBoard*> generatePoseNeighbors();
            std::vector<CheckerBoard*> generateKingPoseNeighbors();
            std::vector<CheckerBoard*> generatePieceMoves(int piece);
            std::vector<CheckerBoard*> generateKingPieceMoves(int piece);
            std::vector<CheckerBoard*> generatePieceJumps(int piece);
            std::vector<CheckerBoard*> generateKingPieceJumps(int piece);
            void makeMove(int piece, int pose, bool isKing);
            void makeJump(int piece, int pose, bool isKing);
            bool* isPieceAtPose(int pose);
            int getConflictingPieces();
            void calculateHeuristic();
    };
}

#endif