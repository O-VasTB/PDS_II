# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Bibliotecas e dependências da Raylib para Windows (MinGW/MSYS2)
LIBS = lib/libraylib.a -lgdi32 -lwinmm -lpthread

# Executável final de saída
TARGET = build/jogo_executavel.exe

# Lista de arquivos fonte e objetos
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

# Regra principal (compila)
all: $(TARGET)

# Ligação do executável
$(TARGET): $(OBJ)
	@mkdir -p build
	$(CXX) $(OBJ) -o $@ $(LIBS)

# Compilação dos arquivos .cpp em objetos .o
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para compilar e executar o jogo imediatamente
run: all
	@./$(TARGET)

# Limpeza dos arquivos compilados
clean:
	rm -f build/*.o $(TARGET)

.PHONY: all run clean