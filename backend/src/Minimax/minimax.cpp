#include "../../include/minimax.hpp"

using namespace Checkers;


std::string findMove(CheckerBoard* board, CheckerBoard* newBoard) {
    std::bitset<64> diff = board->pieces;
    diff ^= newBoard->pieces;
    std::string move;
    std::string str;

    if (board->redTurn) {
        move = "Red: ";

        for (int i = 0; i < 64; i++) {
            if (diff[i]) {
                if (newBoard->blacks[i] || newBoard->redKings[i]) {
                    str = xAxis[int(7 - floor(i / 8))] + std::to_string((i % 8) + 1);
                    move += " to " + str;
                }
                else if (board->blacks[i] || board->redKings[i]) {
                    str = xAxis[int(7 - floor(i / 8))] + std::to_string((i % 8) + 1);
                    move += "moved from " + str;
                }
            }
        }
    }
    else {
        move = "Black: ";

        for (int i = 63; i >= 0; i--) {
            if (diff[i]) {
                if (newBoard->blacks[i] || newBoard->blackKings[i]) {
                    str = xAxis[int(7 - floor(i / 8))] + std::to_string((i % 8) + 1);
                    move += " to " + str;
                }
                else if (board->blacks[i] || board->blackKings[i]) {
                    str = xAxis[int(7 - floor(i / 8))] + std::to_string((i % 8) + 1);
                    move += "moved from " + str;
                }
            }
        }
    }

    return move;
}


Minimax::Minimax(CheckerBoard* start) {
    this->start = start;
}


minimaxResult* Minimax::setupMinimax(int depth) {
    int best = 0;
    states = 0;

    if (abs(start->heuristic) == 100 || depth == 0) {
        return new minimaxResult(start, start->heuristic, states, "");
    }

    std::vector<CheckerBoard*> neighbors = start->generateNeighbors();
    CheckerBoard* bestMove = new CheckerBoard(*start);


    if (start->redTurn) {
        best = INT_MAX;

        for (CheckerBoard* neighbor : neighbors) {
            int result = runMinimax(neighbor, best, depth-1);

            if (result < best) {
                bestMove = neighbor;
                best = result;
            }
        }

        if (neighbors.size() == 0) {
            best = 100;
        }
    }
    else {
        best = INT_MIN;

        for (CheckerBoard* neighbor : neighbors) {
            int result = runMinimax(neighbor, best, depth-1);

            if (result > best) {
                bestMove = neighbor;
                best = result;
            }
        }

        if (neighbors.size() == 0) {
            best = -100;
        }
    }

    std::string move = findMove(start, bestMove);
    return new minimaxResult(bestMove, best, states, move);
}

int Minimax::runMinimax(CheckerBoard* current, int AB, int depth)
{
    states++;

    if (abs(current->heuristic) == 100 || depth == 0) {
        return current->heuristic;
    }

    std::vector<CheckerBoard*> neighbors = current->generateNeighbors();

    if (current->redTurn) {
        int beta = INT_MAX;

        for (CheckerBoard* neighbor : neighbors) {
            int result = runMinimax(neighbor, beta, depth-1);

            if (result < beta) {
                beta = result;
            }

            if (beta < AB) {
                break;
            }
        }

        if (neighbors.size() == 0) {
            beta = 100;
        }

        return beta;
    }
    else {
        int alpha = INT_MIN;

        for (CheckerBoard* neighbor : neighbors) {
            int result = runMinimax(neighbor, alpha, depth-1);

            if (result > alpha) {
                alpha = result;
            }

            if (alpha > AB) {
                break;
            }
        }

        if (neighbors.size() == 0) {
            alpha = -100;
        }

        return alpha;
    }
}