/**
 * @file item.cpp
 * @brief Implementação dos métodos da classe Item.
 */

#include "Item.hpp"

int Item::pontosPorTipo(TipoItem tipo) {
    switch (tipo) {
        case TipoItem::MOEDA: return 10;
        case TipoItem::COGUMELO: return 1000;
        case TipoItem::ESTRELA: return 1000;
        default: return 10;
    }
}

Item::Item(float x, float y, TipoItem tipo)
    : EntityData(EntityType::Item, x, y, 0.0f),
      _tipoItem(tipo),
      _pontos(pontosPorTipo(tipo)),
      _coletado(false),
      _largura(24.0f),
      _altura(24.0f),
      _tempoAnimacao(0.0f)
{
    // posX e posY já foram inicializados pelo EntityData
}

void Item::atualizar(float dt) {
    if (!_coletado) {
        _tempoAnimacao += dt;
    }
}

int Item::coletar() {
    if (!_coletado) {
        _coletado = true;
        return _pontos; // Retorna os pontos correspondentes ao tipo do item
    }
    return 0; // Já foi coletado, não pontua novamente
}

TipoItem Item::getTipoItem() const {
    return _tipoItem;
}

bool Item::foiColetado() const {
    return _coletado;
}

int Item::getPontos() const {
    return _pontos;
}

float Item::getLargura() const {
    return _largura;
}

float Item::getAltura() const {
    return _altura;
}

float Item::getTempoAnimacao() const {
    return _tempoAnimacao;
}