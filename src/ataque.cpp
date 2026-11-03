/**
 * @file tiro.cpp
 * @brief Implementação da classe Tiro.
 */

#include "ataque.hpp"

namespace {
const float kAlcanceMaximo = 420.0f; /**< Distância máxima percorrida por um tiro, em pixels. */
const float kTamanhoTiro = 10.0f;    /**< Lado da caixa de colisão do tiro, em pixels. */
const int kDanoTiro = 1;             /**< Dano causado por um tiro. */
}  // namespace

Tiro::Tiro(float x, float y, float speed, int direcao)
    : EntityData(EntityType::Tiro, x, y, speed),
      _direcao(direcao < 0 ? -1 : 1),
      _dano(kDanoTiro),
      _distPercorrida(0.0f),
      _alcanceMax(kAlcanceMaximo),
      _largura(kTamanhoTiro),
      _altura(kTamanhoTiro) {}

void Tiro::atualizar(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    const float passo = _vel * dt;
    _posicao.x += _direcao * passo;
    _distPercorrida += passo;
    if (_distPercorrida >= _alcanceMax) {
        setAlive(false);
    }
}

void Tiro::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const int cx = static_cast<int>(_posicao.x + _largura / 2.0f);
    const int cy = static_cast<int>(_posicao.y + _altura / 2.0f);
    DrawCircle(cx, cy, _largura / 2.0f, ORANGE);
    DrawCircle(cx, cy, _largura / 4.0f, YELLOW);
}

Rectangle Tiro::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

int Tiro::getDano() const {
    return _dano;
}

int Tiro::getDirecao() const {
    return _direcao;
}