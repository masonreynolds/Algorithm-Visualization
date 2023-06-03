#include "../../include/checkers.hpp"

using namespace Checkers;


CheckerBoard::CheckerBoard(
    nlohmann::basic_json<>::value_type& redPoses,
    nlohmann::basic_json<>::value_type& blackPoses,
    nlohmann::basic_json<>::value_type& redKings,
    nlohmann::basic_json<>::value_type& blackKings
) {
    for (int i = 0; i < redPoses.size(); i++) {
        this->pieces.set(position_t(((7 - int(redPoses[i][0])) * 8) + (int(redPoses[i][1]))));
        this->reds.set(position_t(((7 - int(redPoses[i][0])) * 8) + (int(redPoses[i][1]))));
        this->redPoses.push_back(((7 - int(redPoses[i][0])) * 8) + (int(redPoses[i][1])));
    }
    
    for (int i = 0; i < blackPoses.size(); i++) {
        this->pieces.set(position_t(((7 - int(blackPoses[i][0])) * 8) + (int(blackPoses[i][1]))));
        this->blacks.set(position_t(((7 - int(blackPoses[i][0])) * 8) + (int(blackPoses[i][1]))));
        this->blackPoses.push_back(((7 - int(blackPoses[i][0])) * 8) + (int(blackPoses[i][1])));
    }

    for (int i = 0; i < redKings.size(); i++) {
        this->pieces.set(position_t(((7 - int(redKings[i][0])) * 8) + (int(redKings[i][1]))));
        this->redKings.set(position_t(((7 - int(redKings[i][0])) * 8) + (int(redKings[i][1]))));
        this->redKingPoses.push_back(((7 - int(redKings[i][0])) * 8) + (int(redKings[i][1])));
    }

    for (int i = 0; i < blackKings.size(); i++) {
        this->pieces.set(position_t(((7 - int(blackKings[i][0])) * 8) + (int(blackKings[i][1]))));
        this->blackKings.set(position_t(((7 - int(blackKings[i][0])) * 8) + (int(blackKings[i][1]))));
        this->blackKingPoses.push_back(((7 - int(blackKings[i][0])) * 8) + (int(blackKings[i][1])));
    }

    calculateHeuristic();
    this->redTurn = false;
}


CheckerBoard::CheckerBoard(CheckerBoard& board) {
    this->pieces = board.pieces;
    this->blacks = board.blacks;
    this->reds = board.reds;
    this->blackKings = board.blackKings;
    this->redKings = board.redKings;

    this->blackPoses = board.blackPoses;
    this->redPoses = board.redPoses;
    this->blackKingPoses = board.blackKingPoses;
    this->redKingPoses = board.redKingPoses;

    this->redTurn = board.redTurn;
}


void CheckerBoard::calculateHeuristic() {
    if (redPoses.size() == 0) {
        heuristic = 100;
    }
    else if (blackPoses.size() == 0) {
        heuristic = -100;
    }
    else {
        int h = 0;

        // h += getConflictingPieces();

        h += 2 * (12 - redPoses.size());
        h -= 2 * (12 - blackPoses.size());

        h += 5 * blackKingPoses.size();
        h -= 5 * redKingPoses.size();

        heuristic = h;
    }
}


int CheckerBoard::getConflictingPieces() {
    int val = 0;

    if (redTurn) {
        for (int i = 0; i < redPoses.size(); i++) {
            val += generatePieceJumps(redPoses[i]).size();
        }

        for (int i = 0; i < redKingPoses.size(); i++) {
            val += generateKingPieceJumps(redKingPoses[i]).size();
        }
    }
    else {
        for (int i = 0; i < blackPoses.size(); i++) {
            val += generatePieceJumps(blackPoses[i]).size();
        }

        for (int i = 0; i < blackKingPoses.size(); i++) {
            val += generateKingPieceJumps(blackKingPoses[i]).size();
        }
    }

    return val;
}


std::vector<CheckerBoard*> CheckerBoard::generateNeighbors() {
    std::vector<CheckerBoard*> neighbors;
    std::vector<CheckerBoard*> poseNeighbors = generatePoseNeighbors();
    neighbors.insert(neighbors.end(), poseNeighbors.begin(), poseNeighbors.end());

    poseNeighbors = generateKingPoseNeighbors();
    neighbors.insert(neighbors.end(), poseNeighbors.begin(), poseNeighbors.end());

    return neighbors;
}


std::vector<CheckerBoard*> CheckerBoard::generatePoseNeighbors() {
    std::vector<CheckerBoard*> neighbors;

    if (redTurn) {
        for (int i = 0; i < redPoses.size(); i++) {
            std::vector<CheckerBoard*> pieceNeighbors = generatePieceMoves(redPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());

            pieceNeighbors = generatePieceJumps(redPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());
        }
    }
    else {
        for (int i = 0; i < blackPoses.size(); i++) {
            std::vector<CheckerBoard*> pieceNeighbors = generatePieceMoves(blackPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());

            pieceNeighbors = generatePieceJumps(blackPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());
        }
    }

    return neighbors;
}


std::vector<CheckerBoard*> CheckerBoard::generateKingPoseNeighbors() {
    std::vector<CheckerBoard*> neighbors;

    if (redTurn) {
        for (int i = 0; i < redKingPoses.size(); i++) {
            std::vector<CheckerBoard*> pieceNeighbors = generateKingPieceMoves(redKingPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());

            pieceNeighbors = generateKingPieceJumps(redKingPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());
        }
    }
    else {
        for (int i = 0; i < blackKingPoses.size(); i++) {
            std::vector<CheckerBoard*> pieceNeighbors = generateKingPieceMoves(blackKingPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());

            pieceNeighbors = generateKingPieceJumps(blackKingPoses[i]);
            neighbors.insert(neighbors.end(), pieceNeighbors.begin(), pieceNeighbors.end());
        }
    }

    return neighbors;
}


std::vector<CheckerBoard*> CheckerBoard::generatePieceMoves(int piece) {
    std::vector<CheckerBoard*> neighbors;
    std::vector<int> poses;

    if (redTurn) {
        if (piece < 56) {
            if (piece % 8 > 0) {
                poses.push_back(piece + 8 - 1);
            }
            
            if ((piece + 1) % 8 > 0) {
                poses.push_back(piece + 8 + 1);
            }
        }
    }
    else {
        if (piece > 7) {
            if (piece % 8 > 0) {
                poses.push_back(piece - 8 - 1);
            }
            
            if ((piece + 1) % 8 > 0) {
                poses.push_back(piece - 8 + 1);
            }
        }
    }

    for (int i = 0; i < poses.size(); i++) {
        bool* check = isPieceAtPose(poses[i]);

        if (!(*check)) {
            CheckerBoard* neighbor = new CheckerBoard(*this);
            neighbor->makeMove(piece, poses[i], false);
            neighbor->redTurn = !this->redTurn;
            neighbor->calculateHeuristic();
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}


std::vector<CheckerBoard*> CheckerBoard::generatePieceJumps(int piece) {
    std::vector<CheckerBoard*> neighbors;
    std::vector<int> poses;

    if (redTurn) {
        if (piece < 48) {
            if (!(piece % 8 == 0 || (piece - 1) % 8 == 0)) {
                poses.push_back(piece + 8 - 1);
            }
            
            if (!((piece + 1) % 8 == 0 || (piece + 2) % 8 == 0)) {
                poses.push_back(piece + 8 + 1);
            }
        }
    }
    else {
        if (piece > 15) {
            if (!(piece % 8 == 0 || (piece - 1) % 8 == 0)) {
                poses.push_back(piece - 8 - 1);
            }
            
            if (!((piece + 1) % 8 == 0 || (piece + 2) % 8 == 0)) {
                poses.push_back(piece - 8 + 1);
            }
        }
    }

    for (int i = 0; i < poses.size(); i++) {
        bool* check = isPieceAtPose(poses[i]);

        if ((*check) && *(check + 1)) {
            int jumpPose = poses[i] - (piece - poses[i]);
            bool* check2 = isPieceAtPose(jumpPose);

            if (!(*check2)) {
                CheckerBoard* neighbor = new CheckerBoard(*this);
                neighbor->makeJump(piece, jumpPose, false);

                std::vector<CheckerBoard*> jumpNeighbors = neighbor->generatePieceJumps(jumpPose);
                neighbors.insert(neighbors.end(), jumpNeighbors.begin(), jumpNeighbors.end());

                neighbor->redTurn = !this->redTurn;
                neighbor->calculateHeuristic();
                neighbors.push_back(neighbor);
            }
        }
    }

    return neighbors;
}


std::vector<CheckerBoard*> CheckerBoard::generateKingPieceMoves(int piece) {
    std::vector<CheckerBoard*> neighbors;
    std::vector<int> poses;

    if (piece < 56) {
        if (piece % 8 > 0) {
            poses.push_back(piece + 8 - 1);
        }
        
        if ((piece + 1) % 8 > 0) {
            poses.push_back(piece + 8 + 1);
        }
    }
    else if (piece > 7) {
        if (piece % 8 > 0) {
            poses.push_back(piece - 8 - 1);
        }
        
        if ((piece + 1) % 8 > 0) {
            poses.push_back(piece - 8 + 1);
        }
    }

    for (int i = 0; i < poses.size(); i++) {
        bool* check = isPieceAtPose(poses[i]);

        if (!(*check)) {
            CheckerBoard* neighbor = new CheckerBoard(*this);
            neighbor->makeMove(piece, poses[i], true);
            neighbor->redTurn = !this->redTurn;
            neighbor->calculateHeuristic();
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}


std::vector<CheckerBoard*> CheckerBoard::generateKingPieceJumps(int piece) {
    std::vector<CheckerBoard*> neighbors;
    std::vector<int> poses;

    if (piece < 48) {
        if (!(piece % 8 == 0 || (piece - 1) % 8 == 0)) {
            poses.push_back(piece + 8 - 1);
        }
        
        if (!((piece + 1) % 8 == 0 || (piece + 2) % 8 == 0)) {
            poses.push_back(piece + 8 + 1);
        }
    }
    else if (piece > 15) {
        if (!(piece % 8 == 0 || (piece - 1) % 8 == 0)) {
            poses.push_back(piece - 8 - 1);
        }
        
        if (!((piece + 1) % 8 == 0 || (piece + 2) % 8 == 0)) {
            poses.push_back(piece - 8 + 1);
        }
    }

    for (int i = 0; i < poses.size(); i++) {
        bool* check = isPieceAtPose(poses[i]);

        if ((*check) && *(check + 1)) {
            int jumpPose = poses[i] - (piece - poses[i]);
            bool* check2 = isPieceAtPose(jumpPose);

            if (!(*check2)) {
                CheckerBoard* neighbor = new CheckerBoard(*this);
                neighbor->makeJump(piece, jumpPose, true);

                std::vector<CheckerBoard*> jumpNeighbors = neighbor->generateKingPieceJumps(jumpPose);
                neighbors.insert(neighbors.end(), jumpNeighbors.begin(), jumpNeighbors.end());

                neighbor->redTurn = !this->redTurn;
                neighbor->calculateHeuristic();
                neighbors.push_back(neighbor);
            }
        }
    }

    return neighbors;
}


void CheckerBoard::makeMove(int piece, int pose, bool isKing) {
    pieces.set(position_t(piece), false);
    pieces.set(position_t(pose), true);

    if (redTurn) {
        if (!isKing) {
            if (pose < 56) {
                reds.set(position_t(piece), false);
                redPoses.push_back(pose);
            }
            else {
                redKings.set(position_t(piece), false);
                redKingPoses.push_back(pose);
            }

            redPoses.erase(std::remove(
                    redPoses.begin(), 
                    redPoses.end(), piece
                ), 
                redPoses.end()
            );
        }
        else {
            redKings.set(position_t(piece), false);
            redKingPoses.erase(std::remove(
                    redKingPoses.begin(), 
                    redKingPoses.end(), piece
                ), 
                redKingPoses.end()
            );

            redKingPoses.push_back(pose);
        }
    }
    else {
        if (!isKing) {
            if (pose >= 8) {
                blacks.set(position_t(piece), false);
                blacks.set(position_t(pose), true);
                blackPoses.push_back(pose);
            }
            else {
                blackKings.set(position_t(piece), false);
                blackKings.set(position_t(pose), true);
                blackKingPoses.push_back(pose);
            }

            blackPoses.erase(std::remove(
                    blackPoses.begin(), 
                    blackPoses.end(), piece
                ), 
                blackPoses.end()
            );
        }
        else {
            blackKings.set(position_t(piece), false);
            blackKingPoses.erase(std::remove(
                    blackKingPoses.begin(), 
                    blackKingPoses.end(), piece
                ), 
                blackKingPoses.end()
            );
            
            blackKingPoses.push_back(pose);
        }
    }
}


void CheckerBoard::makeJump(int piece, int pose, bool isKing) {
    pieces.set(position_t(piece), false);
    pieces.set(position_t(pose), true);

    if (redTurn) {
        if (!isKing) {
            if (pose < 56) {
                reds.set(position_t(piece), false);
                redPoses.push_back(pose);
            }
            else {
                redKings.set(position_t(piece), false);
                redKingPoses.push_back(pose);
            }

            redPoses.erase(std::remove(
                    redPoses.begin(), 
                    redPoses.end(), piece
                ), 
                redPoses.end()
            );
        }
        else {
            redKings.set(position_t(piece), false);
            redKingPoses.erase(std::remove(
                    redKingPoses.begin(), 
                    redKingPoses.end(), piece
                ), 
                redKingPoses.end()
            );

            redKingPoses.push_back(pose);
        }

        int removePose = piece + ((pose - piece) / 2);
        pieces.set(position_t(removePose), false);
        blacks.set(position_t(removePose), false);
        blackKings.set(position_t(removePose), false);

        blackPoses.erase(std::remove(
                blackPoses.begin(), 
                blackPoses.end(), removePose
            ), 
            blackPoses.end()
        );

        blackKingPoses.erase(std::remove(
                blackKingPoses.begin(), 
                blackKingPoses.end(), removePose
            ), 
            blackKingPoses.end()
        );
    }
    else {
        if (!isKing) {
            if (pose >= 8) {
                blacks.set(position_t(piece), false);
                blacks.set(position_t(pose), true);
                blackPoses.push_back(pose);
            }
            else {
                blackKings.set(position_t(piece), false);
                blackKings.set(position_t(pose), true);
                blackKingPoses.push_back(pose);
            }

            blackPoses.erase(std::remove(
                    blackPoses.begin(), 
                    blackPoses.end(), piece
                ), 
                blackPoses.end()
            );
        }
        else {
            blackKings.set(position_t(piece), false);
            blackKingPoses.erase(std::remove(
                    blackKingPoses.begin(), 
                    blackKingPoses.end(), piece
                ), 
                blackKingPoses.end()
            );
            
            blackKingPoses.push_back(pose);
        }

        int removePose = piece + ((pose - piece) / 2);
        pieces.set(position_t(removePose), false);
        reds.set(position_t(removePose), false);
        redKings.set(position_t(removePose), false);

        redPoses.erase(std::remove(
                redPoses.begin(), 
                redPoses.end(), removePose
            ), 
            redPoses.end()
        );

        redKingPoses.erase(std::remove(
                redKingPoses.begin(), 
                redKingPoses.end(), removePose
            ), 
            redKingPoses.end()
        );
    }
}


bool* CheckerBoard::isPieceAtPose(int pose) {
    static bool check[2];

    check[0] = pieces[pose];

    if (blacks[pose] || blackKings[pose]) {
        check[1] = redTurn ? true : false;
    }
    else {
        check[1] = redTurn ? false : true;
    }

    return check;
}