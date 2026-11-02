/**
 * @file hud.cpp
 * @brief Implementação dos métodos da classe HUD.
 */

#include "HUD.hpp"
#include <raylib.h>
#include <string>

HUD::HUD() : vidas(3), moedas(0), pontuacao(0) {
    fonteHUD = { 0 };
    iconeVida = { 0 };
    iconeMoeda = { 0 };
}

HUD::~HUD() {
    // Descarrega os recursos da Raylib para evitar vazamento de memória
    if (fonteHUD.texture.id != 0) {
        UnloadFont(fonteHUD);
    }
    if (iconeVida.id != 0) {
        UnloadTexture(iconeVida);
    }
    if (iconeMoeda.id != 0) {
        UnloadTexture(iconeMoeda);
    }
}

bool HUD::inicializarRecursos(const std::string& caminhoFonte) {
    // Tenta carregar a fonte personalizada; se o caminho estiver vazio, usa a padrão
    if (!caminhoFonte.empty()) {
        fonteHUD = LoadFont(caminhoFonte.c_str());
    }
    return true;
}

void HUD::atualizarDados(int vidasAtual, int moedasAtual, int pontuacaoAtual) {
    vidas = vidasAtual;
    moedas = moedasAtual;
    pontuacao = pontuacaoAtual;
}

void HUD::renderizar() const {
    DrawText(TextFormat("VIDAS: %d", vidas), 20, 20, 20, DARKGRAY);
    DrawText(TextFormat("PONTOS: %d", pontuacao), 300, 20, 20, YELLOW);
}
