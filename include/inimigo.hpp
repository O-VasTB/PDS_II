#ifndef INIMIGO_H
#define INIMIGO_H

#include <raylib.h>
#include "entity_data.hpp"

/**
 * @file inimigo.hpp
 * @brief Contrato da classe Inimigo e do enum TipoInimigo.
 */

/**
 * @enum TipoInimigo
 * @brief Variações de inimigo, que alteram velocidade, vida e pontuação.
 */
enum class TipoInimigo {
    Goomba, /**< Inimigo básico que patrulha e morre com um pisão. */
    Koopa   /**< Inimigo mais resistente, que exige mais de um golpe. */
};

/**
 * @class Inimigo
 * @brief Inimigo que patrulha o mapa horizontalmente.
 *
 * Inverte a direção ao colidir com paredes (tratado pelo nível) e pode ser
 * derrotado por pisão ou por tiros do jogador.
 */
class Inimigo : public EntityData {
public:
    /**
     * @brief Construtor de Inimigo.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade de patrulha.
     * @param tipo Variação do inimigo.
     */
    Inimigo(float x, float y, float speed, TipoInimigo tipo);

    /** @brief Destrutor virtual. */
    virtual ~Inimigo() = default;

    /**
     * @brief Atualiza a posição do inimigo (patrulha horizontal).
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void atualizar(float dt);

    /** @brief Inverte a direção de patrulha (ao bater em parede ou borda). */
    void inverterDirecao();

    /**
     * @brief Aplica dano ao inimigo; ele é marcado como morto quando a vida chega a zero.
     * @param dano Quantidade de dano recebida.
     */
    void receberDano(int dano);

    /** @brief Desenha o inimigo na tela usando Raylib. */
    void desenhar() const;

    /**
     * @brief Obtém a caixa de colisão do inimigo.
     * @return Rectangle Retângulo com posição e dimensões do inimigo.
     */
    Rectangle getHitbox() const;

    /**
     * @brief Obtém a variação do inimigo.
     * @return TipoInimigo Goomba ou Koopa.
     */
    TipoInimigo getTipo() const;

    /**
     * @brief Obtém a vida restante do inimigo.
     * @return int Pontos de vida restantes.
     */
    int getVida() const;

    /**
     * @brief Obtém a pontuação concedida ao jogador ao derrotar este inimigo.
     * @return int Pontos da recompensa.
     */
    int getPontos() const;

    /**
     * @brief Obtém a direção atual de patrulha.
     * @return int 1 (direita) ou -1 (esquerda).
     */
    int getDirecao() const;

private:
    TipoInimigo _tipoInimigo; /**< Variação do inimigo. */
    int _direcao;             /**< Direção atual de patrulha (1 ou -1). */
    int _vida;                /**< Pontos de vida restantes. */
    int _pontos;              /**< Pontuação concedida ao ser derrotado. */
    float _largura;           /**< Largura da caixa de colisão. */
    float _altura;            /**< Altura da caixa de colisão. */
};

#endif
