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

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CXX) $(OBJ) -o $@ $(LIBS)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@./$(TARGET)

# Testes: todos os .cpp de src/ exceto main.cpp + os de tests/ (só um deles define o main do doctest)
$(TEST_TARGET): $(TEST_SRC) $(LIB_OBJ)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -Itests $(TEST_SRC) $(LIB_OBJ) -o $@ $(LIBS)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

clean:
	rm -rf build

# Dependências de cabeçalhos geradas pelo -MMD
-include $(OBJ:.o=.d)