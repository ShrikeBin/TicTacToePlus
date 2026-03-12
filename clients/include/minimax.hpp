#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include <climits>
#include <stack>
#include <limits>
#include <vector>
#include <cstring>

#include "board.hpp"
#include "constants.hpp"
#include "heuristics.hpp"
#include "minimax.hpp"
#include "checks.hpp"
#include "moves.hpp"

/**
 * Minimax results
 * 
 * @param score - score
 * @param move - move with that score
 */
struct MinimaxResult {
    int score;
    int move;
};

/**
 * @struct Frame
 * @brief Represents a single stack frame for the iterative alpha‑beta minimax algorithm.
 *
 * @param board         A local copy of the 2D game board state at this node.
 * @param depth         The remaining search depth from this node.
 * @param alpha         The current alpha value (best already explored for maximizer).
 * @param beta          The current beta value (best already explored for minimizer).
 * @param maximizing    True if this node is a maximizing‑player node; false if minimizing.
 * @param currentPlayer The ID of the player whose turn it is at this node.
 * @param moves         Array of all legal moves generated for this board position.
 * @param move_count    The number of valid entries in the `moves` array.
 * @param idx           Index of the next move in `moves[]` to explore.
 * @param bestScore     The best score found so far at this node (initialized to ±∞).
 * @param bestMove      The move index (from `moves[]`) that produced `bestScore`.
 * @param state         State flag: 
 *                      - 0 = first time the frame is processed (generate moves).  
 *                      - 1 = returning from a child node (integrate childResult).
 * @param childResult   The MinimaxResult returned by the most recently explored child.
 */
struct Frame {
    int board[BOARD_SIZE][BOARD_SIZE];
    int depth;
    int alpha;
    int beta;
    bool maximizing;
    int currentPlayer;
    int moves[BOARD_SIZE * BOARD_SIZE];
    int move_count;
    int idx;
    int bestScore;
    int bestMove;
    int state;
    MinimaxResult childResult;
};

/**
 * MINIMAX w/ Alpha Beta prunning
 * 
 * - Prune: alpha >= beta, we can prune branch
 * 
 * @param board - current board
 * @param depth - depth left
 * @param alpha - best for maximising player
 * @param beta - best for minimizing player
 * @param maximizing_player - if now we maximize or minimize
 * @param current_player - current player 1 or 2
 * @return struct with best possible move
 */
MinimaxResult minimax(int board[BOARD_SIZE][BOARD_SIZE], int depth, int alpha, int beta, bool maximizing_player, int currentPlayer){
    if (depth == 0 || isTerminalState(board)){
        MinimaxResult baseRes;
        baseRes.score = evaluateBoard(board, MY_SYMBOL);
        baseRes.move = -1;
        return baseRes;
    }

    std::stack<Frame> stk;

    // Push initial frame
    Frame root;
    memcpy(root.board, board, sizeof(root.board));
    root.depth = depth;
    root.alpha = alpha;
    root.beta = beta;
    root.maximizing = maximizing_player;
    root.currentPlayer = currentPlayer;
    root.move_count = 0;
    root.idx = 0;
    root.bestScore = maximizing_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    root.bestMove = -1;
    root.state = 0;
    stk.push(root);

    MinimaxResult returnValue{0, -1};

    while (!stk.empty()){
        Frame &f = stk.top();

        // If this is the first time we handle this frame, generate moves, etc.
        if (f.state == 0){
            // Check terminal or depth == 0 again (needed for inner nodes)
            if (f.depth == 0 || isTerminalState(f.board)){
                MinimaxResult baseRes;
                baseRes.score = evaluateBoard(f.board, MY_SYMBOL);
                baseRes.move = -1;

                // Pop and pass result to parent
                stk.pop();
                if (stk.empty()){
                    returnValue = baseRes;
                    break;
                }
                // Parent frame:
                Frame &parent = stk.top();
                parent.childResult = baseRes;
                parent.state = 1;  // Indicate “returned from child”
                continue;
            }

            // Generate all legal moves for this node
            getMoves(f.board, f.moves, &f.move_count);

            // Initialize bestScore & bestMove were already set before push

            f.state = 1;  // Next state: start iterating child nodes
            f.idx = 0;
        }

        // State == 1: either first iteration or returning from a child
        if (f.idx > 0 || (f.idx == 0 && f.move_count > 0)){
            // If we’ve just returned from a child (except on the very first idx=0 pre-child),
            // incorporate the child’s result into this node’s bestScore/bestMove/alpha/beta.
            if (f.idx > 0) {
                MinimaxResult &childRes = f.childResult;
                int mv = f.moves[f.idx - 1];

                if (f.maximizing){
                    if (childRes.score > f.bestScore){
                        f.bestScore = childRes.score;
                        f.bestMove = mv;
                    }
                    f.alpha = std::max(f.alpha, f.bestScore);
                } else {
                    if (childRes.score < f.bestScore){
                        f.bestScore = childRes.score;
                        f.bestMove = mv;
                    }
                    f.beta = std::min(f.beta, f.bestScore);
                }
            }

            // Check alpha-beta prune
            if (f.alpha >= f.beta){
                // We can prune: return f.bestScore for this frame
                MinimaxResult pruned;
                pruned.score = f.bestScore;
                pruned.move = f.bestMove;

                stk.pop();
                if (stk.empty()){
                    returnValue = pruned;
                    break;
                }
                Frame &parent = stk.top();
                parent.childResult = pruned;
                parent.state = 1;
                continue;
            }
        }

        // If all moves exhausted, pop with final bestScore
        if (f.idx >= f.move_count){
            MinimaxResult out;
            out.score = f.bestScore;
            out.move = f.bestMove;

            stk.pop();
            if (stk.empty()) {
                returnValue = out;
                break;
            }
            Frame &parent = stk.top();
            parent.childResult = out;
            parent.state = 1;
            continue;
        }

        // Otherwise, push the next child onto the stack
        int nextMove = f.moves[f.idx];
        f.idx++;  // Increment so that when we return, we know which move to attribute

        // Make a copy of the board, apply the move, and push a new frame
        int childBoard[BOARD_SIZE][BOARD_SIZE];
        memcpy(childBoard, f.board, sizeof(childBoard));

        if (!makeMove(nextMove, f.board, f.currentPlayer)) {
            // Illegal move? Just skip
            memcpy(f.board, childBoard, sizeof(childBoard));
            continue;
        }

        // Prepare child frame
        Frame child;
        memcpy(child.board, f.board, sizeof(child.board));
        child.depth = f.depth - 1;
        child.alpha = f.alpha;
        child.beta = f.beta;
        child.maximizing = !f.maximizing;
        child.currentPlayer = (f.currentPlayer == 1 ? 2 : 1);
        child.move_count = 0;
        child.idx = 0;
        child.bestScore = child.maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        child.bestMove = -1;
        child.state = 0;

        // Restore parent board (population after child returns will reapply the board)
        memcpy(f.board, childBoard, sizeof(childBoard));

        stk.push(child);
    }

    // Make the board default again, if we need it for some reason
    // memcpy(board, stk.empty() ? board : stk.top().board, sizeof(root.board));

    return returnValue;
}


#endif