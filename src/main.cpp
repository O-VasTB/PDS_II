/**
 * @file main.cpp
 * @brief Ponto de entrada do jogo.
 */

#include <exception>
#include <iostream>
#include "jogo.hpp"

/**
 * @brief Cria o jogo e executa o laço principal.
 * @return int 0 em caso de sucesso; 1 se ocorrer um erro fatal.
 */
int main() {
    try {
        Jogo jogo(1280, 720, "JOGO PLATAFORMA");
        jogo.executar();
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
