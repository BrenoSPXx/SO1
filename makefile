# Definindo o compilador
CC=g++

# Opções de compilação, onde:
# -c: compila o código fonte para arquivos objeto sem linkar
# -Wall: habilita a maioria das mensagens de aviso
CFLAGS=-c -Wall

# Nome do executável
EXECUTABLE=program

# Diretório para os arquivos objeto
BIN_DIR=bin

# Adicionando o diretório bin/ ao início de cada arquivo objeto
OBJECTS=$(BIN_DIR)/main.o $(BIN_DIR)/read_file.o

# Regra padrão
all: $(BIN_DIR) $(EXECUTABLE)

# Regra para criar o diretório bin
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Regra para criar o executável
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Regra para compilar main.cpp
$(BIN_DIR)/main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o $@

# Regra para compilar read_file.cc
$(BIN_DIR)/read_file.o: read_file.cc
	$(CC) $(CFLAGS) read_file.cc -o $@

# Regra para limpar os arquivos compilados
clean:
	rm -rf $(BIN_DIR) $(EXECUTABLE)

# Impedir que make interprete 'clean' e 'all' como arquivos
.PHONY: all clean
