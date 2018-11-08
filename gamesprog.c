#include "gamesprog.h"


control_s *window;


/* games properties array */
properties_t* games[GAMES_NUM];
int current_game=-1, game_over = 0;
/* players is bit vector - 1 if player is AI */
int players;
/* current player */
int player;
int moves_num;
/* game board */
board_t board;
/* difficulty of each player */
int player1_difficulty;
int player2_difficulty;
/* game area UI tree */
control_s* game_area;
/* 1 if game load from file */
int loaded_game = 0;
/* 1 if game is in pause */
int pause = 0;

/* count moves num */
void count_moves_num(){
	int i,j;
	moves_num = 0;
	for(i= 0; i < games[current_game]->board_height; i++){
		for(j=0; j < games[current_game]->board_width; j++){
			/* all move add disc */
			if(board[i][j] != 0)
				moves_num++;
		}
	}
}

/* get games properties according to game name */
properties_t * get_prop_by_name(char* name){
	int i;
	for( i = 0; i< GAMES_NUM; i++)
		if(strcmp(name, games[i]->name) == 0){
			current_game = i;
			return games[i];
		}
	return NULL;
}

/* clean game resources */
void clean_game_resources(){
	/* free board */
	if(board != NULL){
		free_board(board, games[current_game]->board_height);
		board = NULL;
	}
}

/* restart game button CB */
int restart_game_button(control_s * con){
	clean_game_resources();
	loaded_game = 0;
	pause = 0;
	return choose_players_menu(current_game);
	
}

/* free UI tree and menu images */
void free_gui_resources(){
	free_tree(window);
	free_images();
}

/* main menu button CB */
int main_menu_button(control_s * con){
	/* free last menu resources */
	free_gui_resources();
	/* nullfy all game variable */
	loaded_game = 0;
	pause = 0;
	if(current_game != -1)
		clean_game_resources();
	current_game = -1;
	/* build and display main menu */
	return build_main_menu();
}

/* free all games properties structs */
void free_games_struct(){
	int i;
	/* for all game */
	for(i=0; i< GAMES_NUM; i++){
		if(games[i] != NULL){
			/* free struct */
			free_image(games[i]->game_button);
			free(games[i]);
		}
	}
}

/* init all game properties */
int init_games_struct(){
	/* NEW GAME NEED TO BE ADDED HERE */
	if((games[0] = ttt_get_prop()) == NULL)
		return -1;
	if((games[1] = connect4_get_prop()) == NULL)
		return -1;
	if((games[2] = reversi_get_prop()) == NULL)
		return -1;
	return 0;
}

/* quit button CB */
int quit_button(control_s * con){
	/* return negative number to stop game while */
	return NORMAL_QUIT;
}

/* check if comp turn, and execute */
int check_comp_move(){
	int move;
	if(!IS_PLAYER_COMP){
		return 0;
	}
	/* player move. set difficulty */
	number_steps = DIFFICULTY;
	/* run minimax algorithm */
	if((move = minimax_get_best_move(board, player)) < 0)
		return 0; 
	/* make move */
	if(make_move(move) <0)
		return -1;
	return 0;
}

/* show error message */
int show_error(int (*func)(control_s * con)){
	control_s *dialog_frame, *button1;

	/* init images, frames */
	if(init_error_images()<0)
		return -1;
	if((dialog_frame= create_panel(2*WIN_WIDTH/6, WIN_HEIGHT/6, BUTTON_WIDTH*2 , (4*BUTTON_HEIGHT+10), right_frame_im)) == NULL)
		return -1;
	if(add_child(window, dialog_frame)<0)
		return -1;
	/* add button */
	button1 = create_button(BUTTON_WIDTH/2, 5 + (BUTTON_HEIGHT+10) , BUTTON_WIDTH, BUTTON_HEIGHT, error_im, func, -2);
	if(button1 == NULL)
		return -1;
	if(add_child(dialog_frame, button1) < 0)
		return -1;

	/* display */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}
	return 0;
}

/* choose slot buttons CB */
int choose_slot_button(control_s * con){
	int score;
	char name[NUMBER_OF_SLOTS+6];
	if(con->id < 0){
		/* load game */
		sprintf(name,"slot%d",con->id*-1);
		/* lad game */
		if((board = load_game(name, &get_prop_by_name, &player)) == NULL){
			/* error in loading file */
			return show_error(&main_menu_button);
		} else {
			/* load successed */
			score = games[current_game]->get_score(board);
			game_over =0;
			/* check if gameover */ 
			if(score == WIN_SCORE || score == -1*WIN_SCORE)
				game_over = 1;
			count_moves_num();
			loaded_game = 1;
		}
		/* choose players menu */
		return choose_players_menu(current_game);
	}else{
		/* save game */
		sprintf(name,"slot%d",con->id);
		/* try to save */
		if(save_game(board, games[current_game], name, player) < 0){
			printf(FILE_SAVE_ERROR);
			if(show_error(&make_move_button) < 0)
				return -1;
		}
		/* build and display menu */
		return make_move(-1);
	}
	
}

/* save and load buttons CB */
int save_load_button(control_s * con){
	int i;
	control_s *dialog_frame, *button1;

	/* if load, pause game */
	if(con->id < 0)
		pause =1; /* load game */
	/* init images */
	if(init_choose_slot_images()<0)
		return -1;
	/* init frames */
	dialog_frame= create_panel(WIN_WIDTH/6, WIN_HEIGHT/6, BUTTON_WIDTH*2 , (NUMBER_OF_SLOTS + 1)*(2*BUTTON_HEIGHT+10), right_frame_im);
	if(dialog_frame == NULL)
		return -1;
	if(add_child(window, dialog_frame)<0)
		return -1;
	/* add slots options */
	for(i =1; i<=NUMBER_OF_SLOTS; i++){
		button1 = create_button(BUTTON_WIDTH/2, 5 + i*(BUTTON_HEIGHT+10) ,
				 BUTTON_WIDTH, BUTTON_HEIGHT, slots_im[i -1], &choose_slot_button, con->id * i);
		if(button1 == NULL)
			return -1;
		if(add_child(dialog_frame, button1) < 0)
			return -1;
	}
	/* set buttons CB */
	if(con->id < 0)
		/* load */
		button1 = create_button(BUTTON_WIDTH/2, 5 + NUMBER_OF_SLOTS*(BUTTON_HEIGHT+10) ,
			 BUTTON_WIDTH, BUTTON_HEIGHT, quit_im,&main_menu_button , -2);
	else
		/* save */
		button1 = create_button(BUTTON_WIDTH/2, 5 + NUMBER_OF_SLOTS*(BUTTON_HEIGHT+10) ,
			 BUTTON_WIDTH, BUTTON_HEIGHT, quit_im, &make_move_button , -2);
		
	if(button1 == NULL)
		return -1;
	if(add_child(dialog_frame, button1) < 0)
		return -1;
	/* display tree */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}
	return 0;
}

/* choose difficulty buttons CB */
int choose_difficulty_button(control_s * con){
	/* change difficulty to player (id = player * difficulty) */
	if(con->id < 0)
		player2_difficulty = -1*con->id;
	else
		player1_difficulty = con->id;
	return make_move(-1);
}

/* show pass turn message */
int show_pass_message(control_s* frame){
	control_s *button1;

	/* add button */
	button1 = create_button(WIN_WIDTH/6, WIN_HEIGHT/6,
 			323, 211, turn_pass_im, &make_move_button, -2);
	if(button1 == NULL)
		return -1;	
	if(add_child(frame, button1) < 0)
		return -1;
	return 0;
}

/* difficulty button CB */
int difficulty_button(control_s * con){
	int i;
	control_s *dialog_frame, *button1;

	pause =1;
	
	/* init images */
	if(init_choose_diff_images() < 0)
		return -1;
	/* count difficulty levels */
	for(i =0; games[current_game]->difficulty[i] != 0; i++);
	/* build frame */
	if((dialog_frame= create_panel(WIN_WIDTH/6, WIN_HEIGHT/6, BUTTON_WIDTH*2 , i*(2*BUTTON_HEIGHT+10), right_frame_im)) == NULL)
		return -1;
	if(add_child(window, dialog_frame)<0)
		return -1;
	/* add buttons */
	for(i =0; games[current_game]->difficulty[i] != 0; i++){
		/* id = player * level */
		button1 = create_button(BUTTON_WIDTH/2, 5 + i*(BUTTON_HEIGHT+10) ,
 			BUTTON_WIDTH, BUTTON_HEIGHT, difficulty_im[games[current_game]->difficulty[i] -1], 
			&choose_difficulty_button, con->id * games[current_game]->difficulty[i]);
		if(button1 == NULL)
			return -1;	
		if(add_child(dialog_frame, button1) < 0)
			return -1;
	}
	/* display menu */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}
	return 0;
}

/* unpause button CB */
int unpause_button(control_s * con){
	/* unpause, build menu */
	pause = 0;
	make_move(-1);
	/* check if comp turn */
	return check_comp_move();
}

/* build and display game menu */
int build_game_menu(control_s* game_area){
	control_s *left_frame, *button1, *mess;
	int offset;
	
	/* create window, frames */
	if((window = create_window(WIN_WIDTH, WIN_HEIGHT)) == NULL)
		return -1;
	if((left_frame= create_panel(0, 0, LEFT_FRAME_W, WIN_HEIGHT, left_frame_im)) == NULL)
		return -1;
	if(add_child(window, game_area)<0)
		return -1;
	if(add_child(window, left_frame)<0)
		return -1;
	offset = (WIN_HEIGHT - (7 * (BUTTON_HEIGHT + 10)))/2;
	/* create buttons */
	button1 = create_button(25, offset , BUTTON_WIDTH, BUTTON_HEIGHT, restart_im, &restart_game_button,0);
	if(button1 == NULL)
		return -1;	
	if(add_child(left_frame, button1) < 0)
		return -1;
	button1 = create_button(25, offset + (BUTTON_HEIGHT+10) , BUTTON_WIDTH, BUTTON_HEIGHT, save_im, &save_load_button,1);
	if(button1 == NULL)
		return -1;	
	if(add_child(left_frame, button1) < 0)
		return -1;
	button1 = create_button(25, offset+(BUTTON_HEIGHT+10)*2 , BUTTON_WIDTH, BUTTON_HEIGHT, main_menu_im, &main_menu_button, 0);
	if(button1 == NULL)
		return -1;	
	if(add_child(left_frame, button1) < 0)
		return -1;
	if(pause){
		/* if pause, create unpause button */
		button1 = create_button(25,offset+(BUTTON_HEIGHT+10)*3, BUTTON_WIDTH, BUTTON_HEIGHT, unpause_im, 		
			&unpause_button ,0);
		if(button1 == NULL)
			return -1;	
		if(add_child(left_frame, button1) < 0)
			return -1;
	}
	/* create difficulty button to each AI */
	if(IS_PLAYER_NUM_COMP(PLAYER1)){
		button1 = create_button(25, offset + (BUTTON_HEIGHT+10)*4 , BUTTON_WIDTH, BUTTON_HEIGHT, difficulty1_im,
			&difficulty_button,PLAYER1);
		if(button1 == NULL)
			return -1;	
		if(add_child(left_frame, button1) < 0)
			return -1;
	}
	if(IS_PLAYER_NUM_COMP(PLAYER2)){
		button1 = create_button(25, offset + (BUTTON_HEIGHT+10)*5 , BUTTON_WIDTH, BUTTON_HEIGHT, difficulty2_im,
			&difficulty_button,PLAYER2);
		if(button1 == NULL)
			return -1;	
		if(add_child(left_frame, button1) < 0)
			return -1;
	}
	/* quit button */
	button1 = create_button(25, offset + (BUTTON_HEIGHT+10)*6 , BUTTON_WIDTH, BUTTON_HEIGHT, quit_im, &quit_button,0);
	if(button1 == NULL)
		return -1;	
	if(add_child(left_frame, button1) < 0)
		return -1;
	/* display win message if need */
	if(game_over && win_mess != NULL){
		if((mess= create_panel((WIN_WIDTH - 413)/2,(WIN_HEIGHT - 266) /2, 413, 266, win_mess)) == NULL)
				return -1;
		if(add_child(window, mess)<0)
			return -1;
	}
	/* display menu */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}
	return 0;
}

/**
 * make game move
 * if move < 0, just build UI
 */
int make_move(int move){
	control_s *right_frame;
	int result = -1, status;
	/* if move >= 0, make move */
	if(move >=0 ){
		if(game_over)
			return 0;
		if((result = games[current_game]->make_move(board, player,move, moves_num, &status)) == -1){
			printf("ERROR: unallow move %d\n", move);
			return 0;
		}
	}
	/* free UI - no need to free image, same image use in next menu*/
	free_tree(window);
	/* build frames */
	if((right_frame= create_panel(LEFT_FRAME_W, 0, WIN_HEIGHT, WIN_HEIGHT, right_frame_im)) == NULL)
		return -1;
	if(games[current_game]->build_board(board, right_frame, WIN_HEIGHT, WIN_HEIGHT, &make_move_button)<0)
		return -1;
	if(result == 0){
		/* game over */
		printf("game over\n");
		/* if need to display win message */
		if(status != 2)
			init_winner_message(status);
		game_over =1;
		/* game over */
	}else if(move >=0) {
		/* if turn move to next player */
		if( result != 2)
			player = player * -1;
		else {
			/* player has no legal move */
			if(init_turn_pass_message(player) <0)
				return -1;
			if(show_pass_message(right_frame) < 0)
				return -1;
		}
		moves_num++;
		/* if AI vs AI */
		if(COMP_NUM == 2)
			pause = 1;
	}
	/* build and display game menu */
	if(build_game_menu(right_frame)<0)
		return -1;
	return 0;
}


/* make move CB */
int make_move_button(control_s* con){
	int rv;
	/* if pause or COMP turn, button is disable */
	if((IS_PLAYER_COMP|| pause) && con->id !=-2)
		return 0;
	/* make move accorrding to button id */
	if(make_move(con->id + 1)< 0)
		return -1;
	/* if COMP turn, make move */
	rv = check_comp_move();
	return rv;
}

/* build game UI */
int build_game_tree(){
	board_t game_board;
	control_s *right_frame;

	/* free last menu resources */
	free_gui_resources();
	/* init images */
	if(init_game_menu_images()<0)
		return -1;
	/* init game */
	if((game_board = games[current_game]->init_game()) == NULL)
		return -1;
	/* set difficulty to default */
	player1_difficulty = games[current_game]->difficulty[0];
	player2_difficulty = games[current_game]->difficulty[0];
	/* set minimax */
	minimax_start_game(games[current_game]);
	if(loaded_game){
		/* if game loaded - board is already init */
		free_board(game_board, games[current_game]->board_height);
		loaded_game = 0;
	} else {
		/* start game */
		board = game_board;
		player = PLAYER1;
		moves_num =0;
		game_over = 0;
	}
	
	/* if first player is comp - pause */
	if(IS_PLAYER_COMP)
		pause = 1;
	
	/* build frames */
	if((right_frame= create_panel(LEFT_FRAME_W, 0, WIN_HEIGHT, WIN_HEIGHT, right_frame_im)) == NULL)
		return -1;
	
	game_area= right_frame;

	/* build game area UI tree */
	if(games[current_game]->build_board(board, right_frame, WIN_HEIGHT, WIN_HEIGHT, &make_move_button)<0)
		return -1;
	/* build menu UI tree */
	if(build_game_menu(right_frame)<0)
		return -1;
	/* display menu */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}

	return 0;

}

/* choose players button CB */
int choose_players_button(control_s * con){
	players = con->id;
	/* build game UI */
	if(build_game_tree()<0)
		return -1;
	return 0;
}

/* build choose players menu (AI or player) */
int choose_players_menu(int game){
	control_s
		*left_frame,
		*right_frame,
		*button1;
	int i, offset;
	
	/* set current game */
	current_game = game;
	/* free last menu gui resources */
	free_gui_resources();
	/* init images, frames */
	if(init_choose_players_images()<0)
		return -1;
	if((window = create_window(WIN_WIDTH, WIN_HEIGHT)) == NULL)
		return -1;
	if((left_frame= create_panel(0, 0, LEFT_FRAME_W, WIN_HEIGHT, left_frame_im)) == NULL)
		return -1;
	if((right_frame = create_panel(LEFT_FRAME_W, 0, WIN_HEIGHT, WIN_HEIGHT, right_frame_im)) == NULL)
		return -1;
	if(add_child(window, right_frame)<0)
		return -1;
	if(add_child(window, left_frame)<0)
		return -1;
	offset = (WIN_HEIGHT - 5 * (BUTTON_HEIGHT + 10))/2;
	/* init buttons */
	for(i =0; i< 4; i++){
		button1 = create_button(25, offset + 70*i, BUTTON_WIDTH, BUTTON_HEIGHT, players_op_im[i],
			 &choose_players_button, i);
		if(button1 == NULL)
			return -1;
		if(add_child(left_frame, button1) < 0)
			return -1;
	}
	button1 = create_button(25, offset + 70 * 4 , BUTTON_WIDTH, BUTTON_HEIGHT, main_menu_im, &main_menu_button,0);
	if(button1 == NULL)
		return -1;	
	if(add_child(left_frame, button1) < 0)
		return -1;

	/* display tree */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}
	return 0;

}

/* choose game button CB (called when user choose game) */
int choose_game_button(control_s * con){
	return choose_players_menu(con->id);
}

/* new game nutton CB */
int new_game_button(control_s * con){
	control_s
	*left_frame,
	*right_frame,
	*button1;
	int i, offset;

	/* free last menu gui resources */
	free_gui_resources();
	/* init images */
	if(init_choose_game_images()<0)
		return -1;
	/* create windows, labels */
	if((window = create_window(WIN_WIDTH, WIN_HEIGHT)) == NULL)
		return -1;
	if((left_frame= create_panel(0, 0, LEFT_FRAME_W, WIN_HEIGHT, left_frame_im)) == NULL)
		return -1;
	if((right_frame = create_panel(LEFT_FRAME_W, 0, WIN_HEIGHT, WIN_HEIGHT, right_frame_im)) == NULL)
		return -1;
	if(add_child(window, right_frame)<0)
		return -1;
	if(add_child(window, left_frame)<0)
		return -1;
	/* add buttons */
	offset = (WIN_HEIGHT - (GAMES_NUM + 1) * (BUTTON_HEIGHT + 10))/2;
	for(i =0; i< GAMES_NUM; i++){
		button1 = create_button(25, offset + 70*i, BUTTON_WIDTH, BUTTON_HEIGHT, games[i]->game_button, 		
			&choose_game_button, i);
		if(button1 == NULL)
			return -1;
		if(add_child(left_frame, button1) < 0)
			return -1;
	}
	button1 = create_button(25, offset + 70 * GAMES_NUM , BUTTON_WIDTH, BUTTON_HEIGHT, main_menu_im, &main_menu_button,0);
	if( button1 == NULL)
		return -1;
	if(add_child(left_frame, button1)<0)
		return -1;
	/* display menu */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;//on error
	}
	return 0;
}

/* build main menu */
int build_main_menu(){ 
	control_s
	*left_frame,
	*right_frame,
	*button1,
	*button2,
	*button3;
	int offset;

	/* init images  and add labels*/
	if(init_main_menu_images()<0)
		return -1;
	current_game = -1;
	if((window = create_window(WIN_WIDTH, WIN_HEIGHT)) == NULL)
		return -1;
	if((left_frame= create_panel(0, 0, LEFT_FRAME_W, WIN_HEIGHT, left_frame_im)) == NULL)
		return -1;
	if((right_frame = create_panel(LEFT_FRAME_W, 0, WIN_HEIGHT, WIN_HEIGHT, right_frame_im)) == NULL)
		return -1 ;
	if(add_child(window, right_frame)<0)
		return -1;
	if(add_child(window, left_frame)< 0)
		return -1;
	/* make all buttons */
	offset = (WIN_HEIGHT - 3* (BUTTON_HEIGHT + 10))/2;
	if((button1 = create_button(25, offset, BUTTON_WIDTH, BUTTON_HEIGHT, new_game_im, &new_game_button,0)) == NULL)
		return -1;
	if((button2 = create_button(25, offset + 70, BUTTON_WIDTH, BUTTON_HEIGHT,load_game_im, &save_load_button,-1)) == NULL)
		return -1;
	if((button3 = create_button(25, offset + 2*70 , BUTTON_WIDTH, BUTTON_HEIGHT, quit_im, &quit_button,0)) == NULL)
		return -1;
	if(add_child(left_frame, button1)<0)
		return -1;
	if(add_child(left_frame, button2)<0)
		return -1;
	if(add_child(left_frame, button3)<0)
		return -1;
	/* display menu */
	if(display_tree(window, "Games") <0)
	{
		printf(GUI_DISPLAY_ERROR);
		return -1;
	}
	return 0;
}
int main(int argc, char *argv[])
{
	int done = 0, rt,i;

	/* init images files */
	if( init_images()< 0){
		image_manager_free_all();
		return -1;
	}

	/* init games properties */
	if(init_games_struct() < 0){
		image_manager_free_all();
		free_games_struct();
		return -1;
	}

	/* init gui */
	if(gui_init() <0){
		image_manager_free_all();
		free_games_struct();
		gui_free();
		return -1;
	}
	
	/* build and display main menu */
	if(build_main_menu() < 0){
		image_manager_free_all();
		free_games_struct();
		gui_free();
		if(window != NULL)
			free_tree(window);
		return -1;
	}

	/* handle events - game while */
	while(!done)
	{
		if((rt = handle_events(window, "Games")) < 0){
			/* quit- check if error or normal */
			if(rt == NORMAL_QUIT){
				break;
			}
			if(show_error(quit_button)< 0)
				break;
		}
	}
	/* free all resouces */
	if(current_game != -1)
		clean_game_resources();
	for(i = 0; i< GAMES_NUM; i++){
		games[i]->free_game_resources();	
	}
	image_manager_free_all();
	free_games_struct();
	gui_free();
	if(window != NULL)
		free_tree(window);
	return 0;
}
