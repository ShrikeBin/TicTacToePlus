#ifndef MOVES_HPP
#define MOVES_HPP

#include <algorithm>

#include "constants.hpp"

inline bool isValidMove(int move, int board[BOARD_SIZE][BOARD_SIZE]){
    int row = (move / 10) - 1;
    int col = (move % 10) - 1;
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != EMPTY_CELL){
        return false;
    }
    return true;
}

inline bool makeMove(int move, int board[BOARD_SIZE][BOARD_SIZE], int playerSymbol){
    if(!isValidMove(move, board)){
        return false;
    }
    int row = (move / 10) - 1;
    int column = (move % 10) - 1;
    board[row][column] = playerSymbol;
    return true;
}

inline void getMoves(int board[BOARD_SIZE][BOARD_SIZE], int moves[], int* movesCount) {
    *movesCount = 0;
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            if (board[i][j] == EMPTY_CELL){
                moves[(*movesCount)++] = (i + 1) * 10 + (j + 1);
            }
        }
    }

    if (*movesCount > 1){
        std::shuffle(moves, moves + *movesCount, gen);
    }
}

#endif