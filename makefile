CC = gcc
CFLAGS = -Wall -Wextra -g3
INCLUDES = -Iinclude -Isrc
SRCS = $(wildcard src/*.c) main.c
BUILD_DIR = build
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))
TARGET = output/main.exe
LIBS = -lws2_32

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(INCLUDES) $(LIBS)

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

$(BUILD_DIR)/main.o: main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
