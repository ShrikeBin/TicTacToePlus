#include "board.hpp"
#include "constants.hpp"
#include "checks.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int CURRENT_BOARD[BOARD_SIZE][BOARD_SIZE];

int getManualMove() {
    int move;
    std::cout << " ❯ ENTER YOUR MOVE (e.g., 11-55): ";
    while (!(std::cin >> move)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << " ⚠ Invalid input. Enter a numeric move: ";
    }
    return move;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "ERROR: Invalid number of arguments!\n\n"
                  << "USAGE: " << argv[0] << " <IP> <PORT> <PLAYER_NUMBER> <PLAYER_NAME>\n\n"
                  << "PARAMETERS:\n"
                  << "  IP            - server IP address (e.g., 127.0.0.1)\n"
                  << "  PORT          - server port number (e.g., 8080)\n"
                  << "  PLAYER_NUMBER - player number: 1 (X) or 2 (O)\n"
                  << "  PLAYER_NAME   - player name (max 9 chars)\n"
                  << "EXAMPLE:\n"
                  << "  " << argv[0] << " 127.0.0.1 8080 1 MiniMax\n";
        return -1;
    }

    MY_SYMBOL = 0;
    OPPONENT_SYMBOL = 0;

    try {
        MY_SYMBOL = std::stoi(argv[3]);
    } catch (...) {
        std::cerr << "ERROR: Invalid player number\n";
        return -1;
    }
    if (MY_SYMBOL != 1 && MY_SYMBOL != 2) {
        std::cerr << "ERROR: Player number must be 1 or 2 (got: " << MY_SYMBOL << ")\n";
        return -1;
    }
    OPPONENT_SYMBOL = (MY_SYMBOL == 1) ? 2 : 1;

    std::string player_name = argv[4];
    if (player_name.size() > 9) {
        std::cerr << "ERROR: Player name too long (max 9 chars, got: " << player_name.size() << ")\n";
        return -1;
    }

    std::cout << "╔════════════════════════════════════╗\n"
              << "║     Manual MINIMAX TicTacToe 5x5   ║\n"
              << "║   Manual input client for players  ║\n"
              << "╠════════════════════════════════════╣\n"
              << "║ Player:     " << MY_SYMBOL<< " (" << SYMBOLS[MY_SYMBOL] << ")                  ║\n"
              << "║ Name:       " << player_name << std::string(40 - 16 - player_name.size() - 1, ' ') << "║\n"
              << "║ Depth:      " << DEPTH << std::string(23 - std::to_string(DEPTH).size(), ' ') <<"║\n"
              << "║ Server:     " << argv[1] << ":" << argv[2] << std::string(13 - std::string(argv[2]).size(), ' ') <<"║\n"
              << "╚════════════════════════════════════╝\n\n";

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << " Error creating socket\n";
        return -1;
    }
    std::cout << "Socket created successfully\n";

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(static_cast<uint16_t>(std::stoi(argv[2])));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address\n";
        return -1;
    }

    std::cout << "Connecting to server " << argv[1] << ":" << argv[2] << "...\n";
    if (connect(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Cannot connect to server\n";
        return -1;
    }
    std::cout << "Connected to server successfully\n";

    char server_message[16] = {0};
    if (recv(server_socket, server_message, sizeof(server_message), 0) < 0) {
        std::cerr << "Error receiving message from server\n";
        return -1;
    }
    std::cout << "Server message: " << server_message << '\n';

    std::string player_message = std::to_string(MY_SYMBOL) + " " + player_name;
    if (send(server_socket, player_message.c_str(), player_message.size(), 0) < 0) {
        std::cerr << "Cannot send identification\n";
        return -1;
    }
    std::cout << "Identification sent: " << player_message << '\n';

    clearBoard(CURRENT_BOARD);
    bool end_game = false;

    std::cout << "\n ✶ GAME STARTING...\n"
              << "═══════════════════════════════════════\n";
    printBoard(CURRENT_BOARD);

    while (!end_game) {
        memset(server_message, 0, sizeof(server_message));
        if (recv(server_socket, server_message, sizeof(server_message), 0) <= 0) break;

        int msg_val = std::stoi(server_message);
        int move = msg_val % 100;
        int msg = msg_val / 100;

        if (move != 0) {
            makeMove(move, CURRENT_BOARD, OPPONENT_SYMBOL);
            std::cout << "\nOpponent move: " << move << '\n';
            printBoard(CURRENT_BOARD);
        }

        if (msg == 0 || msg == 6) {
            std::cout << "\nMY TURN! ────────────────────────────\n";
            int my_move = getManualMove();
            
            // Validate and Send
            while (!makeMove(my_move, CURRENT_BOARD, MY_SYMBOL)) {
                std::cout << " ⚠ Invalid move on board! Try again: ";
                my_move = getManualMove();
            }

            printBoard(CURRENT_BOARD);
            std::string move_str = std::to_string(my_move);
            send(server_socket, move_str.c_str(), move_str.size(), 0);
        } else {
            end_game = true;
            std::cout << "\n       ✶✶ GAME OVER! ✶✶\n"
                      << "       ⊕ Played as: " << SYMBOLS[MY_SYMBOL] << " ⊕\n"
                      << "═══════════════════════════════════════\n";
            std::cout << " Result Code: " << msg << "\n";
            std::cout << "═══════════════════════════════════════\n";
        }
    }

    close(server_socket);
    return 0;
}