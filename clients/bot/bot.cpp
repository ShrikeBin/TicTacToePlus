#include "board.hpp"
#include "constants.hpp"
#include "heuristics.hpp"
#include "minimax.hpp"
#include "checks.hpp"

#include <iostream>
#include <string>
#include <cstring>    // for memset
#include <cstdlib>    // for std::stoi
#include <unistd.h>   // for close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * GLOBAL MAIN BOARD FOR THE BOT
 * Main board representation used whilest playing the game
 */
int CURRENT_BOARD[BOARD_SIZE][BOARD_SIZE];


/**
 * Self Explanatory
 * @param player - player symbol
 * @param depth - depth of search
 * @return best move (format 11-55)
 */
int getBestMove(int player, int depth){
    int moves[BOARD_SIZE * BOARD_SIZE];
    int move_count;
    getMoves(CURRENT_BOARD, moves, &move_count);
    
    if (move_count == 0) return -1; // Brak dostępnych ruchów
    
    std::cout << "⦿ Analyzing " << move_count << " viable moves...\n";
    
    // PRIORITY 1: Instant win
    for (int i = 0; i < move_count; ++i){
        if (isWinningMove(CURRENT_BOARD, moves[i], player)){
            std::cout << "♛ FOUND A WIN! winning move: " << moves[i] << "\n";
            return moves[i];
        }
    }
    
    // PRIORITY 2: Check if we could block
    int opponent = (player == 1) ? 2 : 1;
    for (int i = 0; i < move_count; ++i) {
        if (isBlockingOpponent(CURRENT_BOARD, moves[i], player)){
            if (!isLosingMove(CURRENT_BOARD, moves[i], player)){
                std::cout << " ░ Blocking enemy " << moves[i] << "\n";
                return moves[i];
            } else {
                std::cout <<" ✧ Move " << moves[i] << " blocks enemy, but it's suicidal. :| \n";
            }
        }
    }
    
    // PRIORITY 3: filter suicidal tendencies
    int safe_moves[BOARD_SIZE * BOARD_SIZE];
    int safe_count = 0;
    
    std::cout << "Checking moves:\n";
    for (int i = 0; i < move_count; ++i){
        if (!isLosingMove(CURRENT_BOARD, moves[i], player)){
            safe_moves[safe_count++] = moves[i];
            std::cout << " ✔  Move " << moves[i] << "- safe ◉\n";
        } else {
            std::cout << " x  Move " << moves[i] << " - suicide ◌\n";
        }
    }
    
    if (safe_count == 0){
        std::cout <<"⚠ ALL MOVES ARE SUICIDAL\n";
        std::cout << "☆ Russian roulette, but all chambers loaded!\n";
        srand(time(NULL));
        return moves[rand() % move_count];
    }
    
    std::cout << " Got " << safe_count << " safe moves. Running minimax (depth "<< depth << ")...\n";
    
    // PRIORITY 4: Run minimax
    int best_move = -1;
    int best_score = INT_MIN;
    
    for (int i = 0; i < safe_count; ++i){
        int move = safe_moves[i];
        
        int temp_board[BOARD_SIZE][BOARD_SIZE];
        copyBoard(CURRENT_BOARD, temp_board);
        
        if (makeMove(move, CURRENT_BOARD, player)){
            MinimaxResult result = minimax(CURRENT_BOARD, depth - 1, INT_MIN, INT_MAX, false, opponent);
            
            copyBoard(temp_board, CURRENT_BOARD);
            
            std::cout << "   Move " << move << " -> score: "<< result.score << "\n";
            
            if (result.score > best_score) {
                best_score = result.score;
                best_move = move;
            }
        } else {
            copyBoard(temp_board, CURRENT_BOARD);
            std::cout << " ⚠ Error while moving " << move << "\n";
        }
    }
    
    if (best_move == -1){
        std::cout << "Minimax found nothing, running first safe move: " << safe_moves[0] << "\n";
        return safe_moves[0];
    }
    
    std::cout << "Choosing move: " << best_move << " (score: " << best_score << ")\n";
    return best_move;
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "ERROR: Invalid number of arguments!\n\n"
                  << "USAGE: " << argv[0] << " <IP> <PORT> <PLAYER_NUMBER> <PLAYER_NAME> <DEPTH>\n\n"
                  << "PARAMETERS:\n"
                  << "  IP            - server IP address (e.g., 127.0.0.1)\n"
                  << "  PORT          - server port number (e.g., 8080)\n"
                  << "  PLAYER_NUMBER - player number: 1 (X) or 2 (O)\n"
                  << "  PLAYER_NAME   - player name (max 9 chars)\n"
                  << "  DEPTH         - minimax search depth (1-10)\n\n"
                  << "EXAMPLE:\n"
                  << "  " << argv[0] << " 127.0.0.1 8080 1 MiniMax 5\n";
        return -1;
    }

    DEPTH = 0;
    MY_SYMBOL = 0;
    OPPONENT_SYMBOL = 0;

    try {
        DEPTH = std::stoi(argv[5]);
    } catch (...) {
        std::cerr << "ERROR: Invalid search depth value\n";
        return -1;
    }
    if (DEPTH < 1 || DEPTH > 10) {
        std::cerr << "ERROR: Depth must be between 1 and 10 (got: " << DEPTH << ")\n";
        return -1;
    }

    if( DEPTH >= 8){
        std::cout << "I GUESS YOU HAVE A LOOOT OF TIME TO RUN SUCH DEPTH....\n"
                  << "I hope you have a good computer, because this will take a while...\n";
    }

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
              << "║        MINIMAX TicTacToe 5x5       ║\n"
              << "║  with moderate  heuristics quality ║\n"
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

    while (!end_game){
        memset(server_message, 0, sizeof(server_message));
        if (recv(server_socket, server_message, sizeof(server_message), 0) < 0) {
            std::cerr << "Error receiving message from server\n";
            return -1;
        }

        int msg = 0, move = 0;
        try {
            msg = std::stoi(server_message);
        } catch (...) {
            std::cerr << "Invalid server message format\n";
            return -1;
        }
        move = msg % 100;
        msg /= 100;

        std::cout << "\nServer message: " << server_message << " (type: " << msg << ", move: " << move << ")\n";

        if (move != 0){
            std::cout << "Opponent move: " << move << '\n';
            makeMove(move, CURRENT_BOARD, OPPONENT_SYMBOL);
            std::cout << "\nBoard after opponent move:\n";
            printBoard(CURRENT_BOARD);
        }

        if (msg == 0 || msg == 6) {
            std::cout << "\nMY TURN! Calculating best move...\n"
                      << "─────────────────────────────────────\n";
            if(msg != 6){
                move = getBestMove(MY_SYMBOL, DEPTH);
            } else {
                std::uniform_int_distribution<> distr(2, 4);
                int rand1 = distr(gen);
                int rand2 = distr(gen);
                move = rand1 * 10 + rand2; // Random move in range 22-44
            }

            if (move == -1){
                std::cerr << "CRITICAL ERROR: Cannot find a move!\n";
                break;
            }

            std::cout << "─────────────────────────────────────\n"
                      << " ✶ MAKING MOVE: " << move << '\n';

            makeMove(move, CURRENT_BOARD ,MY_SYMBOL);
            std::cout << "\nBoard after my move:\n";
            printBoard(CURRENT_BOARD);

            player_message = std::to_string(move);
            if (send(server_socket, player_message.c_str(), player_message.size(), 0) < 0) {
                std::cerr << " Cannot send move to server\n";
                return -1;
            }
            std::cout << "Move sent to server\n";
        } else {
            end_game = true;
            std::cout << "\n       ✶✶ GAME OVER! ✶✶\n"
                      << "       ⊕ Played as: " << SYMBOLS[MY_SYMBOL] << " ⊕\n"
                      << "═══════════════════════════════════════\n";

            switch (msg){
                case 1:
                    std::cout << "      ▒ THIS BOT WINS! ▒\n";
                    break;
                case 2:
                    std::cout << "      ▒ THIS BOT LOSES :C ▒\n";
                    break;
                case 3:
                    std::cout << "      ▒ DRAW! ▒\n";
                    break;
                case 4:
                    std::cout << "      ▒ THIS BOT WINS! ▒\n";
                    break;
                case 5:
                    std::cout << "      ▒ THIS BOT LOSES :C ▒\n";
                    break;
                default:
                    std::cout << "   UNKNOWN: " << msg << '\n';
                    break;
            }
            std::cout << "═══════════════════════════════════════\n";
        }
    }

    close(server_socket);
    std::cout << "\n Disconnected from server.\n";

    return 0;
}
