#pragma once

#include "global.h"

#include <stdio.h>
#include <stdlib.h>

int initGame(int *rows, int *cols, int difficulty_index, int *key_binds_values);
int getDifficultyAndKeyBinds(int *difficulty_index, int **key_binds_values);
int getWords(char ***words, int *word_count,int difficulty_index);
int playerNaming(char **name);
int addNeighbors(Node ***grid, int rows, int cols);
void createNode(Node *node, char character, int row, int col);
int resetGrid(Node ***grid, int rows, int cols);
int resetVisitedGrid(Node ***grid, int rows, int cols);
void divide(Node ***grid, int x, int y, int width, int height, int orientation, int space, int *wall_count);
int choose_orientation(int width, int height);
void addWords(Node ***grid, int rows, int cols, char ***selected_words, int *count_selected_words, char **words, int word_count, int number_of_walls);
int WordDFS(char *word, int index, Node *node);
Node **getNeighbors(Node *node, int *count, int visited,int is_equal_visited, char character,int is_equal_character);
Node *spawnPosition(Node ***grid, int rows, int cols, char symbol);
int isValidNeighbor(Node *node, int visited, int is_equal_visited, char character, int is_equal_character);
void enqueue(Queue *q, Node *node);
Node *dequeue(Queue *q);
int is_queue_empty(Queue *q);
Node **findShortPath(Node **grid, Node *player_node, Node *end_node, int rows, int cols, int *path_length);
void displayGrid(Node **grid, Node *player_node, int rows, int cols);
void displayGameOverGrid(Node **grid, Node **path, int path_length, int rows, int cols, int score,
						char *player_name, const char *difficulty, int step_count, int words_collected);
int calculateScore(int step_count, int shortest_path_length, char **guessed_words, int word_count);
int isValidWord(char *word, char **selected_words, int selected_words_count);
void deleteStr(char ***array, int *count, int index);
