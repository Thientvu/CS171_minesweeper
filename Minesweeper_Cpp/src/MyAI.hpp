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
    int **board;

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

    //if number == 0, all covered tiles around the last uncovered tile can be uncover also
    //decides what tiles to uncover next, pushes decision into "nextMoves"
    void makeNextDecisions(int number);

    //this function is called when no more obvious decision can be made
    //it happens when there's 1 tile, 2 tiles, 4 tiles left
    //this functions helps Agent make the final decision(s) and place the flag
    void placeFlag(int tilesCovered);

    //checks if a tile contains mine
    //this function is used when there's two covered tiles left
    //helps Agent make decision on which tile to place flag at
    int containMine(int r, int c);
    
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
