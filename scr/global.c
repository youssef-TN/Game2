#include "global.h"
#include "random.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <libconfig.h>

const char *MAIN_MENU_ARRAY[COUNT_MAIN_MENU] = {K_PLAY, K_SETTINGS, K_HIGH_SCORE, K_EXIT};
const char *SETTINGS_MENU_ARRAY[COUNT_SETTINGS_MENU] = {K_DIFFICULTY, K_KEY_BINDS, K_BACK};
const char *KEY_BINDS_MENU_ARRAY[COUNT_KEY_BINDS_MENU] = {K_UP, K_DOWN, K_RIGHT, K_LEFT, K_UP_LEFT, K_UP_RIGHT, K_DOWN_LEFT, K_DOWN_RIGHT};
const char *DIFFICULTY_MENU_ARRAY[COUNT_DIFFICULTY_MENU] = {K_EASY, K_MEDUIM, K_HARD};
const int DIFFICULTY_SIZE[COUNT_DIFFICULTY_MENU+1] = {10, 15, 20, 25};
const int DIFFICULTY_SPACE[COUNT_DIFFICULTY_MENU] = {6, 5, 4};
const int DIFFICULTY_WORD[COUNT_DIFFICULTY_MENU+1] = {3, 5, 7, 9};

void *glb_malloc(size_t size, char *location)
{
	void *ptr = malloc(size);
	if (ptr == NULL)
	{
		char error_message[256];
		snprintf(error_message, sizeof(error_message), "Error: Memory allocation failed! Location : %s", location);
		glb_handleError(error_message);
	}
	return ptr;
}

void glb_free(void **pointer)
{
	if (pointer != NULL && *pointer != NULL)
	{
		free(*pointer);	 // Free the memory
		*pointer = NULL; // Set the pointer to NULL to prevent accidental reuse or double-freeing
	}
}

void glb_freeArray(void ***pointer)
{
	if (pointer != NULL && *pointer != NULL)
	{
		for (int i = 0; (*pointer)[i] != NULL; i++)
		{
			glb_free((void **)&(*pointer)[i]);
		}
		glb_free((void **)&(*pointer));
		*pointer = NULL;
	}
}

void glb_handleError(char *message)
{
	const char *immutable_message = message;
	while (true)
	{
		clear();
		mvprintw(1, (COLS - strlen("Press Esc to exit")) / 2, "Press Esc to exit");
		mvprintw(LINES / 2, (COLS - strlen(immutable_message)) / 2, "%s", immutable_message);
		refresh();
		int ch = getch();
		if (ch == 27)
			break;
	}
}

int glb_min(int a, int b)
{
	return (a < b) ? a : b;
}

int glb_max(int a, int b)
{
	return (a > b) ? a : b;
}

int glb_even(int a)
{
	return (a / 2) * 2;
}

int glb_odd(int a)
{
	return (a / 2) * 2 + 1;
}

char *glb_strUppercase(char *str)
{
	char *s = str;
	while (*s)
	{
		*s = toupper((unsigned char)*s);
		s++;
	}
	return str;
}

char *glb_strLowercase(char *str)
{
	char *s = str;
	while (*s)
	{
		*s = tolower((unsigned char)*s);
		s++;
	}
	return str;
}

char glb_charUppercase(char c)
{
	return toupper((unsigned char)c);
}

char glb_charLowercase(char c)
{
	return tolower((unsigned char)c);
}

char *glb_strRemoveWhitespace(char *str)
{
	char *s = str, *d = str;
	while (*s)
	{
		if (!isspace((unsigned char)*s))
		{
			*d++ = *s;
		}
		s++;
	}
	*d = '\0';
	return str;
}

int glb_strToInt(char *str)
{
	return atoi(str);
}

int glb_charToInt(char c)
{
	return isdigit((unsigned char)c) ? c - '0' : (int)stateError;
}

char *glb_intToStr(int a)
{
	char buffer[12]; // Enough to hold the largest 32-bit integer and the null terminator
	snprintf(buffer, sizeof(buffer), "%d", a);
	char *result = glb_malloc(sizeof(char) * (strlen(buffer) + 1), "glb_intToStr");
	if (result)
	{
		strcpy(result, buffer);
	}
	return result;
}

char *glb_strArrayToCharArray(char **str, int count)
{
	char *result = glb_malloc(sizeof(char) * count, "glb_strArrayToCharArray");
	if (!result)
	{
		glb_handleError("Memory allocation failure. Location : glb_strArrayToCharArray()");
		return NULL; // Handle memory allocation failure
	}
	for (int i = 0; i < count; i++)
	{
		result[i] = glb_strToChar(str[i]);
	}
	return result;
}

char glb_strToChar(char *str)
{
	return str[0];
}

void glb_getConfigureValue(const char *key, int *value)
{
	config_t cfg;
	config_init(&cfg);
	if (!config_read_file(&cfg, CONFIG_FILE))
	{
		char error_message[256];
		snprintf(error_message, sizeof(error_message), "Error reading config file at line %d: %s. Location glb_getConfigureValue() \n",
				 config_error_line(&cfg), config_error_text(&cfg));
		glb_handleError(error_message);
		config_destroy(&cfg);
		value = NULL;
		return;
	}
	if (!config_lookup_int(&cfg, key, value))
		value = NULL;
}

void glb_getConfigureValues(const char **key_array, int **value_array, int count)
{
	*value_array = glb_malloc(sizeof(int) * count, "glb_getConfigureValues()");
	for (int i = 0; i < count; ++i)
	{
		glb_getConfigureValue(key_array[i], &(*value_array)[i]);
		if (value_array[i] == NULL) // Check for errors
		{
			char error_message[256];
			snprintf(error_message, sizeof(error_message), "Failed to retrieve %s key from your config file. Location : glb_getValues()", key_array[i]);
			glb_handleError(error_message);
			glb_freeArray((void ***)&value_array);
			return;
		}
	}
}

char glb_orientation(int width, int height)
{
	if (width < height)
		return HORIZONTAL;
	else if (height < width)
		return VERTICAL;
	else
		return rand() % 2 ? HORIZONTAL : VERTICAL;
}

void glb_shuffleArray(void *array, size_t element_size, int count)
{
	if (count <= 1)
		return;
	srand(time(NULL));
	char temp[element_size];
	for (int i = count - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);
		memcpy(temp, (char *)array + i * element_size, element_size);
		memcpy(array + i * element_size, array + j * element_size, element_size);
		memcpy(array + j * element_size, temp, element_size);
	}
}
void glb_copyArray(void **src_array, void **dst_array, int size, size_t element_size)
{
	if (!src_array || !dst_array || size <= 0)
	{
		glb_handleError("Error: Array copy wasn't successful .\n");
		return;
	}

	for (int i = 0; i < size; i++)
	{
		if (src_array[i] != NULL)
			memcpy(dst_array[i], src_array[i], element_size);
	}
}
