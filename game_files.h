
#ifndef _GAME_FILES_H
#define _GAME_FILES_H

#include "board_game.h"

#define NUMBER_OF_SLOTS	5

/* save game to file. return -1 on error */
int save_game(board_t board, properties_t* prop, char* name, int player);
/* check if file can be open */
int check_file(char* name);
/**
 * try to load game.
 * get_prop - function that get game name
 * and returns game properties.
 *
 * return board, or NULL on error.
 */
board_t load_game(char* name, properties_t* (*get_prop)(char* game), int *player);

#endif /* _GAME_FILES_H */
