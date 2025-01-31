#include "high-score.h"
#include "global.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int displayHighScore(const char *file_name, const char *seperator)
{
	FILE *file = fopen(file_name, "r");
	if (file == NULL)
	{
		return stateError;
	}

	// Count the number of lines in the file
	int count = 0;
	char c;
	while (!feof(file))
	{
		c = fgetc(file);
		if (c == '\n')
		{
			count++;
		}
	}

	// Allocate memory for players and scores
	char **players = glb_malloc(count * sizeof(char *), "displayHighScore()");
	char **scores = glb_malloc(count * sizeof(char *), "displayHighScore()");

	// Reset file pointer to the beginning of the file
	rewind(file);

	// Read the entire file
	int i = 0;
	char line[256];
	while (fgets(line, sizeof(line), file))
	{
		// Ignore comments and blank lines
		if (line[0] == '#' || line[0] == '\n')
			continue;

		// Parse key-value pairs
		char *player = strtok(line, seperator);
		char *score = strtok(NULL, seperator);
		players[i] = glb_malloc(sizeof(char *) * (strlen(player) + 1), "displayHighScore()");
		strcpy(players[i], player);

		scores[i] = glb_malloc(sizeof(char *) * (strlen(score) + 1), "displayHighScore()");
		strcpy(scores[i], score);
		i++;
	}

	// Close the file
	fclose(file);

	int ch;
	while (true)
	{

		// Display the high scores
		clear();
		mvprintw(1, (COLS - strlen(TITLE_HIGH_SCORE)) / 2, TITLE_HIGH_SCORE);
		for (int i = 0; i < count; ++i)
		{
			mvprintw(3 + i, (COLS - strlen(players[i]) - strlen(scores[i])) / 2, "%s %s", players[i], scores[i]);
		}

		refresh();

		ch = getch();
		if (ch == 27)
		{
			for (int j = 0; j < i; j++)
			{
				free(players[j]);
				free(scores[j]);
			}
			free(players);
			free(scores);
			return stateMainMenu;
		}
	}
}

void saveHighScore(char *player_name, int score)
{
	FILE *file = fopen(HIGH_SCORE_FILE, "r+");
	if (!file)
	{
		file = fopen(HIGH_SCORE_FILE, "w");
		if (!file)
		{
			glb_handleError("Failed to open high-score file");
			return;
		}
	}

	char line[MAX_PLAYER_NAME_LENGTH+5], name[MAX_PLAYER_NAME_LENGTH];
	int high_score;
	long pos;
	int found = 0;

	while (fgets(line, sizeof(line), file))
	{
		pos = ftell(file);
		sscanf(line, "%[^=]=%d", name, &high_score);
		if (strcmp(name, player_name) == 0)
		{
			found = 1;
			if (score > high_score)
			{
				fseek(file, pos - strlen(line), SEEK_SET);
				fprintf(file, "%s=%d\n", player_name, score);
			}
			break;
		}
	}

	if (!found)
	{
		fprintf(file, "%s=%d\n", player_name, score);
	}

	fclose(file);
}
