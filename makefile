
C_FLAGS=-I. -I./SDL_ttf -Wall -g -std=c99 -pedantic-errors -lm `sdl-config --cflags`
O_FILES= minimax.o gui.o board_game.o tic_tac_toe.o connect4.o reversi.o gamesprog.o images_manage.o game_files.o

all: gamesprog

clean:
	rm -rf gamesprog $(O_FILES)

gamesprog: $(O_FILES)
	gcc $(CFLAGS) -o $@ $^ `sdl-config --libs` -L./SDL_ttf -lSDL_ttf

minimax.o: minimax.c
	gcc -c $(C_FLAGS) minimax.c

gamesprog.o: gamesprog.c
	gcc -c $(C_FLAGS) gamesprog.c

board_game.o: board_game.c
	gcc -c $(C_FLAGS) board_game.c

connect4.o: connect4.c
	gcc -c $(C_FLAGS) connect4.c

tic_tac_toe.o: tic_tac_toe.c
	gcc -c $(C_FLAGS) tic_tac_toe.c

reversi.o: reversi.c
	gcc -c $(C_FLAGS) reversi.c

gui.o: gui.c
	gcc -c $(C_FLAGS) gui.c

images_manage.o: images_manage.c
	gcc -c $(C_FLAGS) images_manage.c

game_files.o: game_files.c
	gcc -c $(C_FLAGS) game_files.c


