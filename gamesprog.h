
#ifndef _GAMESPROG_H
#define _GAMESPROG_H

#include <math.h>
#include <stdio.h>
#include "gui.h"
#include "board_game.h"
#include "tic_tac_toe.h"
#include "connect4.h"
#include "reversi.h"
#include "game_files.h"
#include "images_manage.h"

/* number of games */
#define GAMES_NUM 3
#define IS_PLAYER_NUM_COMP(player)	(players & (player==1?2:1))
#define IS_PLAYER_COMP	IS_PLAYER_NUM_COMP(player)

/* number of AI players */
#define COMP_NUM	((players & 1) + ((players & 2)>>1))

/* player difficulty */
#define DIFFICULTY	((player==PLAYER1)?(player1_difficulty):(player2_difficulty))
#define NORMAL_QUIT	GUI_QUIT

/* build main menu */
int build_main_menu();
/* make move */
int make_move(int move);
/* make move buttons CB */
int make_move_button(control_s* con);
/* build game UI tree */
int build_game_tree();
/* build and display choose players menu */
int choose_players_menu(int game);

#endif /* _GAMESPROG_H */
