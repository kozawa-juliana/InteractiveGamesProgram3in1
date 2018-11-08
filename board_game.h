#ifndef _BOARD_GAME_H
#define _BOARD_GAME_H

#include "gui.h"
#include "errors.h"

typedef int** board_t;

#define PLAYER2 -1
#define PLAYER1 1

typedef struct properties_s {
		/* game name */
		char * name;
		/* game button for main menu */
		image_s game_button;
		/* array with difficulty options */
		int* difficulty;
		/* board size */
		int board_width;
		int board_height;
		/* function to init game */
		board_t (*init_game)();
		/* function to draw game */
		int (*build_board)(board_t board, control_s* panel, 
			int screen_width, int screen_height, int (*action)(control_s* control));
		/**
		 *  function that returns new board according to move
		 * 	if status != NULL, return winner (Player or 0 if tie)
		 * 	or 2 if no need in winner message
		 * 	return -1 if failed, 0 if game over, 1 otherwise
		 */
		int (*make_move)(board_t board, int player, int move, int move_num, int* status);
		/* function that calculates board score */
		int (*get_score)(board_t board);
		/**
		 *  function that get next possible move
		 *  start to search move from move_id.
		 */
		int (*get_next_possible_move)(board_t board, int player, int * move_id);
		/* function that free all game resources */
		int (*free_game_resources)();
}properties_t;

typedef properties_t *  properties;

/* init board. return NULL on error */
board_t init_board(int board_height, int board_width);
/* free board */
void free_board(board_t board, int board_height);
/* build UI tree to game area. return <0 on error */
int build_game_area(board_t board, control_s* panel, int board_width, int board_height, int screen_width, int screen_height, image_s images[3], int (*action)(control_s* control));
/* init new board and copy board data to it. NULL on error */
board_t copy_board(board_t board, int board_height, int board_width);

#endif /* _BOARD_GAME_H */
