.PHONY: build clean run zip

SRC_DIR := src
OBJ_DIR := obj

CC := gcc
CFLAGS := -Wall
LDFLAGS := -lm

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET := tema1

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

run: $(TARGET)
	./$(TARGET)

zip:
	zip tema1.zip $(SRCS) $(SRC_DIR)/*.h Makefile README.md