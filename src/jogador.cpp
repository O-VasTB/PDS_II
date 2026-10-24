/**
 * @file jogador.cpp
 * @brief Implementação dos métodos da classe Jogador.
 */

#include "Jogador.hpp"

// Construtor inicializando a classe base EntityData e os atributos específicos
Jogador::Jogador(EntityType type, float x, float y, float speed)
    : EntityData(type, x, y, speed), vidas(3), pontuacao(0), _velY(0.0f) {
}

void Jogador::atualizar(float dt) {
    _noChao = false; // Assume que está no ar até que a colisão prove o contrário

    const float GRAVIDADE = 1200.0f; // Força da gravidade a puxar para baixo
    
    // A gravidade aumenta a velocidade vertical ao longo do tempo
    _velY += GRAVIDADE * dt;
    
    // Atualiza a posição Y com base na velocidade vertical e no delta time (dt)
    _posicao.y += _velY * dt;
}

void Jogador::tomarDano(int quantidade) {
    vidas -= quantidade;
    if (vidas < 0) {
        vidas = 0;
        setAlive(false);
    }
}

int Jogador::getVidas() const {
    return vidas;
}

void Jogador::setVidas(int novasVidas) {
    vidas = novasVidas; // Usa o mesmo nome 'vidas' que os outros métodos
}

int Jogador::getPontuacao() const {
    return pontuacao;
}

void Jogador::adicionarPontos(int quantidade) {
    pontuacao += quantidade;
}

void Jogador::pular() {
    if (_noChao) {          // Só pular se estiver no chão!
        _velY = -450.0f; 
        _noChao = false;    // Deixa de estar no chão assim que ganha impulso para cima
    }
}

void Jogador::setNoChao(bool chao) {
    _noChao = chao;
}

bool Jogador::getNoChao() const {
    return _noChao;
}

float Jogador::getLargura() const {
    return 32.0f;
}

float Jogador::getAltura() const {
    return 32.0f;
}