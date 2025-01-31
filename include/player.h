#pragma once

#include "global.h"

void playerMovement(Node **player_node, int *key_binds_values, int pressed_key,int *step_count,char collected_word[MAX_WORD_LENGTH]);
int gameOver(Node *player_node, Node *end_node);
void addLetters(Node **player_node, char collected_word[MAX_WORD_LENGTH]);
