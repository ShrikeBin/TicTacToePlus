#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

#include "constants.hpp"
#include "board.hpp"
#include "checks.hpp"

//══════════════════════════════════════════════════════════════════════════════
//
//         FIX IT TO BE BETTER THAN THE ORIGINAL, VIBE CODED VERSION
//
//              OR.... JUST HAVE SOME FUN NOW THAT IT WORKS!
//
//══════════════════════════════════════════════════════════════════════════════

inline int evaluateBoard(int board[BOARD_SIZE][BOARD_SIZE], int playerSymbol){
    int score = 0;
    int opponentSymbol = (playerSymbol == 1) ? 2 : 1;
    
    // HIGHEST PRIORITY
    if (winCheck(playerSymbol, board)) return 100000;      // Player won
    if (winCheck(opponentSymbol, board)) return -100000;   // Enemy won
    if (loseCheck(playerSymbol, board)) return -50000;    // Player tupid
    if (loseCheck(opponentSymbol, board)) return 50000;   // Enemy tupid
    
    // 2. Check for possible 4 in line
    // Here we analyze every possible 4 in line
    // ROOM FOR IMPROVEMENT
    for (int i = 0; i < 28; ++i){
        int player_count = 0;
        int opponent_count = 0;
        int empty_count = 0;
        bool playerFirst = false;
        bool opponentFirst = false;
        bool playerSecond = false;
        bool opponentSecond = false;
        bool playerThird = false;
        bool opponentThird = false;
        bool playerFourth = false;
        bool opponentFourth = false;
        
        for (int j = 0; j < 4; ++j){
            int row = winTable[i][j][0];
            int col = winTable[i][j][1];
            if (board[row][col] == playerSymbol) ++player_count;
            else if (board[row][col] == opponentSymbol) ++opponent_count;
            if(j==0){
                if(board[row][col] == playerSymbol) playerFirst = true;
                else if (board[row][col] == opponentSymbol) opponentFirst = true;
            }
            if(j==1){
                if(board[row][col] == playerSymbol) playerSecond = true;
                else if (board[row][col] == opponentSymbol) opponentSecond = true;
            }
            if(j==2){
                if(board[row][col] == playerSymbol) playerThird = true;
                else if (board[row][col] == opponentSymbol) opponentThird = true;
            }
            if (j==3){
                if(board[row][col] == playerSymbol) playerFourth = true;
                else if (board[row][col] == opponentSymbol) opponentFourth = true;
            }
            else if (board[row][col] == 0) empty_count++;
            else empty_count++;
        }
        // We have (X)( )( )(X)
        if(playerFirst && playerFourth){
            score += (100 - (opponent_count * 60));
        }
        // Enemy has (X)( )( )(X)
        if(opponentFirst && opponentFourth){
            score -= (100 - (player_count * 50));
        }
        // We have ( )(X)(X)( )
        if(player_count == 2 && playerSecond && playerThird){
            score += (100 - (opponent_count * 40));
        }
        // Enemy has ( )(X)(X)( )
        if(opponent_count == 2 && opponentSecond && opponentThird){
            score -= (100 - (player_count * 35));
        }
        // We have (X)(X)( )( ) or ( )( )(X)(X)
        if(player_count == 2 && ((playerFirst && playerSecond) || (playerThird && playerFourth))){
            if(!(opponentThird|| opponentSecond)){
                score += 10;
                if (!(opponentFirst|| opponentFourth)){
                    score += 60;
                }
            }
        }
        // Enemy has (X)(X)( )( ) or ( )( )(X)(X)
        if(opponent_count == 2 && ((opponentFirst && opponentSecond) || (opponentThird && opponentFourth))){
            if(!(playerThird || playerSecond)){
                score -= 5;
                if (!(playerFirst || playerFourth)){
                    score -= 45;
                }
            }
        }
        // We have (X)( )(X)( ) or ( )(X)( )(X)
        if (player_count == 2 && ((playerFirst && playerThird) || (playerSecond && playerFourth))){
            score += (80 - opponent_count * 70);
        }
        // Enemy has (X)( )(X)( ) or ( )(X)( )(X)
        if (opponent_count == 2 && ((opponentFirst && opponentThird) || (opponentSecond && opponentFourth))){
            score -= (100 - player_count * 70);
        }
        // We have (X)(X)( )(X) or (X)( )(X)(X)
        if (opponent_count < 1 && player_count == 3 && (playerSecond || playerThird)){
            score += 1100;      // Close to winning
        }
        // Enemy has (X)(X)( )(X) or (X)( )(X)(X)
        if (player_count < 1 && opponent_count == 3 && (opponentSecond || opponentThird)){
            score -= 850;       // BLOCK WINNING
        }
    }
    
    // 2. Be present in the middle
    int center_positions[][3] = {
        {2, 2, 15},                                     // Middle
        {1, 2, 10}, {2, 1, 10}, {3, 2, 10}, {2, 3, 10}, // Middle cross
        {1, 1, 5}, {1, 3, 5}, {3, 1, 5}, {3, 3, 5}  // Middle corners
    };
    
    for (int i = 0; i < 9; ++i){
        int row = center_positions[i][0];
        int col = center_positions[i][1];
        int bonus = center_positions[i][2];
        if (board[row][col] == playerSymbol){
            score += bonus;
        }
    }
    
    return score;
}

#endif