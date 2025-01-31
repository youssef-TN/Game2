#include "menu.h"
#include "global.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <libconfig.h>

int general_menu(int type, const char *menu_array[], int array_count, const char *title)
{
	FILE *file;
	int difficulty_index;
	int *key_binds_values;
	config_t cfg;
	switch (type)
	{
	case stateDifficultyMenu:
		config_init(&cfg);
		difficulty_index = getDifficulty();
		if (difficulty_index == stateError)
		{
			glb_handleError("Failed load Difficulty from config file");
			return stateError;
		}

	case stateKeyBindsMenu:
		config_init(&cfg);
		key_binds_values = getKeyBinds();
		if (key_binds_values == NULL)
		{
			glb_handleError("Failed load key binds from config file");
			return stateError;
		}
		break;
	default:
		break;
	}
	int choice = 0;
	int ch;
	while (true)
	{
		clear();
		mvprintw(1, (COLS - strlen(title)) / 2, "%s", title);

		// Print menu items with highlighting
		for (int i = 0; i < array_count; ++i)
		{
			if (i == choice)
			{
				attron(COLOR_PAIR(MENU_COLOR)); // Highlight selected option
				display(type, i, difficulty_index, menu_array, key_binds_values);
				attroff(COLOR_PAIR(MENU_COLOR));
			}
			else
			{
				display(type, i, difficulty_index, menu_array, key_binds_values);
			}
		}

		refresh();

		ch = getch();
		switch (ch)
		{
		case 27: // ASCII value for ESC key
			switch (type)
			{
			case stateDifficultyMenu:
			case stateKeyBindsMenu:
				return stateSettingsMenu;
			case stateSettingsMenu:
				return stateMainMenu;
			default:
				return stateExit;
			}
		case KEY_UP:
			choice = (choice - 1 + array_count) % array_count;
			break;
		case KEY_DOWN:
			choice = (choice + 1) % array_count;
			break;
		case '\n': // Enter key
			switch (type)
			{
			case stateDifficultyMenu:
				return updateConfig(type, K_DIFFICULTY, choice);
			case stateKeyBindsMenu:
				return updateKeybind(menu_array[choice], menu_array, &key_binds_values, array_count);
			default:
				if (strcmp(menu_array[choice], K_PLAY) == 0)
					return statePlay;
				else if (strcmp(menu_array[choice], K_SETTINGS) == 0)
					return stateSettingsMenu;
				else if (strcmp(menu_array[choice], K_DIFFICULTY) == 0)
					return stateDifficultyMenu;
				else if (strcmp(menu_array[choice], K_KEY_BINDS) == 0)
					return stateKeyBindsMenu;
				else if (strcmp(menu_array[choice], K_HIGH_SCORE) == 0)
					return stateHighScore;
				else
					return stateExit;
			}
		default:
			break;
		}
	}
}

int updateKeybind(const char *key_name, const char *menu_array[], int **key_binds_values, int count_array)
{
	while (true)
	{
		clear();

		mvprintw(1, (COLS - strlen(TITLE_KEY_BINDS_MENU)) / 2, "%s", TITLE_KEY_BINDS_MENU);
		mvprintw(3, (COLS - strlen("Press a key to bind to") - strlen(key_name)) / 2, "Press a key to bind to %s", key_name);

		refresh();

		bool key_already_bound = false;
		int ch = getch();
		if (ch == 27)
		{
			return stateKeyBindsMenu;
		}
		else
		{
			for (int i = 0; i < count_array; ++i)
			{
				if (ch == (*key_binds_values)[i])
				{
					mvprintw(5, (COLS - strlen("Key already bound to another action")) / 2, "Key already bound to another action");
					refresh();
					getch();
					key_already_bound = true;
					break;
				}
			}
			// Update the keybind array
			if (key_already_bound == false)
			{
				for (int i = 0; i < count_array; ++i)
				{
					if (strcmp(menu_array[i], key_name) == 0)
					{
						(*key_binds_values)[i] = ch;
						break;
					}
				}
				return updateConfig(stateKeyBindsMenu, key_name, ch);
			}
		}
	};
}

int updateConfig(int type, const char *key, int value)
{
	config_t cfg;
	config_init(&cfg);
	if (!config_read_file(&cfg, CONFIG_FILE))
	{
		char error_message[256];
		snprintf(error_message, sizeof(error_message), "Error reading config file at line %d: %s. Location updateConfig() \n",
				 config_error_line(&cfg), config_error_text(&cfg));
		glb_handleError(error_message);
		config_destroy(&cfg);
		return stateError;
	}
	config_setting_t *config_value = config_lookup(&cfg, key);
	if (config_value)
	{
		config_setting_set_int(config_value, value);

		if (!config_write_file(&cfg, CONFIG_FILE))
		{
			glb_handleError("Error writing to config file.\n");
			config_destroy(&cfg);
			return stateError;
		}
	}
	else
	{
		return stateError;
	}
	config_destroy(&cfg);
	return type;
}

void display(int type, int i, int selected_difficulty, const char *menu_array[], int *key_binds_values)
{
	switch (type)
	{
	case stateDifficultyMenu:
		if (i != selected_difficulty)
		{
			mvprintw(3 + i, (COLS - strlen(menu_array[i])) / 2, "%s", menu_array[i]);
		}
		else
		{
			mvprintw(3 + i, (COLS - strlen(menu_array[i])) / 2, "%s (Selected)", menu_array[i]);
		}
		break;
	case stateKeyBindsMenu:
		mvprintw(3 + i, (COLS - strlen(menu_array[i]) - strlen(keyname(key_binds_values[i])) - 4) / 2, "%s -> %s", menu_array[i], keyname(key_binds_values[i]));
		break;
	default:
		mvprintw(3 + i, (COLS - strlen(menu_array[i])) / 2, "%s", menu_array[i]);
		break;
	}
}

int getDifficulty()
{
	int difficulty_index;
	glb_getConfigureValue(K_DIFFICULTY, &difficulty_index);
	if (&difficulty_index == NULL)
		return stateError;
	return difficulty_index;
}

int *getKeyBinds()
{
	int *key_binds_values = glb_malloc(COUNT_KEY_BINDS_MENU * sizeof(int), "getKeyBinds()");
	if (key_binds_values == NULL)
	{
		glb_handleError("Memory allocation failed in getKeyBinds() -> key_binds_values");
		return NULL;
	}

	glb_getConfigureValues(KEY_BINDS_MENU_ARRAY, &key_binds_values, COUNT_KEY_BINDS_MENU);
	if (key_binds_values == NULL)
	{
		return NULL;
	}
	return key_binds_values;
}