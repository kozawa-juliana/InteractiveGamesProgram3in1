
#ifndef _IMAGES_MANAGE_H
#define _IMAGES_MANAGE_H

#include "game_files.h"
#include "board_game.h"

#define WIN_HEIGHT 642
#define WIN_WIDTH	877
#define BUTTON_WIDTH	194
#define BUTTON_HEIGHT	61
#define LEFT_FRAME_W	235

/* menu images */
image_s new_game_im, load_game_im, quit_im, main_menu_im, players_op_im[4], restart_im, save_im,
	difficulty1_im, difficulty2_im, difficulty_im[10], pause_im, unpause_im, left_frame_im, right_frame_im,
	slots_im[NUMBER_OF_SLOTS], error_im, win_mess, turn_pass_im;

/* init button image according to his place */
int init_button_image(image_s* image, int x, int y);
/* init images file */
int init_images();

/* init images to all menus functions: */
int init_main_menu_images();
int init_error_images();
int init_game_menu_images();
int init_choose_game_images();
int init_choose_players_images();
int init_choose_diff_images();
int init_choose_slot_images();
int init_winner_message(int winner);
int init_turn_pass_message(int player);

/* free menu images */
void free_images();
/* free all images */
void free_all_images();
/* free all resources */
void image_manager_free_all();

#endif /* IMAGES_MANAGE_H_ */
