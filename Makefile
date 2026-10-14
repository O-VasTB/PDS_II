CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
GCOVFLAGS = --coverage

# Sem arquivos SRC por enquanto, apenas para testar a infraestrutura
TEST_SRC = tests/test_main.cpp tests/test_mapa.cpp

test:
	@if not exist build mkdir build
	$(CXX) $(CXXFLAGS) $(GCOVFLAGS) $(TEST_SRC) -o build/test_runner
	./build/test_runner

clean:
	rm -rf build/*