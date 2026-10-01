#ifndef JOGADOR_HPP
#define JOGADOR_HPP
#include "entity.hpp"

class Jogador : public Entity {
private:
    int vidas;         /**< Quantidade de vidas restantes do jogador. */
    int pontuacao;     /**< Pontuação acumulada. */
    bool noChao;       /**< Estado do jogador (se está no chão ou no ar). */

public:
    /**
     * @brief Construtor da classe Jogador.
     * @param x Posição X inicial.
     * @param y Posição Y inicial.
     */
    Jogador(float x = 0.0f, float y = 0.0f);

    /**
     * @brief Atualiza a posição e o estado do jogador.
     * @param deltaTime Tempo decorrido.
     */
    void atualizar(float deltaTime) override;

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