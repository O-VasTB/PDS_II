#ifndef TIRO_H
#define TIRO_H

#include <raylib.h>
#include "entity_data.hpp"

/**
 * @file tiro.hpp
 * @brief Contrato da classe Tiro, projétil disparado pelo jogador.
 */

/**
 * @class Tiro
 * @brief Projétil disparado pelo jogador.
 *
 * Move-se horizontalmente em linha reta e é destruído ao atingir o alcance máximo
 * ou ao colidir com um inimigo ou bloco (a colisão é tratada pelo nível/jogo).
 */
class Tiro : public EntityData {
public:
    /**
     * @brief Construtor de Tiro.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade horizontal do projétil.
     * @param direcao Direção do disparo: 1 para direita, -1 para esquerda.
     */
    Tiro(float x, float y, float speed, int direcao);

    /** @brief Destrutor virtual. */
    virtual ~Tiro() = default;

    /**
     * @brief Atualiza a posição do tiro e marca-o como inativo se passar do alcance.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void atualizar(float dt);

    /** @brief Desenha o tiro na tela usando Raylib. */
    void desenhar() const;

    /**
     * @brief Obtém a caixa de colisão do tiro.
     * @return Rectangle Retângulo com posição e dimensões do tiro.
     */
    Rectangle getHitbox() const;

    /**
     * @brief Obtém o dano causado pelo tiro.
     * @return int Quantidade de dano aplicada ao inimigo atingido.
     */
    int getDano() const;

    /**
     * @brief Obtém a direção do disparo.
     * @return int 1 (direita) ou -1 (esquerda).
     */
    int getDirecao() const;

private:
    int _direcao;          /**< Direção do deslocamento (1 ou -1). */
    int _dano;             /**< Dano causado ao atingir um inimigo. */
    float _distPercorrida; /**< Distância já percorrida desde o disparo. */
    float _alcanceMax;     /**< Distância máxima que o tiro pode percorrer. */
    float _largura;        /**< Largura da caixa de colisão. */
    float _altura;         /**< Altura da caixa de colisão. */
};

#endif
