# Interactive Games Program 3 in 1
Interactive Games Program with a Graphical User Interface (GUI) and AI game engine.

Game Program project is written in C with SDL library.\
It provides fully-functioning implementation of three interactive two-player board games:\
Connect Four, Reversi and Tic Tac Toe.\
Each game included in the project is a digital version of a well-known tabletop game.

User can choose a game to play, select the players participating in the game and set game difficulty level.\
Each player participating in the game can be set to user (human) or AI (computer).

The program consists of three two-player games with 'perfect information'.\
Therefore, each player is perfectly informed of all the events that have previously occurred in the game.\
AI is designed using Minimax algorithm.\
Same Minimax algorithm used for each game.\
Main difference between the different game engines is the scoring function.\
This function is specific to each game and its game state.

The program provides end user with Graphical User Interface (GUI).\
Menus and controls for each game are represented graphically on the scren.\
The user can interact with each game using keyboard or/and mouse.

Games Program designed to be generic and modular program.\
It implemented in such a way that games can be added and removed with minimal coding efforts beyond the game-specific logic.
