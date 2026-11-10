CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude -MMD -MP

# Raylib estática e dependências do Windows
LIBS = lib/libraylib.a -lopengl32 -lgdi32 -lwinmm -lpthread

TARGET      = build/jogo_executavel.exe
TEST_TARGET = build/testes.exe

SRC      = $(wildcard src/*.cpp)
OBJ      = $(patsubst src/%.cpp,build/%.o,$(SRC))
LIB_OBJ  = $(filter-out build/main.o,$(OBJ))
TEST_SRC = $(wildcard tests/*.cpp)

.PHONY: all run test clean

all: $(TARGET)

# "| build" cria a pasta uma única vez, sem usar "mkdir -p" (que não existe no cmd do Windows)
$(TARGET): $(OBJ) | build
	$(CXX) $(OBJ) -o $@ $(LIBS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir build

run: all
	@$(subst /,\,$(TARGET))

# Testes: todos os .cpp de src/ exceto main.cpp + os de tests/ (só um deles define o main do doctest)
$(TEST_TARGET): $(TEST_SRC) $(LIB_OBJ) | build
	$(CXX) $(CXXFLAGS) -Itests $(TEST_SRC) $(LIB_OBJ) -o $@ $(LIBS)

test: $(TEST_TARGET)
	@$(subst /,\,$(TEST_TARGET))

clean:
	if exist build rmdir /s /q build

# Dependências de cabeçalhos geradas pelo -MMD
-include $(OBJ:.o=.d)