#include "minimax.h"


#define BOARD_HEIGHT	(game_properties.board_height)
#define BOARD_WIDTH		(game_properties.board_width)

int number_steps = -1;
properties_t game_properties;
/* get game properties */
void minimax_start_game(properties game_pro)
{
	game_properties = *game_pro;
}

/* calculate best move, return -1 if failed */
int get_best_move_recursive(board_t board, int player, int left_depth, int * score, int beta)
{
	board_t old_board;
	int i,j;
	if((old_board = init_board(BOARD_HEIGHT,BOARD_WIDTH)) == NULL)
		return -2;
	for(i=0; i<BOARD_HEIGHT; i++)
		for(j=0; j<BOARD_WIDTH; j++)
			old_board[i][j]= board[i][j];
	int best_move, best_score, move_score, move_id = -1;
	/* score of leaf is game score, have no possible moves */
	if(left_depth ==0 ){
		*score = game_properties.get_score(board)*player;
		free_board(old_board, BOARD_HEIGHT);
		return -1;
	}
	best_score = WIN_SCORE*(-1);
	/* if player have no legal move */
	if(game_properties.get_next_possible_move(board, player, &move_id) == -1){
		if(get_best_move_recursive(board,-1*player, left_depth-1, &move_score, -1*best_score) == -2){
			free_board(old_board,BOARD_HEIGHT);
			return -2;
		}
		*score = -1*move_score;
		return -1;
	}
	best_move = move_id;
	/* for each possible move */
	do{
		/* calculate best move recursively */
		if(get_best_move_recursive(board,-1*player, left_depth-1, &move_score, -1*best_score) == -2){
			free_board(old_board,BOARD_HEIGHT);
			return -2;
		}
		/* max score of player_A is min score of player_B */
		int player_score = -1*move_score;
		/* if max score, replace best move */
		if(player_score > best_score){
			best_move = move_id;
			best_score = player_score;
		}
		/* copy board */
		for(i=0; i<BOARD_HEIGHT; i++)
			for(j=0; j<BOARD_WIDTH; j++)
				board[i][j]= old_board[i][j];
		if(best_score >= beta){
			*score = best_score;
			free_board(old_board,BOARD_HEIGHT);
			return best_move;
		}
	}while(game_properties.get_next_possible_move(board, player, &move_id) != -1);
	*score = best_score;
	free_board(old_board,BOARD_HEIGHT);
	return best_move;
}

/* return best move of user, -1 if failed */
int minimax_get_best_move(board_t board, int player)
{
	int score;
	return get_best_move_recursive(board, player, number_steps, &score ,WIN_SCORE) + 1;
}
