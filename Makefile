# ============================================================
# Makefile - Projeto Mario 2D (PDS II)
#
#   make            compila o jogo         -> build/mario
#   make run        compila e executa o jogo
#   make test       compila e roda os testes (doctest)
#   make coverage   roda os testes com cobertura (gcovr)
#   make clean      limpa a pasta build/
#
# ATENCAO: as linhas de comando (receitas) comecam com TAB,
# nunca com espacos. Se der "missing separator", e isso.
# ============================================================

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Iinclude

# Flags do Raylib via pkg-config. 
# Caso seu ambiente não tenha o pkg-config configurado, substitua por:
# RAYLIB ?= -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
RAYLIB ?= $(shell pkg-config --cflags --libs raylib)

BUILD  := build
TARGET := $(BUILD)/mario

SRCS      := $(wildcard src/*.cpp)
LIB_SRCS  := $(filter-out src/main.cpp,$(SRCS))
TEST_SRCS := $(wildcard tests/*.cpp)

.PHONY: all run test coverage clean

all: $(TARGET)

$(TARGET): $(SRCS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(RAYLIB)

run: $(TARGET)
	./$(TARGET)

# --- Testes: tudo de src/ menos o main.cpp + os arquivos de tests/ ---
test: $(BUILD)/testes
	./$(BUILD)/testes

$(BUILD)/testes: $(LIB_SRCS) $(TEST_SRCS) | $(BUILD)
	$(CXX) $(CXXFLAGS) -Itests $(LIB_SRCS) $(TEST_SRCS) -o $@ $(RAYLIB)

# --- Cobertura: compila com --coverage, roda e resume com gcovr ---
# Na entrega final, acrescente --fail-under-line 60 ao gcovr.
coverage: | $(BUILD)
	rm -f $(BUILD)/*.gcda $(BUILD)/*.gcno
	$(CXX) $(CXXFLAGS) -O0 -g --coverage -Itests $(LIB_SRCS) $(TEST_SRCS) -o $(BUILD)/testes_cov $(RAYLIB)
	./$(BUILD)/testes_cov
	gcovr -r . --exclude 'tests/' --print-summary

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)/*