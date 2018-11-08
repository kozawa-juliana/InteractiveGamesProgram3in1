#include "tic_tac_toe.h"

#define TTT_BUTTON_SIZE	210
#define PLAYER_IMAGE(x)	((x)<0?1:0)
int ttt_difficulty[] = {9, 0};
char* ttt_name = "Tic_tac_toe";
image_s ttt_images[4] = {NULL};
SDL_Surface *ttt_buttons = NULL;

/* init game images
 * return -1 on error
 */
int ttt_init_images(){
	img_s img;
	if((ttt_buttons = load_img("images/TTT.bmp")) == NULL)
		return -1;
	if((img = init_image(TTT_BUTTON_SIZE*0,TTT_BUTTON_SIZE*1,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE)) == NULL)
		return -1;
	if((ttt_images[0] = create_image(ttt_buttons, img, img)) == NULL)
		return -1;
	if((img = init_image(TTT_BUTTON_SIZE*5,TTT_BUTTON_SIZE*1,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE)) == NULL)
			return -1;
	if((ttt_images[1] = create_image(ttt_buttons, img, img)) == NULL)
			return -1;
	if((img = init_image(TTT_BUTTON_SIZE*0,TTT_BUTTON_SIZE*0,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE)) == NULL)
			return -1;
	if((ttt_images[2] = create_image(ttt_buttons, img, img)) == NULL)
		return -1;
	return 0;
}

/* return game properties, NULL on error */
properties_t* ttt_get_prop()
{
	properties_t* prop;
	if((prop = (properties_t*)calloc(1,sizeof(properties_t))) == NULL){
		printf(ALLOC_ERROR);
		return NULL;
	}
	/* init button image */
	prop->name = ttt_name;
	if(init_button_image(&prop->game_button, 1, 0)< 0)
		return NULL;
	/* fill struct data */
	prop->difficulty = ttt_difficulty;
	prop->board_width =TTT_BOARD_WIDTH;
	prop->board_height = TTT_BOARD_HEIGHT;
	prop->init_game = &ttt_init_game;
	prop->build_board = &ttt_build_board;
	prop->make_move = &ttt_make_move;
	prop->get_score = &ttt_get_score;
	prop->get_next_possible_move = &ttt_get_next_possible_move;
	prop->free_game_resources = &ttt_free_game_resources;

	return prop;
}

/**
 *  calculate score for each span
 *  if change!=0, change win span to 2
 */
int ttt_get_span_score(board_t board, int row, int column, int up, int right, int change)
{
	int i,count =0;
	for (i=0; i<TTT_BOARD_WIDTH; i++){
	/* calculate score for current span */
		count = count + board[row + i*up][column + i*right];
	}
	if (count == TTT_BOARD_WIDTH || count == -1*TTT_BOARD_WIDTH){
		if(change){
			for (i=0; i<TTT_BOARD_WIDTH; i++){
			/* put 2 in winning span in case of victory */
				board[row + i*up][column + i*right] = 2;
			}
		}
		/* return win score */
		return (count/TTT_BOARD_WIDTH)*WIN_SCORE;
	}
	return 0;
}

/**
 *  calculate score of the board
 *  if change != 0, change winning span to 2
 *  and set image of span to be with line
 */
int ttt_get_score_display(const board_t board, int change)
{
	int	span_score,
		image_h,
		i;
	img_s img;
	for (i = 0; i < TTT_BOARD_WIDTH; i++)
	{	
		/* row span */		
		if((span_score = ttt_get_span_score(board, i, 0, 0, 1, change)) != 0){
			/* check if span change is needed */
			if(change){
				image_h = TTT_BUTTON_SIZE*PLAYER_IMAGE(span_score);
				/* init image with row line */
				img = init_image(TTT_BUTTON_SIZE*1,image_h,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE);
				ttt_images[3] = create_image(ttt_buttons, img, img);
			}
			return span_score;
		}
		/* column span */
		if((span_score = ttt_get_span_score(board, 0, i, 1, 0, change)) !=0){
			if(change){
				/* init image with column line */
				image_h = TTT_BUTTON_SIZE*PLAYER_IMAGE(span_score);
				img = init_image(TTT_BUTTON_SIZE*2,image_h,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE);
				ttt_images[3] = create_image(ttt_buttons, img, img);
			}
			return span_score;
		}
	}
	/* diagonal spans */
	if((span_score = ttt_get_span_score(board, 0, 0, 1, 1, change)) !=0){
		if(change){
			/* init image with diagonal line */
			image_h = TTT_BUTTON_SIZE*PLAYER_IMAGE(span_score);
			img = init_image(TTT_BUTTON_SIZE*4,image_h,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE);
			ttt_images[3] = create_image(ttt_buttons, img, img);
		}
		return span_score;
	}
	if((span_score = ttt_get_span_score(board, 0, TTT_BOARD_WIDTH-1, 1, -1, change)) !=0){
		if(change){
			/* init image with diagonal line */
			image_h = TTT_BUTTON_SIZE*PLAYER_IMAGE(span_score);
			img = init_image(TTT_BUTTON_SIZE*3,image_h,TTT_BUTTON_SIZE,TTT_BUTTON_SIZE);
			ttt_images[3] = create_image(ttt_buttons, img, img);
		}
		return span_score;	
	}
	return 0;
}
/* calculate score of the board */
int ttt_get_score(const board_t board){
	return ttt_get_score_display(board, 0);
}

/* get next possible move */
int ttt_get_next_possible_move(board_t board, int player, int * move_id){
	for(*move_id = *move_id + 1; *move_id< TTT_BOARD_WIDTH*TTT_BOARD_HEIGHT; *move_id = *move_id + 1){
		if(board[*move_id/TTT_BOARD_WIDTH][*move_id %TTT_BOARD_WIDTH] == 0){
			/* if empty, add player and return */
			board[*move_id/TTT_BOARD_WIDTH][*move_id %TTT_BOARD_WIDTH] = player;
			return 0;
		}
	}
	return -1;
}

/* delete last move (according to move id) */
int ttt_delete_last_move(board_t board, int  move_id){
	board[move_id/TTT_BOARD_WIDTH][move_id%TTT_BOARD_WIDTH] = 0;
	return 0;
}

/**
 *  function that returns new board according to move
 * 	if status != NULL, return winner (Player or 0 if tie)
 * 	or 2 if no need in winner message
 * 	return -1 if failed, 0 if game over, 1 otherwise
 */
int ttt_make_move(board_t board, int player, int move, int moves_num, int* status)
{
	if(board[(move-1)/TTT_BOARD_WIDTH][(move-1)%TTT_BOARD_WIDTH] != 0)
		return -1;
	board[(move-1)/TTT_BOARD_WIDTH][(move-1)%TTT_BOARD_WIDTH] = player;
	/* check if player wins */
	if(ttt_get_score(board) == WIN_SCORE*player){
		/* game is over, player won */
		if(player == PLAYER2){
			printf("Game over: player2 wins\n");
			if(status != NULL)
				*status = 2;
		} else {
			printf("Game over: player1 win\n");
			if(status != NULL)
				*status = 2;
		}
		return 0;
	}
	if(moves_num >= (TTT_BOARD_HEIGHT*TTT_BOARD_WIDTH-1)){
		/* board is full */
		printf("Game over: board is full\n");
		if(status != NULL)
			*status = 0;
		return 0;
	}
	return 1;
}

/* init game images and board */
board_t ttt_init_game(){
	if(ttt_init_images() <0)
		return NULL;
	return init_board(TTT_BOARD_HEIGHT, TTT_BOARD_WIDTH);
}

/* check and mark winning span */ 
board_t ttt_check_win(board_t board){
	board_t display_board;
	/* copy board to preserve data */
	if((display_board = copy_board(board, TTT_BOARD_HEIGHT, TTT_BOARD_WIDTH)) == NULL)
		return NULL;
	/* check & mark win span */
	ttt_get_score_display(display_board, 1);
	return display_board;
	
}

/* build game tree */
int ttt_build_board(board_t board, control_s* panel, int screen_width, int screen_height, int (*action)(control_s* control)){
	board_t display_board;
	int rv;
	/* check and mark winning span */
	if((display_board = ttt_check_win(board))== NULL)  
		return -1; 
	/* build game tree */
	rv = build_game_area(display_board, panel, TTT_BOARD_WIDTH, TTT_BOARD_HEIGHT, screen_width, screen_height, ttt_images, action); 
	free_board(display_board, TTT_BOARD_HEIGHT);
	return rv;
}

/* free all resources */
int ttt_free_game_resources(){
	int i;
	/* free images */
	for(i = 0; i < 4; i++){ 
		if(ttt_images[i] != NULL)
			free_image(ttt_images[i]);
	}
	/* free buttons surface */
	if( ttt_buttons != NULL){ 
		free_img_file(ttt_buttons);
		ttt_buttons = NULL;
	}
	return 0;
}
