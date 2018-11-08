#ifndef _CONNECT4_H
#define _CONNECT4_H

#include "board_game.h"
#include "minimax.h"
#include "images_manage.h"

#define CONNECT4_BOARD_WIDTH	7
#define CONNECT4_BOARD_HEIGHT	6

/* function that calculates board score */
int connect4_get_score(const board_t board);
/**
 *  function that get next possible move
 *  start to search move from move_id.
 */
int connect4_get_next_possible_move(board_t board, int player, int * move_id);
/**
 *  function that returns new board according to move
 * 	if status != NULL, return winner (Player or 0 if tie)
 * 	or 2 if no need in winner message
 * 	return -1 if failed, 0 if game over, 1 otherwise
 */
int connect4_make_move(board_t board, int player, int move, int moves_num, int* status);
/* function that return game properties */
properties_t* connect4_get_prop();
/* function to init game */
board_t connect4_init_game();
/* function to draw game */
int connect4_build_board(board_t board, control_s* panel, int screen_width, int screen_height, int (*action)(control_s* control));
/* function that free all game resources */
int connect4_free_game_resources();

#endif /* _CONNECT4_H */
