/**
 * @file Jogador.cpp
 * @brief Implementação dos métodos da classe Jogador.
 */

#include "Jogador.hpp"

// Construtor inicializando a classe base EntityData e os atributos específicos do Jogador
Jogador::Jogador(EntityType type, float x, float y, float speed)
    : EntityData(type, x, y, speed), vidas(3), pontuacao(0) {
}

void Jogador::pular() {
    // Lógica básica de pulo (ex: alterar posição Y ou velocidade vertical)
    _posicao.y -= 10.0f; 
}

void Jogador::tomarDano(int quantidade) {
    vidas -= quantidade;
    if (vidas < 0) {
        vidas = 0;
        setAlive(false); // Utiliza o método da classe base EntityData
    }
}

int Jogador::getVidas() const {
    return vidas;
}

int Jogador::getPontuacao() const {
    return pontuacao;
}

void Jogador::adicionarPontos(int quantidade) {
    pontuacao += quantidade;
}

float Jogador::getLargura() const {
    return 32.0f; // Largura padrão da caixa de colisão do jogador
}

float Jogador::getAltura() const {
    return 32.0f; // Altura padrão da caixa de colisão do jogador
}