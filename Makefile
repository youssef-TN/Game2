# Compiler and flags
CC = cc
CFLAGS = -I include/ -g
LDFLAGS = -lncurses -lconfig

# Source files
SRC = scr/main.c scr/global.c scr/random.c scr/menu.c scr/high-score.c scr/maze.c scr/player.c

# Output binary
OUTPUT = game

# Default target
all: $(OUTPUT)

# Build the output binary
$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(OUTPUT)
