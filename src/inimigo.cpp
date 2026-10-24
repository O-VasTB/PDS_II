/**
 * @file inimigo.cpp
 * @brief Implementação dos métodos da classe Inimigo.
 */

#include "Inimigo.hpp"
#include <algorithm>

float Inimigo::velocidadePorTipo(TipoInimigo tipo) {
    if (tipo == TipoInimigo::GOOMBA) {
        return 60.0f;
    } else {
        return 90.0f; // Koopa mais rápido
    }
}

Inimigo::Inimigo(float xPos, float yPos, TipoInimigo tipo, float limiteEsquerdo, float limiteDireito)
    : EntityData(EntityType::Inimigo, xPos, yPos, velocidadePorTipo(tipo)),
      _tipoInimigo(tipo),
      _estado(EstadoInimigo::ANDANDO),
      _direcao(-1),
      _pontos(tipo == TipoInimigo::GOOMBA ? 100 : 200),
      _dano(1),
      _largura(32.0f),
      _altura(32.0f),
      _limiteEsquerdo(std::min(limiteEsquerdo, limiteDireito)),
      _limiteDireito(std::max(limiteEsquerdo, limiteDireito)),
      _tempoEsmagado(0.0f)
{
}

void Inimigo::atualizar(float dt) {
    if (_estado == EstadoInimigo::MORTO) return;

    if (_estado == EstadoInimigo::ESMAGADO) {
        _tempoEsmagado += dt;
        if (_tempoEsmagado > 1.0f) { // Desaparece após 1 segundo esmagado
            _estado = EstadoInimigo::MORTO;
        }
        return;
    }

    if (_estado == EstadoInimigo::ANDANDO) {
        float velocidade = velocidadePorTipo(_tipoInimigo);
        _posicao.x += _direcao * velocidade * dt; // Acede diretamente a _posicao.x da base

        // Verifica os limites de patrulha e inverte a direção se necessário
        if (_posicao.x < _limiteEsquerdo) {
            _posicao.x = _limiteEsquerdo;
            inverterDirecao();
        } else if (_posicao.x > _limiteDireito - _largura) {
            _posicao.x = _limiteDireito - _largura;
            inverterDirecao();
        }
    }
}

void Inimigo::inverterDirecao() {
    _direcao *= -1;
}

void Inimigo::esmagar() {
    if (_estado == EstadoInimigo::ANDANDO) {
        _estado = EstadoInimigo::ESMAGADO;
        _tempoEsmagado = 0.0f;
    }
}

void Inimigo::matar() {
    _estado = EstadoInimigo::MORTO;
}

bool Inimigo::estaAndando() const {
    return _estado == EstadoInimigo::ANDANDO;
}

TipoInimigo Inimigo::getTipoInimigo() const {
    return _tipoInimigo;
}

EstadoInimigo Inimigo::getEstado() const {
    return _estado;
}

int Inimigo::getDirecao() const {
    return _direcao;
}

float Inimigo::getVelocidadeX() const {
    if (_estado != EstadoInimigo::ANDANDO) return 0.0f;
    return _direcao * velocidadePorTipo(_tipoInimigo);
}

int Inimigo::getPontos() const {
    return _pontos;
}

int Inimigo::getDano() const {
    if (_estado != EstadoInimigo::ANDANDO) return 0;
    return _dano;
}

float Inimigo::getLargura() const {
    return _largura;
}

float Inimigo::getAltura() const {
    return _altura;
}

bool Inimigo::foiPontuado() const {
    return _pontuado;
}

void Inimigo::marcarPontuado() {
    _pontuado = true;
}