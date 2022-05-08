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

    //initilizate Agent 
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines   = _totalMines;
    agentX = _agentX;
    agentY = colDimension - _agentY -1;  //convert value given from world.cpp to how we structure the temp board here

    //number of covered tiles, -1 because the first tile is always already uncovered in world.cpp
    tilesCovered = rowDimension * colDimension -1;

    //initilize temp board
    board = new int* [colDimension];
    for (int i=0; i< colDimension; i++){
       board[i] = new int[rowDimension];
    }
    for(int row = 0; row < rowDimension; row++){
        for(int col = 0; col < colDimension; col++){
            board[col][row] = -1;
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

    //number can be either -1 or else based on world.cpp, "number" is total number of mines surrounding a tile
    if (number >= 0){

        //udpate board based on number, number here is total number of mines surrounding the last uncovered tile done by Agent or by world.cpp (First uncovered tile, in this case number = 0)
        //this will return next decision(s) made by Agent stored in nextMoves
        updateBoard(number);

        //however if nextMoves is empty, it means decision can't be made based on previous decisions
        //then calling placeFlag will do the job
        if(nextMoves.empty()){
            placeFlag(tilesCovered);
        }

        //returns the next decision
        Action_type action = nextMoves.front().action;
        int x = nextMoves.front().x;
        int y = nextMoves.front().y;

        //saves location of the about-to-made decision
        agentX = nextMoves.front().x;
        agentY = nextMoves.front().y;

        //removes the about-to-made decision from nextMoves
        nextMoves.erase(nextMoves.begin());

        //decrement number of covered tiles on the board
        tilesCovered --;

        //converts y value from here back to what can be accurately used by world.cpp
        return {action, x, colDimension-y -1};
    }

    return {LEAVE,-1,-1};

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
}

// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

void MyAI::printBoard(){
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
                cout << board[row][col] << " | ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

void MyAI::updateBoard(int number){
    board[agentY][agentX] = number;

    //printBoard()
    
    if (number == 1){
        for (int row = 0; row < rowDimension; ++row) {
            for (int col = 0; col < colDimension; ++col) {
                if((row == agentX-1 || row == agentX || row == agentX+1) && (col == agentY-1 || col == agentY || col == agentY+1) && (board[col][row] == -1)){
                    board[col][row] = 8;
                }
            }
        }
    }
    
    makeNextDecisions(number);
}

void MyAI::makeNextDecisions(int number){
    if (number == 0){
        for (int row = 0; row < rowDimension; ++row) {
            for (int col = 0; col < colDimension; ++col) {
                if((row == agentX-1 || row == agentX || row == agentX+1) && (col == agentY-1 || col == agentY || col == agentY+1) && (board[col][row] == -1 || board[col][row] == 8)){
                    board[col][row] = 0;
                    MyAI::Action nextMove;
                    nextMove.action = UNCOVER;
                    nextMove.x = row;
                    nextMove.y = col;
                    nextMoves.push_back(nextMove);
                }
            }
        }
    }
}

void MyAI::placeFlag(int tilesCovered){
    int row, col, temp, tempRow, tempCol;
    if(tilesCovered == 1){
        for (row = 0; row < rowDimension; ++row) {
            for (col = 0; col < colDimension; ++col) {
                if(board[col][row] == 8){
                    nextMoves.push_back({FLAG, row, col});
                    return;
                }
            }
        } 
    }
    else if(tilesCovered == 2){
        for (row = 0; row < rowDimension; ++row) {
            for (col = 0; col < colDimension; ++col) {
                if(board[col][row] == 8){
                    if(col+1 < colDimension && board[col+1][row] == 8){
                        temp = col +1; 
                        if (containMine(row, col) > containMine(row, col+1)){
                            nextMoves.push_back({UNCOVER, row, temp});
                            nextMoves.push_back({FLAG, row, col});
                            return;
                        }
                        else{
                            nextMoves.push_back({UNCOVER, row, col});
                            nextMoves.push_back({FLAG, row, temp});
                            return;
                        }
                    }
                    else if  (row + 1 < rowDimension && board[col][row+1] == 8){
                        temp = row +1;
                        if (containMine(row, col) > containMine(row +1, col)){
                            nextMoves.push_back({UNCOVER, temp, col});
                            nextMoves.push_back({FLAG, row, col});
                            return;
                        }
                        else{
                            nextMoves.push_back({UNCOVER, row, col});
                            nextMoves.push_back({FLAG, temp, col});
                            return;
                        }
                    }
                }
            }
        } 
    }
    else if(tilesCovered == 4){
        for (row = 0; row < rowDimension; ++row) {
            for (col = 0; col < colDimension; ++col) {
                if(board[col][row] == -1){
                    if (col == 0 && row == 0){
                        nextMoves.push_back({UNCOVER, row, col});
                        temp = col + 1;
                        nextMoves.push_back({UNCOVER, row, temp});
                        temp = row + 1;
                        nextMoves.push_back({UNCOVER, temp, col});
                        tempRow = row + 1;
                        tempCol = col + 1;
                        nextMoves.push_back({FLAG, tempRow, tempCol});
                        return;
                    }
                    else if (col == 0 && row == 4){
                        nextMoves.push_back({UNCOVER, row, col});
                        temp = col + 1;
                        nextMoves.push_back({UNCOVER, row, temp});
                        temp = row - 1;
                        nextMoves.push_back({UNCOVER, temp, col});
                        tempRow = row - 1;
                        tempCol = col + 1;
                        nextMoves.push_back({FLAG, tempRow, tempCol});
                        return;
                    }
                    else if (col == 4 && row == 0){
                        nextMoves.push_back({UNCOVER, row, col});
                        temp = col - 1;
                        nextMoves.push_back({UNCOVER, row, temp});
                        temp = row + 1;
                        nextMoves.push_back({UNCOVER, temp, col});
                        tempRow = row + 1;
                        tempCol = col - 1;
                        nextMoves.push_back({FLAG, tempRow, tempCol});
                        return;
                    }
                    else if (col == 4 && row == 4){
                        nextMoves.push_back({UNCOVER, row, col});
                        temp = col - 1;
                        nextMoves.push_back({UNCOVER, row, temp});
                        temp = row - 1;
                        nextMoves.push_back({UNCOVER, temp, col});
                        tempRow = row - 1;
                        tempCol = col - 1;
                        nextMoves.push_back({FLAG, tempRow, tempCol});
                        return;
                    }
                }
            }
        }
    }
}

int MyAI::containMine(int r, int c){
    int count = 0;
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            if((row == r-1 || row == r || row == r+1) && (col == c-1 || col == c || col == c+1) && (board[col][row] == -1 || board[col][row] == 1))
                count ++;
        }
    }
    
    return count;
}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================
