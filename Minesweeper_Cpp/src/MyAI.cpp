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

    //initialize Agent 
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines   = _totalMines;
    agentX = _agentX;
    agentY = _rowDimension - _agentY -1;  //convert Y value given from world.cpp to how we structure the temp board here

    //number of covered tiles, -1 because the first tile is always already uncovered in world.cpp
    tilesCovered = rowDimension * colDimension -1;

    //initialize temp board to all 0s
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

    //board to keep track of visited tiles
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

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    //"number" is total number of mines surrounding a tile, number can be either -1 (FLAG or done with the game) or 0 or a positive number based on the just uncovered tile from world.cpp, 
    while(tilesCovered != totalMines){ 

        //update board based on number
        updateBoard(number);

        //Uncover everything around a 0
        if (number == 0){ 
            uncoverZero();
        }

        // if all mines are flagged, all tiles left are safe to uncover
        if(tilesCovered != 0 && totalMines == 0){ 
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

        //Basic rule of thumb heuristic, single-point strategies
        if(nextMoves.empty()){
            applySinglePoint();
        }

        //use gaussian elimination to solve linear eqs(matrix)
        if(nextMoves.empty()){
            gaussianElimination();
        }

        // If there are two covered left and they both have equal chance, pick randomly 
        if(nextMoves.empty()){
            if(tilesCovered == 2 && totalMines == 1) {
                bool loopCondition = true;
                int row = 0;
                int col = 0;
                while(loopCondition && row < rowDimension) {
                    while(loopCondition && col < colDimension) {
                        if(visited[row][col] == false) {
                            MyAI::Action nextMove;
                            nextMove.action = UNCOVER;
                            nextMove.x = col;
                            nextMove.y = row;
                            nextMoves.push_back(nextMove);
                            visited[row][col] = true;
                            loopCondition = false;
                        }
                        ++col;
                    }
                    col = 0;
                    ++row;
                }
            }
        }

        //applying probability to covered tiles
        if(nextMoves.empty()){
            chooseProb();
        }

        //if all heuristics above can't figure out the next move(s), stop the game
        if(nextMoves.empty()){
            break;
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

        //decrement number of covered tiles and totalMines on the board
        if(action == UNCOVER) {
            --tilesCovered;
        }
        else if(action == FLAG){
            --totalMines;
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

//temporary, this is only for debugging purposes
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
            cout << visited[row][col] << " | ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

//check if a tile is in board
bool MyAI::inBoard(int y, int x) {
    if(y >= 0 && y < rowDimension) {
        if(x >= 0 && x < colDimension) {
            return true;
        }
        return false;
    }
    return false;
}

//update our temp boards here at MyAI.cpp based on number coming from world.cpp
void MyAI::updateBoard(int number){
    if(board[agentY][agentX] == -8.8) {
        board[agentY][agentX] = number;
    }

    if(mineTracker[agentY][agentX] == -8.8) {
        mineTracker[agentY][agentX] = number;
        prob[agentY][agentX] = number;
        if(number != -1) { // if newly uncovered tile is not a flag, decrement newly uncovered tile on minetrack board if any tiles around it is a flag
            for (int i = 0; i < 9; ++i) {
                if(inBoard(agentY + dy[i], agentX + dx[i])){
                    if(mineTracker[agentY + dy[i]][agentX + dx[i]] == -1) {
                        mineTracker[agentY][agentX] -=1;
                        prob[agentY][agentX] -=1;;
                    }
                }       
            }
        }
        else if(number == -1) { // if newly uncovered tile is a flag, decrement everything around it
            for (int i = 0; i < 9; ++i) {
                if(inBoard(agentY + dy[i], agentX + dx[i])){
                    if(mineTracker[agentY + dy[i]][agentX + dx[i]] != -8.8 && mineTracker[agentY + dy[i]][agentX + dx[i]] > 0) {
                        mineTracker[agentY + dy[i]][agentX + dx[i]] -= 1;
                        prob[agentY + dy[i]][agentX + dx[i]] -= 1;
                    }
                }       
            }
        }
    }
    
    //printBoard(); //temporary
}

//uncover all the tiles around 0
void MyAI::uncoverZero(){
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

//count the total number of covered tiles around a given tile
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

//count the total number of flagged tiles around a given tile
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

//Function to clear mines/ uncover tiles around a number
void MyAI::checkAdjacent(int number) {
    int surroundingCovered = countSurroundingCovered(agentY, agentX);
    int surroundingMines = countSurroundingMines(agentY, agentX);

    //If number of mines flagged is the same as the number, clear all tiles around it
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
    }//If number of mines flagged added with covered tiles is the number, then flag all the covered tiles
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

//Call Adjacent checker around every tile
void MyAI::applySinglePoint() {
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

//get info to construct a 2D vector 
//constraints are all the numbered tiles that have at least a covered tiles as their adjacent tiles
vector<vector<MyAI::tileInfo> > MyAI::getContraintsAndFrontier(){
    vector<vector<MyAI::tileInfo> > matrix;
    vector<MyAI::tileInfo> constraints;

    //This loop goes over the entire board to look for tiles that have covered tiles adjacent to them
    for(int row = 0; row < rowDimension; row++){
        for(int col = 0; col < colDimension; col++){
           if(mineTracker[row][col] > 0){ // if found then create a vector, push all the adjacent convered tiles to the vector
                for (int i = 0; i < 9; ++i) {
                    if(inBoard(row + dy[i], col + dx[i])){
                        if(mineTracker[row + dy[i]][col + dx[i]] == -8.8) {
                            MyAI::tileInfo constraint;
                            constraint.number = mineTracker[row][col];
                            constraint.row = row + dy[i];
                            constraint.col = col + dx[i];

                            constraints.push_back(constraint);
                        }
                    }
                }                    
                matrix.push_back(constraints); //push the vector above into the matrix
                constraints.clear();
            }
        }
    }

    return matrix;
}

//helper function for createAugmentedMatrix(matrix) to fill each row of the augmented matrix
void MyAI::fillMatrix(vector<int> &augMatrix, vector<MyAI::tileInfo> matrix, vector <MyAI::tileInfo> variables){
    for(int b = 0; b < matrix.size(); b++){
        for(int col = 0; col < variables.size() +1; col ++){
            if (col == variables.size())
                augMatrix[col] = matrix[b].number;
            else if(matrix[b].row == variables[col].row && matrix[b].col == variables[col].col)
                augMatrix[col] = 1;
        }
    }
}

//construct a augmented matrix 
//each row is constructed based on a numbered tile that has at least one covered tile that is adjacent to it
//the last row is all the covered tiles left on the board = total number of mines left
vector<vector<int> > MyAI::createAugmentedMatrix(vector<vector<MyAI::tileInfo> > matrix){
    variables.clear();

    //get all the covered tiles left on the board
    for(int row = 0; row < rowDimension; row++){
        for(int col = 0; col < colDimension; col++){
            if(mineTracker[row][col] == -8.8){
                MyAI::tileInfo var;
                var.number = -2;
                var.row = row;
                var.col = col;
                
                variables.push_back(var);
            }
        }
    }

    //convert matrix into an aug matrix
    vector<vector<int> > augMatrix(matrix.size(), vector<int> (variables.size() +1, 0)); 
    for(int row = 0; row < matrix.size(); row++){
        fillMatrix(augMatrix[row], matrix[row], variables);
    }
   
   //last row, including all the covered tiles left and total number of mines left
    vector<int> temp;
    for(int i = 0; i < variables.size() +1; i++){
        if(i == variables.size())
            temp.push_back(totalMines);
        else
            temp.push_back(1);
    }        
    augMatrix.push_back(temp);

    return augMatrix;
}

//Solve the augmented matrix using row echelon form
vector<vector<int> > MyAI::rowEchelon(vector<vector<int> > augMatrix){
    int lead = 0, temp;
    for (int row = 0; row < augMatrix.size(); row++){
        if (lead >= augMatrix[0].size())
            return augMatrix;
        temp = row;
        while (augMatrix[temp][lead] == 0){
            temp += 1;
            if (temp == augMatrix.size()){
                temp = row;
                lead += 1;
                if (augMatrix[0].size() == lead)
                    return augMatrix;
            }         
        }
        vector<int> tempM;
        tempM = augMatrix[temp];
        augMatrix[temp] = augMatrix[row];
        augMatrix[row] = tempM;

        int level = augMatrix[row][lead];
        for(int b =0; b < augMatrix[row].size(); b++){
            augMatrix[row][b] = augMatrix[row][b]/level;
        }

        for(int a = 0; a < augMatrix.size(); a++){
            if (a != row){
                level = augMatrix[a][lead];
                for(int b =0; b < augMatrix[0].size(); b++){
                    augMatrix[a][b] =augMatrix[a][b]-augMatrix[row][b]*level;
                }
            }
        }
        lead += 1;
    }      
    return augMatrix;
}

//Helper function for solveAugMatrix, counting total number of 1 and -1 in a row after row echelon form was applied
vector<vector<int> > MyAI::countOneAndNeg(vector<int> augMatrix){
    vector<vector<int> > results;
    vector<int> ones;
    vector<int> negs;

    for(int i =0; i< augMatrix.size()-1; i++){
        if(augMatrix[i] == 1){
            ones.push_back(i);
        }
        else if(augMatrix[i] == -1){
            negs.push_back(i);
        }
    }

    results.push_back(ones);
    results.push_back(negs);

    return results;
}

//after row echelon form was applied, count total number of 1 and -1 left in each row and based on the last value of the row (right side of the eq), we can check if a tile is a mine or is safe to uncover
void MyAI::solveAugMatrix(vector<vector<int> > augMatrix){
    vector<vector<int> > results, onesAndNegs;
    results = rowEchelon(augMatrix);
    for(int row = 0; row < results.size(); row++){
        onesAndNegs = countOneAndNeg(results[row]);
        if(results[row][results[row].size() -1] == 0){
            if(onesAndNegs[0].size() > 0 && onesAndNegs[1].size() == 0){
                for(int i = 0; i < onesAndNegs[0].size(); i++){
                    if(visited[variables[onesAndNegs[0][i]].row][variables[onesAndNegs[0][i]].col] == false){
                        MyAI::Action nextMove;
                        nextMove.action = UNCOVER;
                        nextMove.x = variables[onesAndNegs[0][i]].col;
                        nextMove.y = variables[onesAndNegs[0][i]].row;
                        nextMoves.push_back(nextMove);
                        visited[variables[onesAndNegs[0][i]].row][variables[onesAndNegs[0][i]].col] = true;
                    }
                }
            }else if(onesAndNegs[0].size() == 0 && onesAndNegs[1].size() > 0){
                for(int i = 0; i < onesAndNegs[1].size(); i++){
                    if(visited[variables[onesAndNegs[1][i]].row][variables[onesAndNegs[1][i]].col] == false){
                        MyAI::Action nextMove;
                        nextMove.action = FLAG;
                        nextMove.x = variables[onesAndNegs[1][i]].col;
                        nextMove.y = variables[onesAndNegs[1][i]].row;
                        nextMoves.push_back(nextMove);
                        visited[variables[onesAndNegs[1][i]].row][variables[onesAndNegs[1][i]].col] = true;
                    }
                }
            }
        }
        else if(results[row][results[row].size() -1] > 0){
            if(onesAndNegs[0].size() == results[row][results[row].size() -1]){
                for(int i = 0; i < onesAndNegs[0].size(); i++){
                    if(visited[variables[onesAndNegs[0][i]].row][variables[onesAndNegs[0][i]].col] == false){
                        MyAI::Action nextMove;
                        nextMove.action = FLAG;
                        nextMove.x = variables[onesAndNegs[0][i]].col;
                        nextMove.y = variables[onesAndNegs[0][i]].row;
                        nextMoves.push_back(nextMove);
                        visited[variables[onesAndNegs[0][i]].row][variables[onesAndNegs[0][i]].col] = true;
                    }
                }
                for(int i = 0; i < onesAndNegs[1].size(); i++){
                    if(visited[variables[onesAndNegs[1][i]].row][variables[onesAndNegs[1][i]].col] == false){
                        MyAI::Action nextMove;
                        nextMove.action = UNCOVER;
                        nextMove.x = variables[onesAndNegs[1][i]].col;
                        nextMove.y = variables[onesAndNegs[1][i]].row;
                        nextMoves.push_back(nextMove);
                        visited[variables[onesAndNegs[1][i]].row][variables[onesAndNegs[1][i]].col] = true;
                    }
                }
            }
        }
        else if(results[row][results[row].size() -1] < 0){
            if(onesAndNegs[1].size() == results[row][results[row].size() -1]){
                for(int i = 0; i < onesAndNegs[0].size(); i++){
                    if(visited[variables[onesAndNegs[0][i]].row][variables[onesAndNegs[0][i]].col] == false){
                        MyAI::Action nextMove;
                        nextMove.action = UNCOVER;
                        nextMove.x = variables[onesAndNegs[0][i]].col;
                        nextMove.y = variables[onesAndNegs[0][i]].row;
                        nextMoves.push_back(nextMove);
                        visited[variables[onesAndNegs[0][i]].row][variables[onesAndNegs[0][i]].col] = true;
                    }
                }
                for(int i = 0; i < onesAndNegs[1].size(); i++){
                    if(visited[variables[onesAndNegs[1][i]].row][variables[onesAndNegs[1][i]].col] == false){
                        MyAI::Action nextMove;
                        nextMove.action = FLAG;
                        nextMove.x = variables[onesAndNegs[1][i]].col;
                        nextMove.y = variables[onesAndNegs[1][i]].row;
                        nextMoves.push_back(nextMove);
                        visited[variables[onesAndNegs[1][i]].row][variables[onesAndNegs[1][i]].col] = true;
                    }
                }
            }
        }
    }
}

//applying Gaussian Elimination to solve the board
void MyAI::gaussianElimination(){
    vector<vector<MyAI::tileInfo> > matrix = getContraintsAndFrontier();
    if(matrix.size() > 1){
        vector<vector<int> > augMatrix = createAugmentedMatrix(matrix);
            solveAugMatrix(augMatrix);
    }
}

//destructor for our temp boards
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

//Calls the assignProbability function and chooses the lowest chance
void MyAI::chooseProb() {
    assignProb();
    
    int x;
    int y;

    //Assign 0 to initialize
    if(visited[0][0] == false) {
        x = 0;
        y = 0;
    }
    //Otherwise, just choose a visited spot so no chance of mine
    else {
        bool loopCondition = true;
        int row = 0;
        int col = 0;
        while(loopCondition && row < rowDimension) {
            while(loopCondition && col < colDimension) {
                if(visited[row][col] && board[row][col] != -1) {
                    y = row;
                    x = col;
                    loopCondition = false;
                }
                ++col;
            }
            col = 0;
            ++row;
        }
    }
    
    //Initialize probability to 1
    double probability = 1;

    //Iterate through to find the lowest probability
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            if(prob[row][col] < probability && prob[row][col] > 0) {
                probability = prob[row][col];
                x = col;
                y = row;
            }
        }
    }

    //Uncover the lowest probability found
    MyAI::Action nextMove;
    nextMove.action = UNCOVER;
    nextMove.x = x;
    nextMove.y = y;
    nextMoves.push_back(nextMove);
    visited[y][x] = true;
}

void MyAI::assignProb() {
    //Assign Probability
    for (int row = 0; row < rowDimension; ++row) {
        for (int col = 0; col < colDimension; ++col) {
            double probability = 0;
            double count = 0;
            //If at a spot the value of minetracker is >= 1, then assign probability for covered tiles
            if(mineTracker[row][col] >= 1) {
                count = countSurroundingCovered(row, col);
                //Just to make sure no divide by 0
                if(count > 0) {
                    //Probability is effective label / number of available spots
                    probability = mineTracker[row][col] / count;

                    //Assign probability for adjacent covered tiles
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