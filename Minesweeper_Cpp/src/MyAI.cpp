// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    //initilize row, col, and total number of mines
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines   = _totalMines;
    // agentX = _agentX;
    // agentY = _agentY;

    //initilize the board
    board = new int* [colDimension];
    for (int i=0; i< colDimension; i++)
    {
       board[i] = new int[rowDimension];
    }

    lastMove.xCoordinate = _agentX;
    lastMove.yCoordinate = _agentY;

    //make next 8 moves based on the first move and push it to nextMoves vector
    for (int col = 0; col < colDimension; ++col) {
        for (int row = 0; row < rowDimension; ++row) {
            if((row == _agentX-1 || row == _agentX || row == _agentX+1) && (col == _agentY-1 || col == _agentY || col == _agentY+1)){
                move nextMove = new move();
                nextMove.action = UNCOVER;
                nextMove.xCoordinate = _agentX;
                nextMove.yCoordinate = _agentY;
                nextMoves.push_back(nextMove)

                board[_agentX][_agentY] = 0;
            }
        }
    }
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    if (number >= 0){
        //DO SOMETHING HERE TO UPDATE THE BOARD AND FIGURE OUT NEXT MOVES BASED ON THE PERCEPT VALUE DISCOVERED FROM THE LAST RECENT MOVE
        updateBoard(number);
        Action nextMove = new Action();
        nextMove.action = nextMoves.front().action;
        nextMove.x = nextMoves.front().xCoordinate;
        nextMove.y = nextMoves.front().yCoordinate;

        //keep track of the current position of the agent
        lastMove.xCoordinate = nextMoves.front().xCoordinate;
        lastMove.yCoordinate = nextMoves.front().yCoordinate;
        erase(nextMoves.front());
        return nextMove;
    }

    return {LEAVE,-1,-1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

Agent::int updateBoard(int number){
    board[lastMove.yCoordinate][lastMove.xCoordinate] = number;

    makeNextDecisions();
}

Agent::void makeNextDecision(){

}


// ======================================================================
// YOUR CODE ENDS
// ======================================================================
