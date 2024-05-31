CC=g++

CFLAGS=-std=c++11 -g

EXECUTABLE=program

SRC_DIR=src
BUILD_DIR=build
SRC_FILES=main.cpp
OBJ_FILES=$(addprefix $(BUILD_DIR)/,main.o Input.o)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJ_FILES)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/main.o $(SRC_DIR)/main.cpp

$(BUILD_DIR)/Input.o: $(SRC_DIR)/Input.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

all: $(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE) $(OBJ_FILES)

# Impedir que make interprete 'clean' e 'all' como arquivos
.PHONY: all clean
