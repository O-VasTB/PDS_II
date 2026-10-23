/**
 * @file item.cpp
 * @brief Implementação dos métodos da classe Item.
 */

#include "Item.hpp"

Item::Item(float x, float y, int valorPontos) 
    : posX(x), posY(y), valor(valorPontos), coletado(false) {
}

int Item::coletar() {
    if (!coletado) {
        coletado = true;
        return valor; // Retorna os pontos que o item vale
    }
    return 0; // Já foi coletado, não pontua novamente
}

bool Item::isColetado() const {
    return coletado;
}

Rectangle Item::getBounds() const {
    return { posX, posY, 24.0f, 24.0f }; // Tamanho padrão da caixa de colisão do item
}

void Item::desenhar() const {
    if (!coletado) {
        // Desenha um item simples (ex: quadrado amarelo representando uma moeda)
        DrawRectangle(static_cast<int>(posX), static_cast<int>(posY), 24, 24, GOLD);
    }
}