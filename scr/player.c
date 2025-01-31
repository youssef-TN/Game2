#include "player.h"
#include "global.h"

#include <string.h>

void playerMovement(Node **player_node, int *key_binds_values, int pressed_key, int *step_count, char collected_word[MAX_WORD_LENGTH])
{
	if (key_binds_values[UpKey] == pressed_key)
	{
		*player_node = (*player_node)->up != NULL && (*player_node)->up->character != WALL_SYMBOL ? (*player_node)->up : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[DownKey] == pressed_key)
	{
		*player_node = (*player_node)->down != NULL && (*player_node)->down->character != WALL_SYMBOL ? (*player_node)->down : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[LeftKey] == pressed_key)
	{
		*player_node = (*player_node)->left != NULL && (*player_node)->left->character != WALL_SYMBOL ? (*player_node)->left : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[RightKey] == pressed_key)
	{
		*player_node = (*player_node)->right != NULL && (*player_node)->right->character != WALL_SYMBOL ? (*player_node)->right : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[UpLeftKey] == pressed_key)
	{
		*player_node = (*player_node)->up_left != NULL && (*player_node)->up_left->character != WALL_SYMBOL ? (*player_node)->up_left : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[UpRightKey] == pressed_key)
	{
		*player_node = (*player_node)->up_right != NULL && (*player_node)->up_right->character != WALL_SYMBOL ? (*player_node)->up_right : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[DownLeftKey] == pressed_key)
	{
		*player_node = (*player_node)->down_left != NULL && (*player_node)->down_left->character != WALL_SYMBOL ? (*player_node)->down_left : *player_node;
		addLetters(player_node, collected_word);
	}
	else if (key_binds_values[DownRightKey] == pressed_key)
	{
		*player_node = (*player_node)->down_right != NULL && (*player_node)->down_right->character != WALL_SYMBOL ? (*player_node)->down_right : *player_node;
		addLetters(player_node, collected_word);
	}
	else
	{
		return;
	}
	(*step_count)++;
	if ((*player_node)->visited == NODE_NOT_VISITED)
	{
		(*player_node)->visited = NODE_VISITED;
	}
}

int gameOver(Node *player_node, Node *end_node)
{
	if (player_node == end_node)
	{
		return stateGameOver;
	}
	return statePlaying;
}

void addLetters(Node **player_node, char collected_word[MAX_WORD_LENGTH])
{
	int len;
	if (collected_word[0] != '\0')
	{
		len = strlen(collected_word);
		collected_word[len] = (*player_node)->character;
		collected_word[len + 1] = '\0';
		(*player_node)->visited = NODE_USED;
	}
}