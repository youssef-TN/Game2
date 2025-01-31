#pragma once

#include <stdio.h>

int general_menu(int type, const char *menu_array[], int array_count, const char *title);
int updateKeybind(const char *key_name, const char *menu_array[], int **key_binds_values, int count_array);
int updateConfig(int type, const char *key, int value);
void display(int type, int i, int selected_difficulty, const char *menu_array[], int *key_binds_values);
int *getKeyBinds();
int getDifficulty();