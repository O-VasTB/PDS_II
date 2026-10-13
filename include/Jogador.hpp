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

public:
    /**
     * @brief Construtor da classe Jogador.
     * @param type Tipo da entidade.
     * @param x Posição X inicial.
     * @param y Posição Y inicial.
     * @param speed Velocidade inicial.
     */
    Jogador(EntityType type, float x = 0.0f, float y = 0.0f, float speed = 0.0f);

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
};

#endif // JOGADOR_HPP