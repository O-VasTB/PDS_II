/**
 * @file Inimigo.hpp
 * @brief Contrato da classe Inimigo (inimigos que patrulham o mapa).
 *
 * Inimigo herda de EntityData (entity.hpp) e usa o tipo EntityType::Inimigo.
 * Ele anda de um lado para o outro dentro de um trecho do mapa
 * (limiteEsquerdo..limiteDireito). Não conhece o Jogador: quem decide se o
 * jogador pulou em cima ou foi tocado é o GerenciadorColisao, que depois
 * chama esmagar() ou lê getDano().
 *
 * Gravidade e colisão com chão/paredes ficam a cargo do GerenciadorColisao;
 * aqui só tratamos o movimento horizontal e o estado do inimigo.
 */

#ifndef INIMIGO_HPP
#define INIMIGO_HPP

#include "entity.hpp"

/**
 * @enum TipoInimigo
 * @brief Variações de inimigo (definem velocidade, pontos e dano).
 */
enum class TipoInimigo {
    GOOMBA,  /**< Lento e simples. */
    KOOPA    /**< Mais rápido e vale mais pontos. */
};

/**
 * @enum EstadoInimigo
 * @brief Estados possíveis de um inimigo.
 *
 * Fluxo: ANDANDO -> ESMAGADO -> (some) ou ANDANDO -> MORTO.
 */
enum class EstadoInimigo {
    ANDANDO,   /**< Patrulhando; causa dano ao tocar no jogador. */
    ESMAGADO,  /**< Pisado pelo jogador; fica parado um instante e some. */
    MORTO      /**< Morreu por outro motivo; já removido do jogo. */
};

/**
 * @class Inimigo
 * @brief Inimigo que patrulha um trecho do mapa.
 */
class Inimigo : public EntityData {
public:
    /**
     * @brief Cria um inimigo.
     * @param x              Posição horizontal inicial (pixels).
     * @param y              Posição vertical inicial (pixels).
     * @param tipo           Tipo do inimigo (define velocidade e pontos).
     * @param limiteEsquerdo Borda esquerda do trecho de patrulha (pixels).
     * @param limiteDireito  Borda direita do trecho de patrulha (pixels).
     *
     * Se os limites vierem trocados, o construtor os inverte.
     */
    Inimigo(float x, float y, TipoInimigo tipo,
            float limiteEsquerdo, float limiteDireito);

    /**
     * @brief Atualiza o inimigo a cada frame.
     *
     * - ANDANDO: move na direção atual e vira ao chegar num limite.
     * - ESMAGADO: conta o tempo e some depois de um instante.
     * - MORTO: garante que está removido do jogo.
     *
     * @param dt Tempo desde o último frame, em segundos.
     */
    void atualizar(float dt);

    /**
     * @brief Inverte a direção de movimento.
     *
     * Usado internamente nos limites de patrulha e também pelo
     * GerenciadorColisao quando o inimigo bate numa parede.
     */
    void inverterDirecao();

    /**
     * @brief Chamado quando o jogador pisa no inimigo.
     *
     * Muda o estado para ESMAGADO (só tem efeito se estiver ANDANDO).
     */
    void esmagar();

    /**
     * @brief Mata o inimigo na hora (ex.: caiu do mapa) e o remove do jogo.
     */
    void matar();

    /**
     * @brief Informa se o inimigo ainda está andando (perigoso e derrotável).
     * @return true se o estado é ANDANDO.
     *
     * Não confundir com confereVivo() da base, que indica se a entidade
     * ainda existe no jogo (inclusive durante o estado ESMAGADO).
     */
    bool estaAndando() const;

    /** @brief Retorna o tipo de inimigo (Goomba, Koopa). */
    TipoInimigo getTipoInimigo() const;

    /** @brief Retorna o estado atual. */
    EstadoInimigo getEstado() const;

    /** @brief Retorna a direção: -1 (esquerda) ou +1 (direita). */
    int getDirecao() const;

    /** @brief Retorna a velocidade horizontal com sinal (0 se não estiver andando). */
    float getVelocidadeX() const;

    /** @brief Retorna os pontos que o jogador ganha ao derrotar este inimigo. */
    int getPontos() const;

    /** @brief Retorna o dano causado ao jogador (0 se não estiver andando). */
    int getDano() const;

    /** @brief Retorna a largura da caixa de colisão, em pixels. */
    float getLargura() const;

    /** @brief Retorna a altura da caixa de colisão, em pixels. */
    float getAltura() const;

private:
    TipoInimigo _tipoInimigo;  /**< Variação do inimigo. */
    EstadoInimigo _estado;     /**< Estado atual. */
    int _direcao;              /**< -1 = esquerda, +1 = direita. */
    int _pontos;               /**< Pontos ao ser derrotado. */
    int _dano;                 /**< Dano causado ao jogador. */
    float _largura;            /**< Largura da caixa de colisão. */
    float _altura;             /**< Altura da caixa de colisão. */
    float _limiteEsquerdo;     /**< Borda esquerda da patrulha. */
    float _limiteDireito;      /**< Borda direita da patrulha. */
    float _tempoEsmagado;      /**< Tempo no estado ESMAGADO (segundos). */

    /**
     * @brief Velocidade de patrulha de cada tipo (usada no construtor da base).
     * @param tipo Tipo do inimigo.
     * @return Velocidade em pixels por segundo.
     */
    static float velocidadePorTipo(TipoInimigo tipo);
};

#endif  // INIMIGO_HPP