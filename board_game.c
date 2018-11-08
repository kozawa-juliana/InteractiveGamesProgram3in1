#include <string.h>
#include <math.h>
#include <stdio.h>
#include "board_game.h"





/* free board memory */
void free_board(board_t board, int board_height)
{
	int i;
	if(board == NULL)
		return;
	for( i=0 ; i<board_height ; i++ )
		free(board[i]);
	board = NULL;
	free(board);
}

/* alloc memory for board, and copy */
board_t copy_board(board_t board, int board_height, int board_width)
{
	int i,j;
	board_t copy;
	if((copy = init_board(board_height, board_width)) == NULL)
		return NULL; /* return NULL on error */
	for( i=0 ; i<board_height ; i++ ){
		for( j=0 ; j<board_width ; j++ ){
			copy[i][j] = board[i][j];
		}
	}
	return copy;
}

/* alloc memory for board, return NULL if failed */
board_t init_board(int board_height, int board_width)
{
	int i;
	board_t board;
	if((board = (board_t)calloc(board_height, sizeof(int *))) == NULL){
		printf(ALLOC_ERROR);
		return NULL;
	}
	for( i=0 ; i<board_height ; i++ ){
		if((board[i] = (int *)calloc(board_width, sizeof(int)))== NULL){
			printf(ALLOC_ERROR);
			free_board(board, board_height);
			return NULL;
		}
	}
	return board;
}
/* Drawing board acorrding to board */
int build_game_area(board_t board, control_s* panel, int board_width, int board_height, int screen_width, int screen_height, image_s images[3], int (*action)(control_s* control)){
	int i, j,
		button_x,
		button_y,
		button_width,
		button_height;
	control_s *button;
	/* create button for each squre in board */
	for( i =0; i< board_height; i++){
		for(j=0; j< board_width; j++){
			button_x = j*(screen_width/board_width)+2;
			button_y = screen_height - ((i+1)*(screen_height/board_height)+2);
			button_width = (screen_width/board_width)-4;
			button_height = (screen_height/board_height)-4;
			if((button = create_button(button_x, button_y, button_width, button_height, images
				[board[i][j]+1], action, (i*board_width + j))) == NULL){
				/* free all on error and return -1 */
				free_board(board, board_height);
				free_tree(panel);
				return -1;
			}
			if(add_child(panel, button) < 0){
				/* free all on error and return -1 */
				free_board(board, board_height);
				free_tree(panel);
				return -1;
			}
			
		}
	}
	return 0;
}
