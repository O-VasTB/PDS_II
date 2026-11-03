CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Iinclude -MMD -MP
TARGET   := build/jogo

SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp,build/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

ifeq ($(OS),Windows_NT)
    LDLIBS := -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := build/jogo.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LDLIBS := -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        LDLIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDLIBS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

-include $(DEP)