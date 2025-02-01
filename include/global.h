#pragma once

#include <stdio.h>
#include <limits.h> 

// Files and gobal variables
#define CONFIG_FILE "config.cfg"
#define WORDS_FILE "words.txt"
#define HIGH_SCORE_FILE "high_score.txt"
#define HORIZONTAL 1
#define VERTICAL 0
#define MAX_WORD_LENGTH 15
#define MAX_PLAYER_NAME_LENGTH 10
#define PASSAGE_SYMBOL '-'
#define WALL_SYMBOL '#'
#define END_SYMBOL '*'
#define END_WORD_COLLECTING 0	// To validate collected word in the graph
#define START_WORD_COLLECTING 1 // To read a word in the graph

// Titles
#define TITLE_MAIN_MENU "=== Main Menu ==="
#define TITLE_SETTINGS_MENU "=== Settings ==="
#define TITLE_HIGH_SCORE "=== High Score List ==="
#define TITLE_DIFFICULTY_MENU "=== Game Difficulty ==="
#define TITLE_KEY_BINDS_MENU "=== Key Binds ==="

// Array Count
#define COUNT_MAIN_MENU 4
#define COUNT_SETTINGS_MENU 3
#define COUNT_DIFFICULTY_MENU 3
#define COUNT_KEY_BINDS_MENU 8

// Array Value
#define K_PLAY "Play"
#define K_SETTINGS "Settings"
#define K_HIGH_SCORE "High Score"
#define K_EXIT "Exit"
#define K_DIFFICULTY "Difficulty"
#define K_KEY_BINDS "Key Binds"
#define K_UP "Up"
#define K_DOWN "Down"
#define K_RIGHT "Right"
#define K_LEFT "Left"
#define K_UP_LEFT "UpLeft"
#define K_UP_RIGHT "UpRight"
#define K_DOWN_LEFT "DownLeft"
#define K_DOWN_RIGHT "DownRight"
#define K_BACK "Back"
#define K_EASY "Easy"
#define K_MEDUIM "Meduim"
#define K_HARD "Hard"

// Arrays
extern const char *MAIN_MENU_ARRAY[];
extern const char *SETTINGS_MENU_ARRAY[];
extern const char *KEY_BINDS_MENU_ARRAY[];
extern const char *DIFFICULTY_MENU_ARRAY[];
extern const int DIFFICULTY_SIZE[];
extern const int DIFFICULTY_SPACE[];
extern const int DIFFICULTY_WORD[];

// Enum
typedef enum
{
	stateMainMenu,
	statePlay,
	statePlayerName,
	stateStartGame,
	statePlaying,
	stateGameOver,
	stateSettingsMenu,
	stateDifficultyMenu,
	stateKeyBindsMenu,
	stateHighScore,
	stateError,
	stateExit
} GameState;

// Make sure that the ket binds are in the same order as config file.
typedef enum
{
	UpKey,
	DownKey,
	RightKey,
	LeftKey,
	UpLeftKey,
	UpRightKey,
	DownLeftKey,
	DownRightKey
} GameKeyBinds;

// Enum for key IDs
typedef enum
{
	PLAY_KEY,
	SETTINGS_KEY,
	HIGH_SCORES_KEY,
	DIFFICULTY_KEY,
	KEY_BINDS_KEY,
	BACK_EXIT,
} MenuState;

// Enum for key IDs
typedef enum
{
	WALL_COLOR,
	END_COLOR,
	PASSAGE_COLOR,
	MENU_COLOR,
	PLAYER_COLOR,
	PATH_COLOR,
	VISITED_COLOR,
	SCORE_COLOR,
} GameColors;

typedef enum{
NODE_NOT_VISITED,
NODE_VISITED,
NODE_USED,
} VisitedType;

// Enum to define the type of the value
typedef enum {
    CONFIG_INT,
    CONFIG_STRING,
	CONFIG_CHAR,
} config_value_type;
typedef struct Node
{
	char character;
	int visited, row, col;
	struct Node *up, *down, *left, *right, *up_left, *up_right, *down_left, *down_right;
} Node;

typedef struct QueueNode {
    Node *node;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front, *rear;
} Queue;

void *glb_malloc(size_t size, char *location);
void glb_free(void **pointer);
void glb_freeArray(void ***pointer);
void glb_handleError(char *message);
int glb_min(int a, int b);
int glb_max(int a, int b);
int glb_odd(int a);
int glb_even(int a);
char *glb_intToStr(int a);
char *glb_strUppercase(char *str);
char *glb_strLowercase(char *str);
char glb_charUppercase(char c);
char glb_charLowercase(char c);
 char *glb_strRemoveWhitespace(char *str);
int glb_strToInt(char *str);
int glb_charToInt(char c);
char glb_strToChar(char *str);
char *glb_strArrayToCharArray(char **str, int count);
void glb_getConfigureValue(const char *key, int *value);
void glb_getConfigureValues(const char **key_array, int **value_array, int count);
char glb_orientation(int width, int height);
void glb_shuffleArray(void *array, size_t element_size, int count);
void glb_copyArray(void **src_array, void **dst_array, int size, size_t element_size);
