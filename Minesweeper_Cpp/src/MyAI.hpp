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

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    //creates a temporary board for Agent to keep track of current status of the board
    double **board;
    //bool **boolBoard;

    //acts like a queue to keep track of moves decided by Agent
    vector <Action> nextMoves;

    //keeps track of how many covered tiles are left
    int tilesCovered;

    //prints the temp board, this is just for the purpose of visualize what's happenning
    void printBoard();

    //updates number of mines on the temp board of the last uncovered tile (number is total# of mines)
    //calls printBoard (optional)
    //if number == 1, places flags on the temp board, this helps Agent makes decision later
    //calls makeNextDecisions
    void updateBoard(int number);

    void checkFlags(int tilesCovered);

    int checkRisk(int row, int col);

    void assignProb(int number, int row, int col, double prob);

    void makeNextDecisions();

    //checks if a tile contains mine
    //this function is used when there's two covered tiles left
    //helps Agent make decision on which tile to place flag at
    //int containMine(int r, int c);
    
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
