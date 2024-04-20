CC=g++

CFLAGS=-std=c++11

EXECUTABLE=program

SRC_DIR=src
SRC_FILES=main.cpp

all:
	$(CC) -o $(EXECUTABLE) $(addprefix $(SRC_DIR)/,$(SRC_FILES))

clean:
	rm -rf $(EXECUTABLE)

# Impedir que make interprete 'clean' e 'all' como arquivos
.PHONY: all clean
