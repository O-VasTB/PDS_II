/**
 * @file inimigo.cpp
 * @brief Implementação dos métodos da classe Inimigo.
 */

#include "Inimigo.hpp"

Inimigo::Inimigo(float x, float y, float velocidade) 
    : posX(x), posY(y), velX(velocidade), vivo(true) {
}

void Inimigo::atualizar() {
    if (!vivo) return;

    // Movimentação simples de vai e vem horizontal
    posX += velX;
    
    // Inverte a direção se atingir limites arbitrários (exemplo de patrulha)
    if (posX < 100.0f || posX > 600.0f) {
        velX *= -1.0f;
    }
}

void Inimigo::destruir() {
    vivo = false;
}

bool Inimigo::isVivo() const {
    return vivo;
}

Rectangle Inimigo::getBounds() const {
    return { posX, posY, 32.0f, 32.0f }; // Tamanho padrão da caixa de colisão do inimigo
}

void Inimigo::desenhar() const {
    if (vivo) {
        // Desenha um inimigo simples (ex: retângulo vermelho)
        DrawRectangle(static_cast<int>(posX), static_cast<int>(posY), 32, 32, RED);
    }
}