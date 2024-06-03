CC=g++

CFLAGS=-std=c++11 -g -Wall -Wextra

EXECUTABLE=program

SRC_DIR=src
BUILD_DIR=build
SRC_FILES=main.cpp
OBJ_FILES=$(addprefix $(BUILD_DIR)/,main.o Input.o BitmapManager.o MemorySystem.o)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJ_FILES)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/main.o $(SRC_DIR)/main.cpp

$(BUILD_DIR)/Input.o: $(SRC_DIR)/Input.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

$(BUILD_DIR)/BitmapManager.o: $(SRC_DIR)/BitmapManager.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/BitmapManager.o $(SRC_DIR)/BitmapManager.cpp

$(BUILD_DIR)/MemorySystem.o: $(SRC_DIR)/MemorySystem.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/MemorySystem.o $(SRC_DIR)/MemorySystem.cpp

all: $(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE) $(OBJ_FILES)

# Impedir que make interprete 'clean' e 'all' como arquivos
.PHONY: all clean
