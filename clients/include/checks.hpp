#ifndef CHECKS_HPP
#define CHECKS_HPP

#include "constants.hpp"
#include "board.hpp"
#include "moves.hpp"

inline bool winCheck(int player, int board[BOARD_SIZE][BOARD_SIZE]){
    bool w = false;
    for (int i = 0; i < 28; i++){
        if ( (board[winTable[i][0][0]][winTable[i][0][1]] == player) &&
            (board[winTable[i][1][0]][winTable[i][1][1]] == player) &&
            (board[winTable[i][2][0]][winTable[i][2][1]] == player) &&
            (board[winTable[i][3][0]][winTable[i][3][1]] == player) )
        {
            w = true;
        }
    }
    return w;
}

inline bool loseCheck(int player, int board[BOARD_SIZE][BOARD_SIZE]){
    bool l = false;
    for (int i = 0; i < 48; i++){
        if ( (board[loseTable[i][0][0]][loseTable[i][0][1]] == player) &&
        (board[loseTable[i][1][0]][loseTable[i][1][1]] == player) &&
        (board[loseTable[i][2][0]][loseTable[i][2][1]] == player) )
        {
            l = true;
        }
    }
    return l;
}

inline bool isWinningMove(int board[BOARD_SIZE][BOARD_SIZE], int move, int playerSymbol){
    if(!isValidMove(move, board)){
        return false;
    }
    
    // Make a copy with that move
    int temp_board[BOARD_SIZE][BOARD_SIZE];
    copyBoard(board, temp_board);
    makeMove(move, temp_board, playerSymbol);
    
    // Check for 4 in line
    for (int i = 0; i < 28; i++){
        int player_count = 0;
        for (int j = 0; j < 4; j++){
            int check_row = winTable[i][j][0];
            int check_col = winTable[i][j][1];
            if (temp_board[check_row][check_col] == playerSymbol){
                player_count++;
            }
        }
        if (player_count == 4) return true;
    }
    return false;
}

inline bool isLosingMove(int board[BOARD_SIZE][BOARD_SIZE], int move, int playerSymbol){
    if(!isValidMove(move, board)){
        return true;
    }
    
    // Make a copy with that move
    int temp_board[BOARD_SIZE][BOARD_SIZE];
    copyBoard(board, temp_board);
    makeMove(move, temp_board, playerSymbol);
    
    // Check for 3 in line
    bool has_three = false;
    for (int i = 0; i < 48; i++){
        int player_count = 0;
        for (int j = 0; j < 3; j++){
            int check_row = loseTable[i][j][0];
            int check_col = loseTable[i][j][1];
            if (temp_board[check_row][check_col] == playerSymbol){
                player_count++;
            }
        }
        if (player_count == 3){
            has_three = true;
            break;
        }
    } 

    if (!has_three){
        return false;
    }

    // Is 3 in line, check for 4 in line
    for (int i = 0; i < 28; i++){
        int win_count = 0;
        for (int j = 0; j < 4; j++){
            int win_row = winTable[i][j][0];
            int win_col = winTable[i][j][1];
            if (temp_board[win_row][win_col] == playerSymbol){
                win_count++;
            }
        }
        if (win_count == 4){
            return false;
        }
    }
    return true;
}

inline bool isBlockingOpponent(int board[BOARD_SIZE][BOARD_SIZE], int move, int playerSymbol){
    if(!isValidMove(move, board)){
        return false;
    }
    int opponentSymbol = (playerSymbol == 1) ? 2 : 1;

    // Make a copy with that move made by the opponent
    int temp_board[BOARD_SIZE][BOARD_SIZE];
    copyBoard(board, temp_board);
    makeMove(move, temp_board, opponentSymbol);

    // Checks if that move is winning for opponent
    for (int i = 0; i < 28; i++){
        int opponent_count = 0;
        for (int j = 0; j < 4; j++){
            int check_row = winTable[i][j][0];
            int check_col = winTable[i][j][1];
            if (temp_board[check_row][check_col] == opponentSymbol){
                opponent_count++;
            }
        }
        if (opponent_count == 4) return true;
    }
    return false;
}

inline bool isTerminalState(int board[BOARD_SIZE][BOARD_SIZE]) {
    if (winCheck(1, board) || winCheck(2, board) || loseCheck(1, board) || loseCheck(2, board)){
        return true;
    }
    
    // Check for draw
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            if (board[i][j] == 0) return false;
        }
    }
    return true;
}

#endif