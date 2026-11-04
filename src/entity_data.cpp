#include "entity_data.hpp"
#include "excecoes.hpp"
#include <cmath>

EntityData::EntityData(EntityType type, float x, float y, float speed)
    : _type(type), _vel(speed), vivo(true) {
        if (!std::isfinite(x) || !std::isfinite(y)) {
            throw ErroValorInvalido("Posicao inicial invalida (NaN ou infinito)");
        }
        if (!std::isfinite(speed) || speed < 0.0f) {
            throw ErroValorInvalido("A velocidade deve ser um numero finito e nao negativo");
        }
    _posicao.x = x;
    _posicao.y = y;
}

EntityType EntityData::getType() const {
    return _type;
}

Pos EntityData::getPosition() const {
    return _posicao;
}

void EntityData::setPosition(float x, float y) {
    _posicao.x = x;
    _posicao.y = y;
}

bool EntityData::confereVivo() const {
    return vivo;
}

void EntityData::setAlive(bool alive) {
    vivo = alive;
}
