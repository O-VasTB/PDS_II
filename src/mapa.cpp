/**
 * @file mapa.cpp
 * @brief Implementação dos métodos da classe Mapa.
 */

#include "mapa.hpp"
#include <raylib.h>

Mapa::Mapa() : larguraTiles(40), alturaTiles(15), tamanhoTile(32.0f) {
    // Inicialização básica ou carregamento de matriz padrão do mapa
}

void Mapa::desenhar() const {
    // Desenho simplificado de exemplo para blocos do chão/paredes
    for (int x = 0; x < larguraTiles; ++x) {
        // Exemplo: desenha uma linha de chão na base do mapa
        DrawRectangle(static_cast<int>(x * tamanhoTile), static_cast<int>((alturaTiles - 2) * tamanhoTile), 
                      static_cast<int>(tamanhoTile), static_cast<int>(tamanhoTile), DARKGRAY);
    }
}

bool Mapa::verificarColisaoBloco(float x, float y) const {
    // Converte a posição em coordenadas do mundo para índices da matriz/grid
    int tileX = static_cast<int>(x / tamanhoTile);
    int tileY = static_cast<int>(y / tamanhoTile);

    // Se estiver fora dos limites laterais, considera colisão (parede invisível)
    if (tileX < 0 || tileX >= larguraTiles) return true;
    
    // Se passar do fundo da tela, colide (chão padrão)
    if (tileY >= alturaTiles - 2) return true;

    return false;
}