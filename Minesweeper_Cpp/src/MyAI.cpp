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

    //initalize Agent 
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines   = _totalMines;
    agentX = _agentX;
    agentY = _rowDimension - _agentY -1;  //convert value given from world.cpp to how we structure the temp board here

    //number of covered tiles, -1 because the first tile is always already uncovered in world.cpp
    tilesCovered = rowDimension * colDimension -1;

    //initialize number of mines left on map
    currentMines = totalMines;

    //initilize temp board
    //-8.8 is unexplored
    board = new double* [rowDimension];
    for (int i=0; i< rowDimension; i++){
       board[i] = new double[colDimension];
    }
    for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            board[row][col] = -8.8;
        }
    }
    board[agentY][agentX] = 0;

    //initialize board for mine tracking
    //-8.8 is unexplored
    //number is how many mines are not flagged around it
    //-1 means flagged
    mineTracker = new double* [rowDimension];
    for (int i=0; i< rowDimension; i++){
       mineTracker[i] = new double[colDimension];
    }
    for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            mineTracker[row][col] = -8.8;
        }
    }
    mineTracker[agentY][agentX] = 0;

    visited = new bool* [rowDimension];
    for (int i=0; i< rowDimension; i++){
       visited[i] = new bool[colDimension];
    }
    for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            visited[row][col] = false;
        }
    }
    visited[agentY][agentX] = true;

    //Create Probability Board
    prob = new double* [rowDimension];
    for (int i=0; i< rowDimension; i++){
       prob[i] = new double[colDimension];
    }
    for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            prob[row][col] = -8.8;
        }
    }

    //printBoard();
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
    while(tilesCovered > totalMines){ 

        //udpate board based on number, number here is total number of mines surrounding the last uncovered tile done by Agent or by world.cpp (First uncovered tile, in this case number = 0)
        //this will return next decision(s) made by Agent stored in nextMoves
        updateBoard(number);

        //however if nextMoves is empty, it means decision can't be made based on previous decisions
        //then calling checkBoundary will do the job
        if(nextMoves.empty()){//Basic rule of thumb heuristic, Model checking
            checkBoundary(tilesCovered);
        }

        //Add heuristic here for no longer working
        if(nextMoves.empty()){
            if(tilesCovered == totalMines+1) {//If last is surrounded by mines
                for(int row = 0; row < rowDimension; ++row){
                    for(int col = 0; col < colDimension; ++col){
                        if(visited[row][col] == false) {
                            MyAI::Action nextMove;
                            nextMove.action = UNCOVER;
                            nextMove.x = col;
                            nextMove.y = row;
                            nextMoves.push_back(nextMove);
                            visited[row][col] = true;
                        }
                    }
                }
            }
            else {
                break;
            }
        }

        // Actual heuristic if no idea what to do
        if(nextMoves.empty()){
            chooseProb();
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
        if(action == UNCOVER) {
            --tilesCovered;
        }

        //converts y value from here back to what can be accurately used by world.cpp
        return {action, x, rowDimension - 1 - y};
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
    for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            cout << board[row][col] << " | ";
        }
        cout << endl;
    }
    cout << endl << endl;

   for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            cout << mineTracker[row][col] << " | ";
        }
        cout << endl;
    }
    cout << endl << endl;

    for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            cout << prob[row][col] << " | ";
        }
        cout << endl;
    }
    cout << endl << endl;

   for(int row = 0; row < rowDimension; ++row){
        for(int col = 0; col < colDimension; ++col){
            cout << visited[row][col] << " | ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

void MyAI::updateBoard(int number){
    if(board[agentY][agentX] == -8.8) {
        board[agentY][agentX] = number;
    }
    if(mineTracker[agentY][agentX] == -8.8) {
        mineTracker[agentY][agentX] = number;
        if(number != -1) {
            for (int i = 0; i < 9; ++i) {
                if(inBoard(agentY + dy[i], agentX + dx[i])){
                    if(mineTracker[agentY + dy[i]][agentX + dx[i]] == -1) {
                        mineTracker[agentY][agentX] -=1;
                    }
                }       
            }
        }
    }
    
    if (number == 0){//Uncover everything around a 0
        for (int i = 0; i < 9; ++i) {
            if(inBoard(agentY + dy[i], agentX + dx[i])){
                if(board[agentY + dy[i]][agentX + dx[i]] == -8.8 && visited[agentY + dy[i]][agentX + dx[i]] == false) {
                    MyAI::Action nextMove;
                    nextMove.action = UNCOVER;
                    nextMove.x = agentX + dx[i];
                    nextMove.y = agentY + dy[i];
                    nextMoves.push_back(nextMove);
                    visited[agentY + dy[i]][agentX + dx[i]] = true;
                }
            }
        }
    }
    else if(number > 0)//Greater than 0, check if covered around it is mines
    {
        checkAdjacent(number);
    }//Lower everything around the flag
    else if(number == -1) {
        for (int i = 0; i < 9; ++i) {
            if(inBoard(agentY + dy[i], agentX + dx[i])){
                if(mineTracker[agentY + dy[i]][agentX + dx[i]] != -8.8 && mineTracker[agentY + dy[i]][agentX + dx[i]] > 0) {
                    mineTracker[agentY + dy[i]][agentX + dx[i]] -= 1;
                }
            }       
        }
    }

    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            if(mineTracker[row][col] == 0 && board[row][col] != 0) {
                agentY = row;
                agentX = col;
                checkAdjacent(board[row][col]);
            }
        }
    }

    assignProb();

    //printBoard(); //temporary
}

void MyAI::checkAdjacent(int number) {
    int surroundingCovered = countSurroundingCovered(agentY, agentX);
    int surroundingMines = countSurroundingMines(agentY, agentX);

    if(surroundingMines == number) {
        if(surroundingCovered > 0) {
            for (int i = 0; i < 9; ++i) {
                if(inBoard(agentY + dy[i], agentX + dx[i])){
                    if(board[agentY + dy[i]][agentX + dx[i]] == -8.8 && visited[agentY + dy[i]][agentX + dx[i]] == false) {
                        MyAI::Action nextMove;
                        nextMove.action = UNCOVER;
                        nextMove.x = agentX + dx[i];
                        nextMove.y = agentY + dy[i];
                        nextMoves.push_back(nextMove);
                        visited[agentY + dy[i]][agentX + dx[i]] = true;
                    }
                }   
            }
        }
    }
    else if(number == surroundingMines + surroundingCovered) {
        for (int i = 0; i < 9; ++i) {
            if(inBoard(agentY + dy[i], agentX + dx[i])){
                if(board[agentY + dy[i]][agentX + dx[i]] == -8.8 && visited[agentY + dy[i]][agentX + dx[i]] == false) {
                    board[agentY + dy[i]][agentX + dx[i]] = -1;
                    MyAI::Action nextMove;
                    nextMove.action = FLAG;
                    nextMove.x = agentX + dx[i];
                    nextMove.y = agentY + dy[i];
                    nextMoves.push_back(nextMove);
                    visited[agentY + dy[i]][agentX + dx[i]] = true;
                }
            }
        }
    }
}

void MyAI::checkBoundary(int tilesCovered) {
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            if(board[row][col] > 0)
            {
                agentY = row;
                agentX = col;
                checkAdjacent(board[row][col]);
            }
        }
    }
}

bool MyAI::inBoard(int y, int x) {
    if(y >= 0 && y < rowDimension) {
        if(x >= 0 && x < colDimension) {
            return true;
        }
        return false;
    }
    return false;
}

int MyAI::countSurroundingCovered(int y, int x) {
    int count = 0;

    for (int i = 0; i < 9; ++i) {
        if(inBoard(y + dy[i], x + dx[i])){
            if(board[y + dy[i]][x + dx[i]] == -8.8) {
                ++count;
            }
        }
    }

    return count;
}

int MyAI::countSurroundingMines(int y, int x) {
    int count = 0;

    for (int i = 0; i < 9; ++i) {
        if(inBoard(y + dy[i], x + dx[i])){
            if(board[y + dy[i]][x + dx[i]] == -1) {
                ++count;
            }
        }
    }

    return count;
}

MyAI::~MyAI() {
    for(int col = 0; col < colDimension; ++col) {
        delete[] board[col];
        delete[] mineTracker[col];
        delete[] visited[col];
        delete[] prob[col];
    }

    delete[] board;
    delete[] mineTracker;
    delete[] visited;
    delete[] prob;
}

void MyAI::chooseProb() {
    assignProb();
    
    int x;
    int y;
    double probability = 1;

    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            if(prob[row][col] <= probability && prob[row][col] > 0) {
                probability = prob[row][col];
                x = col;
                y = row;
            }
        }
    }

    MyAI::Action nextMove;
    nextMove.action = UNCOVER;
    nextMove.x = x;
    nextMove.y = y;
    nextMoves.push_back(nextMove);
    visited[y][x] = true;
}

void MyAI::assignProb() {
    //Copy Board
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            prob[row][col] = mineTracker[row][col];
        }
    }

    //Assign Probability
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            double probability;
            int count = 0;
            if(prob[row][col] >= 1) {
                count = countSurroundingCovered(row, col);
                if(count > 0) {
                    probability = prob[row][col] / double(count);

                    for (int i = 0; i < 9; ++i) {
                        if(inBoard(row + dy[i], col + dx[i])){
                            if(board[row + dy[i]][col + dx[i]] == -8.8) {
                                if(probability > prob[row + dy[i]][col + dx[i]]) {
                                    prob[row + dy[i]][col + dx[i]] = probability;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================