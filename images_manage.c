#include "images_manage.h"

SDL_Surface *buttons;
SDL_Surface *left;
SDL_Surface *right;
SDL_Surface *wins;
SDL_Surface *pass;

/* images files */
char *but_img = "images/buts.bmp";
char *bground1 = "images/b111.bmp";
char *bground11 = "images/b12.bmp";
char *winners = "images/win.bmp";
char *turnover = "images/turnover.bmp";

/* init button image from buttons surface */
int init_button_image(image_s* image, int x, int y){
	img_s img1, img2;
	if((img1 = init_image(x*BUTTON_WIDTH, 2*y*(BUTTON_HEIGHT+1), BUTTON_WIDTH, BUTTON_HEIGHT)) ==NULL){
		return -1;
	}
	if((img2 = init_image(x*BUTTON_WIDTH,(2*y+1)*(BUTTON_HEIGHT+1), BUTTON_WIDTH, BUTTON_HEIGHT)) ==NULL){
		return -1;
	}
	*image = create_image(buttons, img1, img2);
	if(image == NULL)
		return -1;
	return 0;
}

/* init files, left and right frame images */
int init_images(){
	img_s img;
	buttons = load_img(but_img); /* load button image */
	left = load_img(bground1); /* load left background image */
	right = load_img(bground11);/* load right background image */
	wins = load_img(winners);/* load winners messages image */
	pass = load_img(turnover);/* load pass turn messages image */
	if (left == NULL || right == NULL || buttons == NULL || wins == NULL)
	{
		printf(IMAGE_FILE_ERROR);
		return -1;
	}
	if((img = init_image(0, 0, 235, WIN_HEIGHT)) == NULL)
		return -1;;
	left_frame_im = create_image(left, img, img);
	if(left_frame_im == NULL)
		return -1;
	if((img = init_image(0, 0, WIN_HEIGHT, WIN_HEIGHT)) == NULL)
		return -1;
	right_frame_im =  create_image(right, img, img);
	if(right_frame_im == NULL)
		return -1;
	return 0;
}

/* init main menu images */
int init_main_menu_images(){
	if(init_button_image(&new_game_im, 0, 0) < 0)
		return -1;
	if(init_button_image(&load_game_im, 0, 1) < 0)
		return -1;
	if(init_button_image(&quit_im, 0, 2) < 0)
		return -1;
	return 0;
}
/* init error message */
int init_error_images(){
	if(init_button_image(&error_im, 8, 0) < 0)
		return -1;
	return 0;
}

/* init game menu images */
int init_game_menu_images(){
	if(init_button_image(&main_menu_im, 1, 3) < 0)
		return -1;
	if(init_button_image(&restart_im, 3, 0) < 0)
		return -1;
	if(init_button_image(&save_im, 3, 1) < 0)
		return -1;
	if(init_button_image(&difficulty1_im, 3, 5) < 0)
		return -1;
	if(init_button_image(&difficulty2_im, 3, 6) < 0)
		return -1;
	if(init_button_image(&pause_im, 3, 3) < 0)
		return -1;
	if(init_button_image(&unpause_im, 3, 4) < 0)
		return -1;
	if(init_button_image(&quit_im, 0, 2) < 0)
		return -1;
	return 0;
}

/* init choose game menu images */
int init_choose_game_images(){
	if(init_button_image(&main_menu_im, 1, 3) < 0)
		return -1;
	return 0;
}

/* init choose player menu images */
int init_choose_players_images(){
	int i;
	if(init_button_image(&main_menu_im, 1, 3) < 0)
		return -1;
	for(i=0; i<4; i++){
		if(init_button_image(&(players_op_im[i]), 2, i) < 0)
			return -1;
	}
	return 0;
}

/* init difficulty images */
int init_choose_diff_images(){
	int i;
	for(i=0; i<10; i++){
		if(init_button_image(&(difficulty_im[i]), 4, i) < 0)
			return -1;
	}
	return 0;
}

/* init files slots images */
int init_choose_slot_images(){
	int i, row;
	char name[NUMBER_OF_SLOTS + 6];
	for(i=0; i<NUMBER_OF_SLOTS; i++){
		sprintf(name,"slot%d",i+1);
		row = 5 + check_file(name);
		if(init_button_image(&(slots_im[i]), row, i) < 0)
			return -1;
	}
	if(init_button_image(&quit_im, 0, 2) < 0)
		return -1;
	return 0;
}

/*free images */
void free_images(){
	int i;
	if(new_game_im != NULL){
			free_image(new_game_im);
			new_game_im = NULL;
	}
	if(load_game_im != NULL){
			free_image(load_game_im);
			load_game_im = NULL;
	}
	if(quit_im != NULL){
			free_image(quit_im);
			quit_im = NULL;
	}
	if(main_menu_im != NULL){
			free_image(main_menu_im);
			main_menu_im = NULL;
	}
	if(restart_im != NULL){
			free_image(restart_im);
			restart_im = NULL;
	}
	if(save_im != NULL){
			free_image(save_im);
			save_im = NULL;
	}
	if(difficulty1_im != NULL){
			free_image(difficulty1_im);
			difficulty1_im = NULL;
	}
	if(difficulty2_im != NULL){
			free_image(difficulty2_im);
			difficulty2_im = NULL;
	}
	if(pause_im != NULL){
			free_image(pause_im);
			pause_im = NULL;
	}
	if(unpause_im != NULL){
			free_image(unpause_im);
			unpause_im = NULL;
	}
	if(win_mess != NULL){
			free_image(win_mess);
			win_mess = NULL;
	}
	if(turn_pass_im != NULL){
			free_image(turn_pass_im);
			turn_pass_im = NULL;
	}
	for(i=0; i< 4; i++)
		if(players_op_im[i] != NULL){
			free_image(players_op_im[i]);
			players_op_im[i] = NULL;
		}
	for(i=0; i< 10; i++)
		if(difficulty_im[i] != NULL){
			free_image(difficulty_im[i]);
			difficulty_im[i] = NULL;
		}
	for(i=0; i< NUMBER_OF_SLOTS; i++)
		if(slots_im[i] != NULL){
			free_image(slots_im[i]);
			slots_im[i] = NULL;
		}
}

/* init winner message image according to winner */
int init_winner_message(int winner){
	img_s img1;
	int col;
	if(winner == PLAYER1)
		col = 0;
	else if(winner == PLAYER2)
		col = 1;
	else
		col = 2;
	if((img1 = init_image(0, col * 266 , 413, 266)) ==NULL){
		return -1;
	}

	win_mess = create_image(wins, img1, img1);
	if(win_mess == NULL)
		return -1;
	return 0;
}

/* init "turn pass" message according to player */
int init_turn_pass_message(int player){
	img_s img1;
	int col;
	if(player == PLAYER1)
		col = 1;
	else
		col = 0;

	if((img1 = init_image(0, col * 211 , 323, 211)) ==NULL){
		return -1;
	}

	turn_pass_im = create_image(pass, img1, img1);
	if(turn_pass_im == NULL)
		return -1;
	return 0;
}

/* free images including left and right frames */
void free_all_images(){
	free_images();
	if(left_frame_im != NULL)
		free_image(left_frame_im);
	if(right_frame_im != NULL)
			free_image(right_frame_im);
}

/* free all resources (images, surface) */
void image_manager_free_all(){
	free_all_images();
	free_img_file(buttons);
	free_img_file(left);
	free_img_file(right);
	free_img_file(wins);
	free_img_file(pass);
}
