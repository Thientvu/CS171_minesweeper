// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <fstream>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    ~MyAI();
    
    Action getAction ( int number ) override;

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

private:
    //creates a temporary board for Agent to keep track of current status of the board
    double **board;
    double **mineTracker;
    double **prob;
    bool **visited;

    int tilesCovered;
    class tileInfo{
    public:
        int number;
        int row;
        int col;
    };

    int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    std::vector <Action> nextMoves;

    vector <MyAI::tileInfo> variables;

    void printBoard();

    void updateBoard(int number);

    void uncoverZero();

    void applySinglePoint();

    void checkAdjacent(int number);

    bool inBoard(int y, int x);

    int countSurroundingCovered(int y, int x);

    int countSurroundingMines(int y, int x);

    void gaussianElimination();

    vector<vector<MyAI::tileInfo> > getContraintsAndFrontier();

    vector<vector<int> > createAugmentedMatrix(vector<vector<MyAI::tileInfo> > matrix);

    void fillMatrix(vector<int> &augMatrix, vector<MyAI::tileInfo> matrix, vector <MyAI::tileInfo> variables);

    void solveAugMatrix(vector<vector<int> > augMatrix);

    vector<vector<int> > rowEchelon(vector<vector<int> > augMatrix);

    vector<vector<int> > countOneAndNeg(vector<int> augMatrix);

    void chooseProb();

    void assignProb();
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
