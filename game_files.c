#include "game_files.h"

#define MAX_NAME_LENGTH 100

/* save game. return -1 on error */
int save_game(board_t board, properties_t* prop, char* name, int player){
	FILE * slot;
	int i,j;
	/* try to open file */
	if((slot = fopen(name, "w")) == NULL)
		return -1;
	/* print name and player */
	fprintf(slot,"%s\n",prop->name);
	fprintf(slot,"%d\n",player);
	/* print board */
	for(i= prop->board_height; i>0; i--){
		for(j=0; j<prop->board_width; j++){
			fprintf(slot,"%d ", board[i-1][j]);
		}
		fprintf(slot,"\n");
	}
	/* close file */
	fclose(slot);
	return 0;
}

/* check if file can be open. return 1 if can, 0 otherwise */
int check_file(char* name){
	FILE * slot;
	if((slot = fopen(name, "r")) == NULL)
		return 0;
	fclose(slot);
	return 1;
}

/**
 * try to load game.
 * get_prop - function that get game name
 * and returns game properties.
 *
 * return board, or NULL on error.
 */
board_t load_game(char* name, properties_t* (*get_prop)(char* game), int *player){

	FILE *slot;
	board_t board;
	int i, j;
	char temp[MAX_NAME_LENGTH];
	char *temp_line;
	char *game_name;
	int line_num;
	char *line;
	char* delim = " ";
	char *token;
	properties_t* prop;
	int board_width;
	int board_height;
	int player_num;
	int value;

	/* open file */
	if((slot = fopen(name, "r")) == NULL)
	{
		return NULL;
	}
	/* read file name */
	if((line = fgets (temp, sizeof temp, slot))== NULL){
		printf(FILE_FORMAT_ERROR);
		return NULL;
	}
	game_name = line;
	game_name[strlen(game_name)-1] = '\0';
	/* get file properties */
	if((prop = get_prop(game_name)) == NULL){
		printf(FILE_FORMAT_ERROR);
		fclose(slot);
		return NULL;
	}
	board_width = prop->board_width;
	board_height = prop->board_height;
	/* init board */
	if((board = init_board(board_height, board_width)) == NULL)
	{
		printf(ALLOC_ERROR);
		fclose(slot);
		return NULL;
	}
	/* read player */
	if((line = fgets (temp, sizeof temp, slot))== NULL){
		printf(FILE_FORMAT_ERROR);
		free_board(board, board_height);
		fclose(slot);
		return NULL;
	}
	player_num = atoi(line);
	if(player_num != PLAYER1 && player_num != PLAYER2)
	{
		/* illegal number */
		free_board(board, board_height);
		printf(FILE_FORMAT_ERROR);
		fclose(slot);
		return NULL;
	}
	*player = player_num;
	line_num = 2;
	i = 0;
	j = 0;
	if((temp_line = (char*)malloc(board_width*5 * sizeof(char))) == NULL){
		printf(FILE_READ_ERROR);
		fclose(slot);
		free_board(board, board_height);
	}
	/* read board */
	while((line = fgets (temp_line, board_width*5, slot))!= NULL)
	{
		line[strlen(line)-1] = '\0';
		line_num++;
		token = strtok(line, delim);
		while( token != NULL )
		{
			if(i >= board_height) /* check number of rows */
			{
				/* too much rows */
				printf(FILE_FORMAT_ERROR);
				free_board(board, board_height);
				free(temp_line);
				fclose(slot);
				return NULL;
			}
			if(j >= board_width) /* check number of columns */
			{
				/* too much columns */
				printf(FILE_FORMAT_ERROR);
				free_board(board, board_height);
				free(temp_line);
				fclose(slot);
				return NULL;
			}
			value = atoi(token);
			if(value == PLAYER1 || value == PLAYER2 || value == 0) /*check cell value */
			{
				board[board_height - 1 - i][j] = value;
			}
			else
			{
				/* illegal player num */
				printf(FILE_FORMAT_ERROR);
				free_board(board, board_height);
				free(temp_line);
				fclose(slot);
				return NULL;
			}
			j++; /* next column */
			token = strtok(NULL, delim);
		}
		if (j < board_width){
			/* line is shorter than need */
			printf(FILE_FORMAT_ERROR);
			free_board(board, board_height);
			free(temp_line);
			fclose(slot);
			return NULL;
		}
		i++; /* next row */
		j = 0; /* restart columns */
	}
	/* check that read all rows */
	if (i < board_height){
		printf(FILE_FORMAT_ERROR);
		free_board(board, board_height);
		free(temp_line);
		fclose(slot);
		return NULL;
	}
	/* close all */
	fclose(slot);
	free(temp_line);
	return board;
}
