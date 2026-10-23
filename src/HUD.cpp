/**
 * @file hud.cpp
 * @brief Implementação dos métodos da classe HUD.
 */

#include "HUD.hpp"
#include <raylib.h>
#include <string>

HUD::HUD() : vidas(3), moedas(0), pontuacao(0) {
}

void HUD::atualizarDados(int novasVidas, int novasMoedas, int novaPontuacao) {
    vidas = novasVidas;
    moedas = novasMoedas;
    pontuacao = novaPontuacao;
}

void HUD::desenhar() const {
    // Exibe as informações na tela usando as funções de texto da Raylib
    DrawText(TextFormat("VIDAS: %d", vidas), 20, 20, 20, RAYWHITE);
    DrawText(TextFormat("MOEDAS: %d", moedas), 150, 20, 20, RAYWHITE);
    DrawText(TextFormat("PONTOS: %d", pontuacao), 300, 20, 20, YELLOW);
}