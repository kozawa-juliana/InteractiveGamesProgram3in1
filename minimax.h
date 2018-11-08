#ifndef _MINIMAX_H
#define _MINIMAX_H

#include "board_game.h"

#define MAX_STEPS_NUMBER 7
#define WIN_SCORE 100000

/* number of steps of minimax algorithm (difficulty) */
extern int number_steps;





/* get game properties */
void minimax_start_game(properties game_pro);
/* return best move of user, -1 if failed */
int minimax_get_best_move(board_t board, int player);

#endif /* _MINIMAX_H */
