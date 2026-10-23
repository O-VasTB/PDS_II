/**
 * @file GerenciadorInput.cpp
 * @brief Implementação dos métodos da classe GerenciadorInput utilizando a Raylib.
 */

#include "GerenciadorInput.hpp"
#include <raylib.h>

void GerenciadorInput::processarEntradas() {
    // Na Raylib, a verificação de teclas é feita diretamente sob demanda,
    // portanto este método pode ficar vazio ou gerenciar estados globais se necessário.
}

bool GerenciadorInput::isTeclaPressionada(int codigoTecla) const {
    // Utiliza a função IsKeyDown da Raylib para verificar se a tecla está pressionada
    return IsKeyDown(codigoTecla);
}