#include "reversi.h"


#define REVERSI_BUTTON_SIZE	76
int reversi_difficulty[] = {1,2,3,4,0};
int winner_status;
char* reversi_name= "Reversi";
image_s reversi_images[4] = {NULL};
SDL_Surface *reversi_buttons;

/* init game images
 * return -1 on error
 */
int reversi_init_images(){
	reversi_buttons = load_img("images/REV.bmp");
	img_s img;
	img = init_image(REVERSI_BUTTON_SIZE*1,REVERSI_BUTTON_SIZE*0,REVERSI_BUTTON_SIZE,REVERSI_BUTTON_SIZE);
	reversi_images[0] = create_image(reversi_buttons, img, img);
	img = init_image(REVERSI_BUTTON_SIZE*0,REVERSI_BUTTON_SIZE*0,REVERSI_BUTTON_SIZE,REVERSI_BUTTON_SIZE);
	reversi_images[1] = create_image(reversi_buttons, img, img);
	img = init_image(REVERSI_BUTTON_SIZE*1,REVERSI_BUTTON_SIZE*1,REVERSI_BUTTON_SIZE,REVERSI_BUTTON_SIZE);
	reversi_images[2] = create_image(reversi_buttons, img, img);
	return 0;
}

/* return game properties, NULL on error */
properties_t* reversi_get_prop()
{
	properties_t* prop;
	if((prop = (properties_t*)calloc(1,sizeof(properties_t))) == NULL){
		printf(ALLOC_ERROR);
		return NULL;
	}

	/* init button image */
	prop->name = reversi_name;
	if(init_button_image(&prop->game_button, 1, 2)< 0)
		return NULL;
	/* fill struct data */
	prop->difficulty = reversi_difficulty;
	prop->board_width =REVERSI_BOARD_WIDTH;
	prop->board_height = REVERSI_BOARD_HEIGHT;
	prop->init_game = &reversi_init_game;
	prop->build_board = &reversi_build_board;
	prop->make_move = &reversi_make_move;
	prop->get_score = &reversi_get_score;
	prop->get_next_possible_move = &reversi_get_next_possible_move;
	prop->free_game_resources = &reversi_free_game_resources;

	return prop;
}


/* check if there are any changes in a score spans after the move
 * update board if change_board is 1 */
int reversi_check_span_changes(board_t board, int row, int col, int up, int right, int player, int change_board){
	int last_player_col = col,
			last_player_row = row,
			cur_col = col,
			cur_row = row,
			changed = 0;
	row += up;
	col += right;
	/* check if move is in board's boundaries */
	if(row >= REVERSI_BOARD_HEIGHT || row <0 || col >= REVERSI_BOARD_WIDTH || col <0 || board[row][col] != -1*player)
		return 0;
	/* check if span changed */
	while(row < REVERSI_BOARD_HEIGHT && row >=0 && col < REVERSI_BOARD_WIDTH && col >=0 && board[row][col] != 0){
		if(board[row][col] == player){
			last_player_col = col;
			last_player_row = row;
			break;
		}
		if(board[row][col] != player){
			changed = 1;
		}
		col += right;
		row += up;
	}
	if( (last_player_col == cur_col && last_player_row == cur_row) || !changed)
		return 0;
	/* update board if needed */
	if(change_board){
		row = cur_row;
		col = cur_col;
		while( row != last_player_row || col != last_player_col){
			board[row][col] = player;
			row += up;
			col += right;
		}
	}
	return 1;
}

/* check if curent move is legal */
int reversi_is_valid_move(board_t board, int row, int col, int player, int change_board){
	int i,j, return_value = 0;
	/* check if move is in board's boundaries */
	if(row >= REVERSI_BOARD_HEIGHT || row <0 || col >= REVERSI_BOARD_WIDTH || col <0 || board[row][col] != 0)
		return 0;
	for(i=-1; i<2; i++){
		for(j =-1; j<2; j++){
			if(i == 0 && j == 0)
				continue;
			/* if one of spans changed, move is legal */
			if(reversi_check_span_changes(board, row, col, i, j, player, change_board))
				return_value = 1;
		}
	}
	return return_value;
}

/* get next possible move */
int reversi_calc_next_possible_move(board_t board, int player, int * move_id, int change){
	*move_id = *move_id + 1;
	for(; *move_id< REVERSI_BOARD_WIDTH*REVERSI_BOARD_HEIGHT; *move_id = *move_id + 1){
		if(reversi_is_valid_move(board,*move_id/REVERSI_BOARD_WIDTH, *move_id %REVERSI_BOARD_WIDTH, player, 0)){
			/* if valid move, changed board if needed and return */
			reversi_is_valid_move(board,*move_id/REVERSI_BOARD_WIDTH, *move_id %REVERSI_BOARD_WIDTH, player, change);
			return 0;
		}
	}
	return -1;
}

/* divide board into regions */
int reversi_get_region(int row, int col){
	/* return a region accordingly to score calculation rules */
	if((row == 0 || row == REVERSI_BOARD_HEIGHT -1) && (col == 0 || col == REVERSI_BOARD_WIDTH -1))
		return 5;
	if((row <=1 || row >= REVERSI_BOARD_HEIGHT -2) && (col <= 0 || col >= REVERSI_BOARD_WIDTH -2))
		return 4;
	if((row == 0 || row == REVERSI_BOARD_HEIGHT -1) || (col == 0 || col == REVERSI_BOARD_WIDTH -1))
		return 3;
	if((row <=1 || row >= REVERSI_BOARD_HEIGHT -2) || (col <= 0 || col >= REVERSI_BOARD_WIDTH -2))
		return 2;
	return 1;
}

/* count discs on board */
int reversi_count_disc(board_t board){
	int i, j, count;
	for(i=0; i< REVERSI_BOARD_HEIGHT; i++){
		for(j=0; j<REVERSI_BOARD_WIDTH; j++){
			count += board[i][j];
		}
	}
	/* return win player */
	if(count < 0)
		return -1;
	if(count > 0)
		return 1;
	/* tie */
	return 0;
}

/* calculate score of the board */
int reversi_get_score(const board_t board)
{
	int weight_vector[] = {1,-1, 5, -5, 10};
	int cur_row,
		cur_col,
		poss_move = -1,
		total_score;
		winner_status = 2;
	if( reversi_calc_next_possible_move(board, PLAYER1, &poss_move, 0) == -1){
		poss_move =-1;
		if( reversi_calc_next_possible_move(board, PLAYER2, &poss_move, 0) == -1){
				/* no possible moves to both players. winner is player with most discs */
				winner_status = reversi_count_disc(board);
				return (winner_status)*WIN_SCORE;
			}
	}
	for (cur_row = 0; cur_row < REVERSI_BOARD_HEIGHT; cur_row++)
	{
		for (cur_col = 0; cur_col < REVERSI_BOARD_WIDTH; cur_col++)
		{
			/* total score is the sum of cell value * weight vector */
			total_score += board[cur_row][cur_col] * weight_vector[reversi_get_region(cur_row, cur_col)-1];
		}
	}

	return total_score;
}

/* get next possible move */
int reversi_get_next_possible_move(board_t board, int player, int * move_id){
	return reversi_calc_next_possible_move(board, player, move_id, 1);
}

/*
 * make player move
 * return -1 if failed, 0 if game over, 1 otherwise, 2 if next player is player
 */
int reversi_make_move(board_t board, int player, int move, int moves_num, int* status)
{
	int move_id;
	move_id = move-2;
	/* check if move is legal */
	if(reversi_calc_next_possible_move(board, player, &move_id,0) <0 || move_id != move-1){
		return -1;		
	}
	move = move -2;
	reversi_get_next_possible_move(board, player, &move);
	if(reversi_get_score(board) == WIN_SCORE*player || winner_status != 2){
		/* game is over, player won */
		if(player == PLAYER1){
			printf("Game over: player1 wins\n");
		}
		else{
			printf("Game over: player2 win\n");
		}
		if(status != NULL)
			*status = winner_status;
		return 0;
	}
	if(moves_num >= (REVERSI_BOARD_HEIGHT*REVERSI_BOARD_WIDTH-1)){
		/* board is full */
		printf("Game over: board is full\n");
		if(status != NULL)
			*status = 0;
		return 0;
	}
	move = -1;
	if(reversi_calc_next_possible_move(board, -1*player, &move, 0)<0)
		return 2;
	return 1;
}

/* init game images and board */
board_t reversi_init_game(){
	board_t board = init_board(REVERSI_BOARD_HEIGHT, REVERSI_BOARD_WIDTH);
	if(board == NULL)
		return NULL;
	if(reversi_init_images() <0)
		return NULL;
	/* set start position */
	board[REVERSI_BOARD_HEIGHT/2][REVERSI_BOARD_WIDTH/2] = PLAYER1;
	board[REVERSI_BOARD_HEIGHT/2 - 1][REVERSI_BOARD_WIDTH/2 - 1] = PLAYER1;
	board[REVERSI_BOARD_HEIGHT/2][REVERSI_BOARD_WIDTH/2 - 1] = PLAYER2;
	board[REVERSI_BOARD_HEIGHT/2 - 1][REVERSI_BOARD_WIDTH/2] = PLAYER2;
	return board;
}

/* build game tree */
int reversi_build_board(board_t board, control_s* panel, int screen_width, int screen_height, int (*action)(control_s* control)){
	/* build game tree */
	return build_game_area(board, panel, REVERSI_BOARD_WIDTH, REVERSI_BOARD_HEIGHT, screen_width, screen_height, reversi_images, action);
}

/* free all resources */
int reversi_free_game_resources(){
	int i;
	for(i = 0; i< 4; i++){ /* free images */
		if(reversi_images[i] != NULL)
		free_image(reversi_images[i]);
	}
	if( reversi_buttons != NULL){ /* free buttons surface */
		free_img_file(reversi_buttons);
		reversi_buttons = NULL;
	}
	return 0;
}
