#include "main.h"
#include "random.h"
#include "global.h"
#include "menu.h"
#include "high-score.h"
#include "maze.h"
#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

int main()
{
	initscr();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	noecho();
	cbreak();

	// Initialize color
	if (has_colors())
	{
		start_color();
		init_pair(MENU_COLOR, COLOR_GREEN, COLOR_BLACK);
		init_pair(WALL_COLOR, COLOR_WHITE, COLOR_RED);
		init_pair(END_COLOR, COLOR_WHITE, COLOR_GREEN);
		init_pair(PASSAGE_COLOR, COLOR_BLACK, COLOR_WHITE);
		init_pair(PLAYER_COLOR, COLOR_WHITE, COLOR_BLUE);
		init_pair(PATH_COLOR, COLOR_WHITE, COLOR_RED);
		init_pair(SCORE_COLOR, COLOR_YELLOW, COLOR_BLACK);
	}

	int ch;
	int current_state = stateMainMenu;
	int cols = 0;
	int rows = 0;
	int difficulty_index = 0;
	int *key_binds_values = NULL;
	int word_count = 0;
	int count_selected_words = 0;
	int wall_count = 0;
	int path_length = 0;
	int step_count = 0;
	int count_guessed_words = 0;
	int guessing;
	char collected_word[MAX_WORD_LENGTH];
	char **words = NULL;
	char **selected_words = NULL;
	char *player_name = NULL;
	char **guessed_words = NULL;
	Node **grid = NULL;
	Node *player_node = NULL;
	Node *end_node = NULL;
	Node **shortestPath = NULL;
	while (true)
		switch (current_state)
		{
		case stateMainMenu:
			current_state = general_menu(stateMainMenu, MAIN_MENU_ARRAY, COUNT_MAIN_MENU, TITLE_MAIN_MENU);
			break;
		case stateHighScore:
			current_state = displayHighScore(HIGH_SCORE_FILE, "=");
			break;
		case stateSettingsMenu:
			current_state = general_menu(stateSettingsMenu, SETTINGS_MENU_ARRAY, COUNT_SETTINGS_MENU, TITLE_SETTINGS_MENU);
			break;
		case stateDifficultyMenu:
			current_state = general_menu(stateDifficultyMenu, DIFFICULTY_MENU_ARRAY, COUNT_DIFFICULTY_MENU, TITLE_DIFFICULTY_MENU);
			break;
		case stateKeyBindsMenu:
			current_state = general_menu(stateKeyBindsMenu, KEY_BINDS_MENU_ARRAY, COUNT_KEY_BINDS_MENU, TITLE_KEY_BINDS_MENU);
			break;
		case statePlay:
			difficulty_index = 0;
			key_binds_values = NULL;
			current_state = getDifficultyAndKeyBinds(&difficulty_index, &key_binds_values);
			if (current_state == stateError)
				break;

			cols = 0;
			rows = 0;
			current_state = initGame(&rows, &cols, difficulty_index, key_binds_values);
			if (current_state == stateError)
				break;

			setSeed(NULL);

			words = NULL;
			word_count = 0;
			current_state = getWords(&words, &word_count, difficulty_index);
			if (current_state == stateError)
				break;
			current_state = statePlayerName;
			break;
		case statePlayerName:
			player_name = NULL;
			current_state = playerNaming(&player_name);
			if (current_state == stateMainMenu || current_state == stateError)
				break;
			current_state = stateStartGame;
			break;
		case stateStartGame:
			current_state = resetGrid(&grid, rows, cols);
			current_state = addNeighbors(&grid, rows, cols);
			wall_count = 0;
			divide(&grid, 0, 0, cols, rows, choose_orientation(cols, rows), DIFFICULTY_SPACE[difficulty_index], &wall_count);
			end_node = NULL;
			end_node = spawnPosition(&grid, rows, cols, END_SYMBOL);
			if (end_node == NULL)
			{
				current_state = stateError;
				break;
			}
			selected_words = NULL;
			count_selected_words = 0;
			addWords(&grid, rows, cols, &selected_words, &count_selected_words, words, word_count, wall_count);
			player_node = NULL;
			player_node = spawnPosition(&grid, rows, cols, PASSAGE_SYMBOL);
			if (player_node == NULL)
			{
				current_state = stateError;
				break;
			}
			path_length = 0;
			current_state = resetVisitedGrid(&grid, rows, cols);
			shortestPath = findShortPath(grid, player_node, end_node, rows, cols, &path_length);
			current_state = resetVisitedGrid(&grid, rows, cols);
			current_state = statePlaying;
			break;
		case statePlaying:
			step_count = 0;
			count_guessed_words = 0;
			guessing = 0;
			guessed_words = NULL;
			guessed_words = glb_malloc(sizeof(char*)*count_selected_words,"main()");
			for (int i = 0; i < strlen(collected_word); i++)
			{
				if (collected_word[i] == '\0')
					break;
				collected_word[i] = '\0';
			}
			while (true)
			{
				displayGrid(grid, player_node, rows, cols);
				ch = getch();
				if (ch == 27)
				{
					current_state = stateGameOver;
				}
				else if (ch == '\n')
				{
					if (!guessing)
					{
						guessing = START_WORD_COLLECTING;
						collected_word[0] = grid[player_node->row][player_node->col].character;
						grid[player_node->row][player_node->col].visited = NODE_USED;
						collected_word[1] = '\0';
					}
					else
					{
						if (strlen(collected_word) > DIFFICULTY_WORD[0])
						{
							printf("0 \n");
							guessing = isValidWord(collected_word, selected_words, count_selected_words);
							printf("0 \n");
							deleteStr(&selected_words, &count_selected_words, guessing);
							printf("0 \n");
							if (guessing != -1){
							printf("1 \n");
								guessed_words[count_guessed_words++] = collected_word;
							printf("1 \n");
							}
							guessing = END_WORD_COLLECTING;
							printf("0 \n");
							collected_word[0] = '\0';
							printf("0 \n");
						}
					}
				}
				else
				{
					playerMovement(&player_node, key_binds_values, ch, &step_count, collected_word);
				}
				current_state = gameOver(player_node, end_node);
				if (current_state == stateGameOver)
					break;
			}
			break;
		case stateGameOver:
			int score = calculateScore(step_count, path_length, NULL, 0);
			displayGameOverGrid(grid, shortestPath, path_length, rows, cols, score, player_name, DIFFICULTY_MENU_ARRAY[difficulty_index], step_count, count_guessed_words);
			saveHighScore(player_name, score);
			ch = getch();
			if (ch == 27)
			{
				current_state = stateError;
			}
			break;
		default:
			endwin();
			return 0;
		}
}
