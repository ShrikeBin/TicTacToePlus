#ifndef BOARD_HPP
#define BOARD_HPP
#include <iostream>
#include <string>
#include "constants.hpp"

inline void clearBoard(int board[BOARD_SIZE][BOARD_SIZE]){
    for (int i = 0; i < 5; ++i){
        for (int j = 0; j < 5; ++j){
            board[i][j] = 0;
        }
    }
}

inline void copyBoard(int source[BOARD_SIZE][BOARD_SIZE], int dest[BOARD_SIZE][BOARD_SIZE]){
    for (int i = 0; i < BOARD_SIZE; ++i){
        for (int j = 0; j < BOARD_SIZE; ++j){
            dest[i][j] = source[i][j];
        }
    }
}

inline void printBoard(int board[BOARD_SIZE][BOARD_SIZE]){
    std::cout << "\n" << "\033[90m┏";
    for (int i = 0; i < BOARD_SIZE - 1; ++i){
        std::cout << "━━━┳";
    }
    std::cout << "━━━┓\033[0m" << std::endl;

    for (int row = 0; row < BOARD_SIZE; ++row){
        std::cout << "\033[90m┃\033[0m";
        for (int col = 0; col < BOARD_SIZE; ++col){
            std::cout << " " << SYMBOLS[board[row][col]] << " \033[90m┃\033[0m";
        }
        std::cout << std::endl;

        if (row < BOARD_SIZE - 1){
            std::cout << "\033[90m┣";
            for (int i = 0; i < BOARD_SIZE - 1; ++i){
                std::cout << "━━━╋";
            }
            std::cout << "━━━┫\033[0m" << std::endl;
        } else {
            std::cout << "\033[90m┗";
            for (int i = 0; i < BOARD_SIZE - 1; ++i){
                std::cout << "━━━┻";
            }
            std::cout << "━━━┛\033[0m" << std::endl;
        }
    }
}

#endif
