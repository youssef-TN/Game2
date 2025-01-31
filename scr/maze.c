#include "maze.h"
#include "global.h"
#include "random.h"
#include "menu.h"

#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

int initGame(int *rows, int *cols, int difficulty_index, int *key_binds_values)
{
    int min_maze_size = DIFFICULTY_SIZE[difficulty_index], max_maze_size = DIFFICULTY_SIZE[difficulty_index + 1];

    *rows = randomInRange(min_maze_size, max_maze_size);
    *cols = randomInRange(min_maze_size, max_maze_size);

    int ch;
    int offset = 10;

    while (true)
    {
        clear();
        mvprintw(1, (COLS - strlen("Game Difficulty ") - strlen(DIFFICULTY_MENU_ARRAY[difficulty_index])) / 2, "Game Difficulty %s ", DIFFICULTY_MENU_ARRAY[difficulty_index]);
        mvprintw(3, (COLS - strlen("Key Binds:")) / 2, "Key Binds:");

        // get all the keybinds from config
        for (int i = 0; i < COUNT_KEY_BINDS_MENU; i++)
        {
            if (i % 2 == 0)
            {
                mvprintw(4 + i, ((COLS - strlen(KEY_BINDS_MENU_ARRAY[i]) - strlen(keyname(key_binds_values[i]))) / 2) - offset, "%s %s", KEY_BINDS_MENU_ARRAY[i], keyname(key_binds_values[i]));
            }
            else
            {
                mvprintw(4 + i - 1, ((COLS - strlen(KEY_BINDS_MENU_ARRAY[i]) - strlen(keyname(key_binds_values[i]))) / 2) + offset, "%s %s", KEY_BINDS_MENU_ARRAY[i], keyname(key_binds_values[i]));
            }
        }
        refresh();
        ch = getch();
        switch (ch)
        {
        case 27: // ASCII value for ESC key
            return stateMainMenu;
        case '\n': // Enter key
            return statePlay;
        }
    }
    return stateError;
}

int getDifficultyAndKeyBinds(int *difficulty_index, int **key_binds_values)
{
    // Get difficulty
    *difficulty_index = getDifficulty();
    if (*difficulty_index == stateError)
    {
        glb_handleError("Failed to get difficulty from config file. Location : getDifficultyAndKeyBinds()");
        return stateError;
    }
    // Get key binds
    *key_binds_values = getKeyBinds();
    if (*key_binds_values == NULL)
    {
        glb_handleError("Failed to get keybinds from config file. Location : getDifficultyAndKeyBinds()");
        return stateError;
    }
    return statePlayerName;
}

int getWords(char ***words_array, int *count, int difficulty_index)
{
    int min_word_difficulty = DIFFICULTY_WORD[difficulty_index], max_word_difficulty = DIFFICULTY_WORD[difficulty_index + 1];

    FILE *file = fopen(WORDS_FILE, "r");
    if (file == NULL)
    {
        glb_handleError("Failed to open config file. Location : getWords()");
        return stateError;
    }

    char buffer[MAX_WORD_LENGTH];
    int capacity = 33;
    *words_array = glb_malloc(capacity * sizeof(char *), "getWirds()");
    if (*words_array == NULL)
        return stateError;

    // Read each line from the file
    while (fgets(buffer, sizeof(buffer), file) && capacity > *count)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        if (strlen(buffer) >= min_word_difficulty && strlen(buffer) < max_word_difficulty)
        {
            (*words_array)[*count] = strdup(buffer); // Copy word to array
            if (!(*words_array)[*count])
            {
                glb_handleError("Memory allocation failed. Location: getWords()");
                return stateError;
            }
            (*count)++;
        }
    }
    // Free the buffer used by getline
    fclose(file);
    return statePlay;
}

int playerNaming(char **name)
{
    glb_free((void **)name);
    *name = glb_malloc(MAX_PLAYER_NAME_LENGTH * sizeof(char), "playerNaming()");
    for (int i = 0; i < MAX_PLAYER_NAME_LENGTH; i++)
    {
        (*name)[i] = '\0';
    }

    int ch, i = 0;
    while (i < MAX_PLAYER_NAME_LENGTH - 1)
    {
        clear();
        mvprintw(0, (COLS - strlen("Select your name")) / 2, "Select your name");
        mvprintw(3, (COLS - strlen(*name)) / 2, "%s", *name);
        refresh();
        ch = getch();
        if (isalnum(ch))
        {
            (*name)[i++] = ch;
            (*name)[i] = '\0';
        }
        else if (ch == '\n' && i != 0)
        {
            (*name)[i] = '\0'; // Ensure the name is null-terminated
            return statePlayerName;
        }
        else if (ch == KEY_BACKSPACE && i > 0)
        {
            (*name)[--i] = '\0';
        }
        else if (ch == 27)
        {
            return stateMainMenu;
        }
    }
    (*name)[i] = '\0'; // Ensure the name is null-terminated
    return stateError;
}

int addNeighbors(Node ***grid, int rows, int cols)
{
    Node *current;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            current = &(*grid)[row][col]; // Use a pointer to the actual Node
            if (current == NULL)
                return stateError;
            if (row > 0)
                current->up = &(*grid)[row - 1][col]; // Up
            if (row < rows - 1)
                current->down = &(*grid)[row + 1][col]; // Down
            if (col > 0)
                current->left = &(*grid)[row][col - 1]; // Left
            if (col < cols - 1)
                current->right = &(*grid)[row][col + 1]; // Right
            if (row > 0 && col > 0)
                current->up_left = &(*grid)[row - 1][col - 1]; // Up-Left
            if (row > 0 && col < cols - 1)
                current->up_right = &(*grid)[row - 1][col + 1]; // Up-Right
            if (row < rows - 1 && col > 0)
                current->down_left = &(*grid)[row + 1][col - 1]; // Down-Left
            if (row < rows - 1 && col < cols - 1)
                current->down_right = &(*grid)[row + 1][col + 1]; // Down-Right
        }
    }
    return stateStartGame;
}

void createNode(Node *node, char character, int row, int col)
{
    node->character = character;
    node->row = row;
    node->col = col;
    node->up = node->down = node->left = node->right = node->down_left = node->down_right = node->up_left = node->up_right = NULL;
    node->visited = NODE_NOT_VISITED;
}

int resetGrid(Node ***grid, int rows, int cols)
{
    *grid = glb_malloc(rows * sizeof(Node *), "main()");
    if (*grid == NULL)
        return stateError;
    for (int i = 0; i < rows; i++)
    {
        // Allocate memory for each column
        (*grid)[i] = glb_malloc(cols * sizeof(Node), "main()");
        if ((*grid)[i] == NULL)
            return stateError;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            createNode(&((*grid)[i][j]), PASSAGE_SYMBOL, i, j);
            if (&((*grid)[i][j]) == NULL)
                return stateError;
        }
    }
    return stateStartGame;
}

int resetVisitedGrid(Node ***grid, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            (*grid)[i][j].visited = NODE_NOT_VISITED;
        }
    }
    return stateStartGame;
}

void divide(Node ***grid, int x, int y, int width, int height, int orientation, int space, int *wall_count)
{
    if (width < space || height < space)
        return;

    int horizontal = orientation == HORIZONTAL;
    // Where the wall is
    int wall_x = x + (horizontal ? 0 : glb_even(randomInRange(0, width - space - 1) + 2));  // Ensure even coordinate
    int wall_y = y + (horizontal ? glb_even(randomInRange(0, height - space - 1) + 2) : 0); // Ensure even coordinate
    // Where the passage is
    int passage_x = wall_x + (horizontal ? glb_odd(randomInRange(0, width - space - 2) + 1) : 0);  // Ensure odd coordinate
    int passage_y = wall_y + (horizontal ? 0 : glb_odd(randomInRange(0, height - space - 2) + 1)); // Ensure odd coordinate
    // Length of the wall
    int length = horizontal ? width : height;
    // Direction to move to draw the wall
    int x_direction = horizontal ? 1 : 0;
    int y_direction = horizontal ? 0 : 1;
    // Draw the wall
    for (int i = 0; i < length; i++)
    {
        if (wall_x != passage_x || wall_y != passage_y)
        { // Leave a passage
            (*grid)[wall_y][wall_x].character = WALL_SYMBOL;
            (*wall_count)++;
        }
        wall_x += x_direction;
        wall_y += y_direction;
    }
    // Recursivelry divide the two new regions
    int new_width = horizontal ? width : wall_x - x + 1;
    int new_height = horizontal ? wall_y - y + 1 : height;
    divide(grid, x, y, new_width, new_height, choose_orientation(new_width, new_height), space, wall_count);
    int new_x = horizontal ? x : wall_x;
    int new_y = horizontal ? wall_y : y;
    new_width = horizontal ? width : x + width - wall_x;
    new_height = horizontal ? y + height - wall_y : height;

    divide(grid, new_x, new_y, new_width, new_height, choose_orientation(new_width, new_height), space, wall_count);
}

int choose_orientation(int width, int height)
{
    if (height > width)
    {
        return HORIZONTAL;
    }
    else if (width > height)
    {
        return VERTICAL;
    }
    else
    {
        return rand() % 2 ? VERTICAL : HORIZONTAL;
    }
}

void addWords(Node ***grid, int rows, int cols, char ***selected_words, int *count_selected_words, char **words, int word_count, int wall_count)
{
    *selected_words = glb_malloc(word_count * sizeof(char *), "addWords() -> selected_words");
    int row, col;
    int count_non_empty_words = 0;
    for (int i = 0; i < word_count; i++)
    {
        if (rows * cols < count_non_empty_words + wall_count + strlen(words[i]))
            break;
        // Randomly pick a starting position in the grid
        row = randomInRange(0, rows - 1);
        col = randomInRange(0, cols - 1);
        while ((*grid)[row][col].character != PASSAGE_SYMBOL)
        {
            row = randomInRange(0, rows - 1);
            col = randomInRange(0, cols - 1);
        }
        if (!WordDFS(words[i], 0, &((*grid)[row][col])))
            continue;
        (*selected_words)[*count_selected_words] = words[i];
        (*count_selected_words)++;
        count_non_empty_words += strlen(words[i]);
    }
}

int WordDFS(char *word, int index, Node *node)
{
    if (word[index] == '\0')
        return 1;

    if (!isValidNeighbor(node, NODE_NOT_VISITED, 1, PASSAGE_SYMBOL, 1))
        return 0;

    (*node).visited = NODE_VISITED;
    int count = 0;
    Node **neighbors = getNeighbors(node, &count, NODE_NOT_VISITED, 1, PASSAGE_SYMBOL, 1);
    for (int i = 0; i < count; i++)
    {
        if (WordDFS(word, index + 1, neighbors[i]))
        {
            (*node).character = word[index];
            return 1;
        }
    }
    // Backtrack: unmark the current node as visited
    (*node).visited = NODE_NOT_VISITED;

    return 0;
}

Node **getNeighbors(Node *node, int *count, int visited, int is_equal_visited, char character, int is_equal_character)
{
    Node **neighbors = glb_malloc(COUNT_KEY_BINDS_MENU * sizeof(Node *), "getNeighbors() -> neighbors");

    if (isValidNeighbor(node->up, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->up;
    }
    if (isValidNeighbor(node->down, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->down;
    }
    if (isValidNeighbor(node->left, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->left;
    }
    if (isValidNeighbor(node->right, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->right;
    }
    if (isValidNeighbor(node->up_left, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->up_left;
    }
    if (isValidNeighbor(node->up_right, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->up_right;
    }
    if (isValidNeighbor(node->down_left, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->down_left;
    }
    if (isValidNeighbor(node->down_right, visited, is_equal_visited, character, is_equal_character))
    {
        neighbors[(*count)++] = node->down_right;
    }

    if ((*count) == 0)
        return NULL;

    glb_shuffleArray((void *)neighbors, sizeof(Node *), (*count));
    return neighbors;
}

int isValidNeighbor(Node *node, int visited, int is_equal_visited, char character, int is_equal_character)
{
    return node &&
           (is_equal_visited ? node->visited == visited : node->visited != visited) &&
           (is_equal_character ? node->character == character : node->character != character);
}

Node *spawnPosition(Node ***grid, int rows, int cols, char symbol)
{
    Node *spawn = NULL;

    // Find a random spawn position
    while (spawn == NULL || spawn->character != PASSAGE_SYMBOL)
    {
        int row = rand() % rows;
        int col = rand() % cols;
        spawn = &(*grid)[row][col];
    }

    spawn->character = symbol;

    return spawn;
}

void displayGrid(Node **grid, Node *player_node, int rows, int cols)
{
    clear();

    // Get terminal size
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    // Calculate starting positions for centering
    int start_y = (term_rows - rows) / 2;
    int start_x = (term_cols - (cols * 3)) / 2; // Each cell takes 3 spaces ( " X " )

    // Print the grid centered
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int y = start_y + i;
            int x = start_x + (j * 3);

            if (grid[i][j].character == WALL_SYMBOL)
            {
                attron(COLOR_PAIR(WALL_COLOR));
                mvprintw(y, x, "   "); // Wall is a blank block
                attroff(COLOR_PAIR(WALL_COLOR));
            }
            else if (grid[i][j].character == END_SYMBOL)
            {
                attron(COLOR_PAIR(END_COLOR));
                mvprintw(y, x, " %c ", END_SYMBOL);
                attroff(COLOR_PAIR(END_COLOR));
            }
            else
            {
                if (player_node == &grid[i][j])
                {
                    attron(COLOR_PAIR(PLAYER_COLOR));
                    mvprintw(y, x, " %c ", grid[i][j].character);
                    attroff(COLOR_PAIR(PLAYER_COLOR));
                }
                else
                {
                    if (grid[i][j].visited==NODE_USED)
                    {attron(COLOR_PAIR(MENU_COLOR));
                    mvprintw(y, x, " %c ", grid[i][j].character);
                    attroff(COLOR_PAIR(MENU_COLOR));
                    }
                    else{
                    attron(COLOR_PAIR(PASSAGE_COLOR));
                    mvprintw(y, x, " %c ", grid[i][j].character);
                    attroff(COLOR_PAIR(PASSAGE_COLOR));
                    }
                }
            }
        }
    }

    refresh();
}

void enqueue(Queue *q, Node *node)
{
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->node = node;
    newNode->next = NULL;
    if (!q->rear)
    {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

Node *dequeue(Queue *q)
{
    if (!q->front)
        return NULL;
    QueueNode *temp = q->front;
    Node *node = temp->node;
    q->front = q->front->next;
    if (!q->front)
        q->rear = NULL;
    free(temp);
    return node;
}

int is_queue_empty(Queue *q)
{
    return q->front == NULL;
}

Node **findShortPath(Node **grid, Node *player_node, Node *end_node, int rows, int cols, int *path_length)
{
    if (!player_node || !end_node)
        return NULL;

    Queue q = {NULL, NULL};
    enqueue(&q, player_node);
    player_node->visited = NODE_VISITED;

    Node *prev[rows][cols];
    memset(prev, 0, sizeof(prev));

    while (!is_queue_empty(&q))
    {
        Node *current = dequeue(&q);

        if (current == end_node)
            break;
        int neighbors_count = 0;
        Node **neighbors = getNeighbors(current, &neighbors_count, NODE_NOT_VISITED, 1, WALL_SYMBOL, 0);

        for (int i = 0; i < neighbors_count; i++)
        {
            neighbors[i]->visited = NODE_VISITED;
            prev[neighbors[i]->row][neighbors[i]->col] = current;
            enqueue(&q, neighbors[i]);
        }
    }

    Node *path[rows * cols];
    *path_length = 0;
    for (Node *current = end_node; current != NULL; current = prev[current->row][current->col])
    {
        path[(*path_length)++] = current;
    }

    Node **result = malloc((*path_length) * sizeof(Node *));
    for (int i = 0; i < *path_length; i++)
    {
        result[i] = path[*path_length - i - 1];
    }

    return result;
}

void displayGameOverGrid(Node **grid, Node **path, int path_length, int rows, int cols, int score,
                         char *player_name, const char *difficulty, int step_count, int words_collected)
{
    clear();
    const char *temp_player_name = player_name;
    // Get terminal size
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    // Calculate starting positions for centering
    int start_y = (term_rows - rows) / 2;
    int start_x = (term_cols - (cols * 3)) / 2; // Each cell takes 3 spaces ( " X " )

    // Display game over summary with colors
    int label_x = (term_cols - 30) / 2;
    int value_x = label_x + 25; // Offset for better alignment

    mvprintw(start_y - 5, label_x + 12, "Game Over!");
    mvprintw(start_y - 4, label_x, "Player:");
    attron(COLOR_PAIR(MENU_COLOR));
    mvprintw(start_y - 4, value_x, "%s", temp_player_name);
    attroff(COLOR_PAIR(MENU_COLOR));

    mvprintw(start_y - 3, label_x, "Difficulty:");
    attron(COLOR_PAIR(MENU_COLOR));
    mvprintw(start_y - 3, value_x, "%s", difficulty);
    attroff(COLOR_PAIR(MENU_COLOR));

    mvprintw(start_y - 2, label_x, "Final Score:");
    attron(COLOR_PAIR(SCORE_COLOR));
    mvprintw(start_y - 2, value_x, "%d", score);
    attroff(COLOR_PAIR(SCORE_COLOR));

    mvprintw(start_y - 1, label_x, "Words Collected:");
    attron(COLOR_PAIR(SCORE_COLOR));
    mvprintw(start_y - 1, value_x, "%d", words_collected);
    attroff(COLOR_PAIR(SCORE_COLOR));

    mvprintw(start_y, label_x, "Steps:");
    attron(COLOR_PAIR(SCORE_COLOR));
    mvprintw(start_y, value_x, "%d", step_count);
    attroff(COLOR_PAIR(SCORE_COLOR));

    mvprintw(start_y + 1, label_x, "Shortest Path Achieved:");
    attron(COLOR_PAIR(MENU_COLOR));
    mvprintw(start_y + 1, value_x, "%s", path_length - step_count - 1 ? "No" : "Yes");
    attroff(COLOR_PAIR(MENU_COLOR));

    // Print the grid centered
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            bool is_path = false;
            for (int k = 0; k < path_length; k++)
            {
                if (&grid[i][j] == path[k])
                {
                    is_path = true;
                    break;
                }
            }

            int y = start_y + i + 3;
            int x = start_x + (j * 3);

            if (grid[i][j].character == WALL_SYMBOL)
            {
                attron(COLOR_PAIR(WALL_COLOR));
                mvprintw(y, x, "   "); // Wall is a blank block
                attroff(COLOR_PAIR(WALL_COLOR));
            }
            else if (grid[i][j].character == END_SYMBOL)
            {
                attron(COLOR_PAIR(END_COLOR));
                mvprintw(y, x, " %c ", END_SYMBOL);
                attroff(COLOR_PAIR(END_COLOR));
            }
            else if (is_path)
            {
                attron(COLOR_PAIR(PATH_COLOR));
                mvprintw(y, x, " %c ", grid[i][j].character);
                attroff(COLOR_PAIR(PATH_COLOR));
            }
            else
            {
                if (grid[i][j].visited != NODE_NOT_VISITED)
                {
                    attron(COLOR_PAIR(PLAYER_COLOR));
                    mvprintw(y, x, " %c ", grid[i][j].character);
                    attroff(COLOR_PAIR(PLAYER_COLOR));
                }
                else
                {
                    attron(COLOR_PAIR(PASSAGE_COLOR));
                    mvprintw(y, x, " %c ", grid[i][j].character);
                    attroff(COLOR_PAIR(PASSAGE_COLOR));
                }
            }
        }
    }

    refresh();
}

int calculateScore(int step_count, int shortest_path_length, char **guessed_words, int word_count)
{
    // Base score calculation
    double efficiency = (double)shortest_path_length - 1 / step_count;
    int base_score = (int)(efficiency * step_count);

    // Word score calculation
    int word_score = 0;
    for (int i = 0; i < word_count; i++)
    {
        word_score += strlen(guessed_words[i]);
    }

    return base_score + word_score;
}

int isValidWord(char *word, char **selected_words, int selected_words_count)
{
    printf("%s \n",word);
    for (int i = 0; i < selected_words_count; i++)
    {
        if (strcmp(word, selected_words[i]) == 0)
            return 1; // Word is valid
    }
    return 0; // Word is not valid
}

void deleteStr(char ***array, int *count, int index)
{
    if (index < 0 || index >= *count)
    {
        return;
    }

    // Free the memory for the string at the specified index
    free((*array)[index]);

    // Shift the remaining strings to the left
    for (int i = index; i < *count - 1; i++)
    {
        (*array)[i] = (*array)[i + 1];
    }

    // Nullify the last string (optional, but it's good practice to avoid dangling pointers)
    (*array)[*count - 1] = NULL;

    // Decrease the count of strings in the array
    (*count)--;

    // Optionally resize the array if you are dynamically allocating memory for it
    *array = realloc(*array, (*count) * sizeof(char *));
}
