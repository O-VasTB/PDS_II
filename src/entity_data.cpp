#include "entity_data.hpp"

EntityData::EntityData(EntityType type, float x, float y, float speed)
    : _type(type), _vel(speed < 0.0f ? 0.0f : speed), vivo(true) {
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
