CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wstrict-prototypes -Iinclude
LDFLAGS =
TARGET  = myls

SRC = src/main.c \
      src/options.c \
      src/list.c \
      src/format.c \
      src/utils.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

src/%.o: src/%.c include/ls.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

help:
	@echo "make        - build $(TARGET)"
	@echo "make clean  - remove object files and binary"
	@echo "make run    - build and run $(TARGET)"

clean:
	rm -f $(OBJ) $(TARGET)
