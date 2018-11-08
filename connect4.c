
#include "connect4.h"

#define CONNECT4_BUTTONS_WIDTH	88
#define CONNECT4_BUTTONS_HEIGHT	103
#define PLAYER_IMAGE(x)	((x)<0?0:1)

int connect4_difficulty[] = {1,2,3,4,5,6,7,0};
char* connect4_name= "Connect4";
image_s connect4_images[4] = {NULL};
SDL_Surface *connect4_buttons;

/* init game images
 * return -1 on error
 */
int connect4_init_images(){
	img_s img;
	connect4_buttons = load_img("images/C4.bmp");
	img = init_image(CONNECT4_BUTTONS_WIDTH*1,CONNECT4_BUTTONS_HEIGHT*0,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
	connect4_images[0] = create_image(connect4_buttons, img, img);
	img = init_image(CONNECT4_BUTTONS_WIDTH*0,CONNECT4_BUTTONS_HEIGHT*0,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
	connect4_images[1] = create_image(connect4_buttons, img, img);
	img = init_image(CONNECT4_BUTTONS_WIDTH*1,CONNECT4_BUTTONS_HEIGHT*1,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
	connect4_images[2] = create_image(connect4_buttons, img, img);
	return 0;
}

/* return game properties, NULL on error */
properties_t* connect4_get_prop()
{
	properties_t* prop;
	if((prop = (properties_t*)calloc(1,sizeof(properties_t))) == NULL){
		printf(ALLOC_ERROR);
		return NULL;
	}
	/* init button image */
	prop->name = connect4_name;
	if(init_button_image(&prop->game_button, 1, 1)< 0)
		return NULL;
	/* fill struct data */
	prop->difficulty = connect4_difficulty;
	prop->board_width =CONNECT4_BOARD_WIDTH;
	prop->board_height = CONNECT4_BOARD_HEIGHT;
	prop->init_game = &connect4_init_game;
	prop->build_board = &connect4_build_board;
	prop->make_move = &connect4_make_move;
	prop->get_score = &connect4_get_score;
	prop->get_next_possible_move = &connect4_get_next_possible_move;
	prop->free_game_resources = &connect4_free_game_resources;

	return prop;
}

/**
 *  calculate score for each span
 *  if chang!=0, change win span to 2
 */
int connect4_get_span_score(board_t board, int * spans, int row, int column, int up, int right, int change)
{
	int i,count =0;
	for (i=0; i<4; i++){
		/* calculate score for current span */
		count = count + board[row + i*up][column + i*right];
	}
	if (count == 4 || count == -4){
		if(change){
			for (i=0; i<4; i++){
				/* put 2 in winning span in case of victory */
				board[row + i*up][column + i*right] = 2;
			}
		}
		/* return win score */
		return (count/4)*WIN_SCORE;
	}
	spans[count + 3] ++;
	return 0;
}

/**
 *  calculate score of the board
 *  if change != 0, change winning span to 2
 *  and set image of span to be with line
 */
int connect4_get_score_display(board_t board, int change)
{
	int array_of_spans[7] = {0};
	int weight_vector[] = {-5, -2, -1, 0, 1, 2, 5};
	img_s img;
	int cur_row,
		cur_col,
		image_h,
		image_w,
		span_score,
		total_score = 0,
		i;
	for (cur_row = 0; cur_row < CONNECT4_BOARD_HEIGHT; cur_row++)
	{
		for (cur_col = 0; cur_col < CONNECT4_BOARD_WIDTH; cur_col++)
		{
			/* row span */
			if(cur_col <= CONNECT4_BOARD_WIDTH - 4){
				if((span_score = connect4_get_span_score(board, array_of_spans, cur_row, cur_col, 0, 1, change)) !=0){
					/* check if span change is needed */
					if(change){
						image_h = CONNECT4_BUTTONS_HEIGHT*PLAYER_IMAGE(span_score);
						image_w = CONNECT4_BUTTONS_WIDTH*2;
						/* init image with row line */
						img = init_image(image_w,image_h,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
						connect4_images[3] = create_image(connect4_buttons, img, img);
					}
					return span_score;
				}
			}
			/* column span */
			if(cur_row <= CONNECT4_BOARD_HEIGHT - 4){
				if((span_score = connect4_get_span_score(board, array_of_spans, cur_row, cur_col, 1, 0, change)) !=0){
					if(change){
						image_h = CONNECT4_BUTTONS_HEIGHT*PLAYER_IMAGE(span_score);
						image_w = CONNECT4_BUTTONS_WIDTH*3;
						/* init image with column line */
						img = init_image(image_w,image_h,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
						connect4_images[3] = create_image(connect4_buttons, img, img);
					}
					return span_score;
				}
			}
			/* diagonal spans */
			if(cur_row <= CONNECT4_BOARD_HEIGHT - 4 && cur_col <= CONNECT4_BOARD_WIDTH - 4){
				if((span_score = connect4_get_span_score(board, array_of_spans, cur_row, cur_col, 1, 1, change)) !=0){
					if(change){
						image_h = CONNECT4_BUTTONS_HEIGHT*PLAYER_IMAGE(span_score);
						image_w = CONNECT4_BUTTONS_WIDTH*5;
						/* init image with column line */
						img = init_image(image_w,image_h,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
						connect4_images[3] = create_image(connect4_buttons, img, img);
					}
					return span_score;
				}
			}
			if(cur_row <= CONNECT4_BOARD_HEIGHT - 4 && cur_col >= 3){
				if((span_score = connect4_get_span_score(board, array_of_spans, cur_row, cur_col, 1, -1, change)) !=0){
					if(change){
						image_h = CONNECT4_BUTTONS_HEIGHT*PLAYER_IMAGE(span_score);
						image_w = CONNECT4_BUTTONS_WIDTH*4;
						/* init image with column line */
						img = init_image(image_w,image_h,CONNECT4_BUTTONS_WIDTH,CONNECT4_BUTTONS_HEIGHT);
						connect4_images[3] = create_image(connect4_buttons, img, img);
					}
					return span_score;
				}
			}
		}
	}
	/* multiplay with weight vector */
	for (i = 0; i < 7; i++)
		total_score += array_of_spans[i] * weight_vector[i];
	return total_score;
}

/* calculate score of the board */
int connect4_get_score(const board_t board)
{
	return connect4_get_score_display(board, 0);
}

/*
 * copy the board and add player disc to move column
 * return NULL if failed, old board if move is illegal
 */
board_t connect4_create_move_board(const board_t board, int player, int move)
{
	board_t new_board;
	int row, r, c;
	row = CONNECT4_BOARD_HEIGHT -1;
	if (board[row][move-1] != 0)
		return board; /* move is illegal */
	/* find first empty cell in move column */
	while (row >= 0 && board[row][move-1] == 0)
		row --;
	row++;
	/* init new board */
	if((new_board = init_board(CONNECT4_BOARD_HEIGHT, CONNECT4_BOARD_WIDTH)) == NULL)
		return NULL;
	/* copy board */
	for (r = 0; r < CONNECT4_BOARD_HEIGHT; r++)
		for (c = 0; c < CONNECT4_BOARD_WIDTH; c++)
			new_board[r][c] = board[r][c];
	/* add new disc */
	new_board[row][move -1] = player;
	return new_board;
}

/* get next possible move */
int connect4_get_next_possible_move(board_t board, int player, int * move_id){
	int row;
	*move_id = *move_id + 1;
	for(; *move_id< CONNECT4_BOARD_WIDTH; *move_id = *move_id + 1){
		row = CONNECT4_BOARD_HEIGHT -1;
		if (board[row][*move_id] != 0)
			continue; /* move is illegal */
		/* find first empty cell in move column */
		while (row >= 0 && board[row][*move_id] == 0)
			row --;
		row ++;
		/* add new disc */
		board[row][*move_id] = player;
		return 0;
	}
	return -1;
}

/* delete last move (according to move id) */
int connect4_delete_last_move(board_t board, int  move_id){
	int row;
	row = CONNECT4_BOARD_HEIGHT -1;
	/* find first empty cell in move column */
	if(move_id < 0 || move_id >= CONNECT4_BOARD_WIDTH)
		return 0;
	while (row >= 0 && board[row][move_id] == 0)
		row --;
	/* add new disc */
	board[row][move_id] = 0;
	return 0;
}

/**
 *  function that returns new board according to move
 * 	if status != NULL, return winner (Player or 0 if tie)
 * 	or 2 if no need in winner message
 * 	return -1 if failed, 0 if game over, 1 otherwise
 */
int connect4_make_move(board_t board, int player, int move, int moves_num,int* status)
{
	move = ((move - 1) % CONNECT4_BOARD_WIDTH) +1;
	int poss_move = move -2;
	connect4_get_next_possible_move(board, player, &poss_move);
	if(poss_move != move-1){
		connect4_delete_last_move(board, poss_move);
		return -1;
	}
	/* check if player wins */
	if(connect4_get_score(board) == WIN_SCORE*player){
		/* game is over, player won */
		if(player == PLAYER1){
			printf("Game over: player1 wins\n");
			if(status != NULL)
				*status = 2;
		}
		else{
			printf("Game over: player2 win\n");
			if(status != NULL)
				*status = 2;
		}
		return 0;
	}
	if(moves_num >= (CONNECT4_BOARD_HEIGHT*CONNECT4_BOARD_WIDTH-1)){
		/* board is full */
		printf("Game over: board is full\n");
		if(status != NULL)
			*status = 0;
		return 0;
	}
	return 1;
}

/* init game images and board */
board_t connect4_init_game(){
	if(connect4_init_images() <0)
		return NULL;
	return init_board(CONNECT4_BOARD_HEIGHT, CONNECT4_BOARD_WIDTH);
}

/* check and mark winning span */ 
board_t connect4_check_win(board_t board){
	board_t display_board;
	/* copy board to preserve data */
	if((display_board = copy_board(board, CONNECT4_BOARD_HEIGHT, CONNECT4_BOARD_WIDTH)) == NULL)
		return NULL;
	/* check & mark win span */
	connect4_get_score_display(display_board, 1);
	return display_board;
	
}

/* build game tree */
int connect4_build_board(board_t board, control_s* panel, int screen_width, int screen_height, int (*action)(control_s* control)){
	board_t display_board;
	int rv;
	if((display_board = connect4_check_win(board))== NULL) /* check and mark winning span */ 
		return -1;
	/* build game tree */
	rv = build_game_area(display_board, panel,
		CONNECT4_BOARD_WIDTH,CONNECT4_BOARD_HEIGHT, screen_width, screen_height, connect4_images, action);
	free_board(display_board, CONNECT4_BOARD_HEIGHT);
	return rv;
}

/* free all resources */
int connect4_free_game_resources(){
	int i;
	/* free images */
	for(i = 0; i< 4; i++){
		if(connect4_images[i] != NULL)
		free_image(connect4_images[i]);
	}
	/* free buttons surface */
		if( connect4_buttons != NULL){ 
		free_img_file(connect4_buttons);
		connect4_buttons = NULL;
	}
	return 0;
}
