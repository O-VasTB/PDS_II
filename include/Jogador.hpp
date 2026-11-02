#ifndef JOGADOR_HPP
#define JOGADOR_HPP
#include "entity.hpp"

/**
 * @class Jogador
 * @brief Gerencia as propriedades e comportamentos do jogador no jogo.
 */
class Jogador : public EntityData {
private:
    int vidas;         /**< Quantidade de vidas restantes do jogador. */
    int pontuacao;     /**< Pontuação acumulada. */
    float _velY;       /**< Velocidade vertical atual (para a gravidade e pulo). */
    bool _noChao;      /**< Indica se o jogador está no chão (para permitir pulo). */

public:

    void setNoChao(bool chao);
    bool getNoChao() const;
    void pararQueda() { _velY = 0.0f; } // Zera a velocidade vertical
    void setVidas(int vidas);

    /**
     * @brief Construtor da classe Jogador.
     * @param type Tipo da entidade.
     * @param x Posição X inicial.
     * @param y Posição Y inicial.
     * @param speed Velocidade inicial.
     */
    Jogador(EntityType type, float x = 0.0f, float y = 0.0f, float speed = 0.0f);

    /**
     * @brief Atualiza a física vertical e a posição do jogador a cada frame.
     * @param dt Tempo decorrido desde o último frame (Delta Time).
     */
    void atualizar(float dt);

    /**
     * @brief Executa a ação de pulo do jogador.
     */
    void pular();

    /**
     * @brief Processa o dano recebido pelo jogador.
     * @param quantidade Quantidade de dano/vidas a perder.
     */
    void tomarDano(int quantidade = 1);

    /**
     * @brief Obtém o número de vidas do jogador.
     * @return Quantidade de vidas.
     */
    int getVidas() const;

    /** @brief Retorna a largura da caixa de colisão, em pixels. */
    float getLargura() const;

    /** @brief Retorna a altura da caixa de colisão, em pixels. */
    float getAltura() const;

    /**
     * @brief Obtém a pontuação acumulada do jogador.
     * @return Pontuação atual.
     */
    int getPontuacao() const;

    /**
     * @brief Adiciona pontos à pontuação atual do jogador.
     * @param quantidade Quantidade de pontos a serem adicionados.
     */
    void adicionarPontos(int quantidade);
};

#endif // JOGADOR_HPP