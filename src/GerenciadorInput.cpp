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
    // Mapeamento expandido para aceitar tanto as Setas quanto o WASD / Espaço
    if (codigoTecla == KEY_LEFT || codigoTecla == 'A') {
        return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    }
    if (codigoTecla == KEY_RIGHT || codigoTecla == 'D') {
        return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    }
    if (codigoTecla == KEY_UP || codigoTecla == 'W' || codigoTecla == KEY_SPACE) {
        return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE);
    }
    if (codigoTecla == KEY_DOWN || codigoTecla == 'S') {
        return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    }

    // Para qualquer outra tecla genérica
    return IsKeyDown(codigoTecla);
}