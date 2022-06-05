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

    //creates a temporary board for Agent to keep track of current status of the board
    double **board;
    double **mineTracker;
    double **prob;
    bool **visited;

    class tileInfo{
    public:
        int number;
        int row;
        int col;
    };

    int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    //acts like a queue to keep track of moves decided by Agent
    std::vector <Action> nextMoves;

    //keeps track of how many covered tiles are left
    int tilesCovered;

    //keeps track of how many mines are left
    int currentMines;

    vector <MyAI::tileInfo> variables;

    //prints the temp board, this is just for the purpose of visualize what's happenning
    void printBoard();

    //updates number of mines on the temp board of the last uncovered tile (number is total# of mines)
    //calls printBoard (optional)
    //if number == 1, places flags on the temp board, this helps Agent makes decision later
    //calls makeNextDecisions
    void updateBoard(int number);

    void uncoverZero();

    void applySinglePoint();

    void checkAdjacent(int number);

    bool inBoard(int y, int x);

    int countSurroundingCovered(int y, int x);

    int countSurroundingMines(int y, int x);

    void gaussianElimnation();

    vector<vector<MyAI::tileInfo> > getContraintsAndFrontier();

    vector<vector<int> > createAugmentedMatrix(vector<vector<MyAI::tileInfo> > matrix);

    void fillMatrix(vector<int> &augMatrix, vector<MyAI::tileInfo> matrix, vector <MyAI::tileInfo> variables);

    void solveAugMatrix(vector<vector<int> > augMatrix);

    vector<vector<int> > gau(vector<vector<int> > augMatrix);

    vector<vector<int> > countOneAndNeg(vector<int> augMatrix);

    void chooseProb();

    void assignProb();
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
