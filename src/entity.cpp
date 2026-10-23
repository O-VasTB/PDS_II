#include "entity.hpp"

EntityData::EntityData(EntityType type, float x, float y, float speed)
    : _type(type), 
      _posicao{x, y}, 
      _vel(speed), 
      vivo(true) {
} //inicializador da entidade

EntityType EntityData::getType() const {
    return _type;
} //Função para retornar tipo e posteriormente calcular colisao

Pos EntityData::getPosition() const {
    return _posicao;
} //Função para retornar posição para geração gráfica

void EntityData::setPosition(float x, float y) {
    _posicao.x = x;
    _posicao.y = y;
} //Inicializar em posição 

bool EntityData::confereVivo() const {
    return vivo;
} //Meio auto explicativo

void EntityData::setAlive(bool alive) {
    this->vivo = alive;
} //Torna vivo entidades q antes estavam inativas (parte da reciclagem dos inimigos gerados)