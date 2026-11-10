#include "inimigo.hpp"

namespace {
const float kLargura = 28.0f;       /**< Largura da caixa de colisão. */
const float kAlturaAndarilho = 28.0f;  /**< Altura do Andarilho. */
const float kAlturaEscudo = 30.0f;   /**< Altura do Escudo. */
const int kVidaAndarilho = 1;          /**< Vida do Andarilho. */
const int kVidaEscudo = 2;           /**< Vida do Escudo. */
const int kPontosAndarilho = 100;      /**< Recompensa por derrotar um Andarilho. */
const int kPontosEscudo = 200;       /**< Recompensa por derrotar um Escudo. */
}  // namespace

Inimigo::Inimigo(float x, float y, float speed, TipoInimigo tipo)
    : EntityData(EntityType::Inimigo, x, y, speed),
      _tipoInimigo(tipo),
      _direcao(-1),
      _vida(tipo == TipoInimigo::Escudo ? kVidaEscudo : kVidaAndarilho),
      _pontos(tipo == TipoInimigo::Escudo ? kPontosEscudo : kPontosAndarilho),
      _largura(kLargura),
      _altura(tipo == TipoInimigo::Escudo ? kAlturaEscudo : kAlturaAndarilho) {}

void Inimigo::atualizar(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _posicao.x += _direcao * _vel * dt;
}

void Inimigo::inverterDirecao() {
    _direcao = -_direcao;
}

void Inimigo::receberDano(int dano) {
    if (dano <= 0 || !confereVivo()) {
        return;
    }
    _vida -= dano;
    if (_vida <= 0) {
        _vida = 0;
        setAlive(false);
    }
}

void Inimigo::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const Rectangle hb = getHitbox();
    const Color corpo = (_tipoInimigo == TipoInimigo::Escudo) ? DARKGREEN : BROWN;
    DrawRectangleRec(hb, corpo);

    // "Pés" e olhos viram para o lado em que o inimigo anda.
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y + hb.height - 6.0f),
                  static_cast<int>(hb.width), 6, BLACK);
    const int olhoY = static_cast<int>(hb.y) + 7;
    const int olho1 = static_cast<int>(hb.x) + (_direcao > 0 ? 12 : 4);
    DrawRectangle(olho1, olhoY, 6, 6, WHITE);
    DrawRectangle(olho1 + 9, olhoY, 6, 6, WHITE);
    DrawRectangle(olho1 + (_direcao > 0 ? 3 : 0), olhoY + 2, 3, 3, BLACK);
    DrawRectangle(olho1 + 9 + (_direcao > 0 ? 3 : 0), olhoY + 2, 3, 3, BLACK);
}

Rectangle Inimigo::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

TipoInimigo Inimigo::getTipo() const {
    return _tipoInimigo;
}

int Inimigo::getVida() const {
    return _vida;
}

int Inimigo::getPontos() const {
    return _pontos;
}

int Inimigo::getDirecao() const {
    return _direcao;
}