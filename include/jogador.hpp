#ifndef JOGADOR_H
#define JOGADOR_H

#include <memory>
#include <raylib.h>
#include "entity_data.hpp"
#include "ataque.hpp"

/**
 * @file jogador.hpp
 * @brief Contrato da classe Jogador e do enum EstadoPoder.
 */

/**
 * @enum EstadoPoder
 * @brief Estado de poder do jogador, que influencia tamanho e resistência a dano.
 */
enum class EstadoPoder {
    Pequeno, /**< Estado inicial: qualquer dano remove uma vida. */
    Grande   /**< Estado após coletar um cogumelo: o primeiro dano apenas encolhe. */
};

/**
 * @class Jogador
 * @brief Personagem controlado pelo usuário.
 *
 * Armazena vidas, moedas, pontuação, estado de poder e física vertical
 * (velocidade de pulo e contato com o chão). A leitura do teclado é feita fora
 * da classe, que apenas expõe as ações (mover, pular, atirar).
 */
class Jogador : public EntityData {
public:
    /**
     * @brief Construtor de Jogador.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade de movimentação horizontal.
     */
    Jogador(float x, float y, float speed);

    /** @brief Destrutor virtual. */
    virtual ~Jogador() = default;

    /**
     * @brief Move o jogador para a esquerda e atualiza a direção em que ele olha.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void moverEsquerda(float dt);

    /**
     * @brief Move o jogador para a direita e atualiza a direção em que ele olha.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void moverDireita(float dt);

    /** @brief Inicia um pulo, caso o jogador esteja no chão. */
    void pular();

    /**
     * @brief Atualiza a física vertical e os temporizadores (invencibilidade e recarga).
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     * @param gravidade Aceleração da gravidade aplicada à velocidade vertical.
     */
    void atualizar(float dt, float gravidade);

    /**
     * @brief Tenta disparar um tiro.
     * @return std::unique_ptr<Tiro> Novo tiro, ou nullptr se o jogador não puder atirar
     *         (por exemplo, enquanto a recarga não terminou).
     */
    std::unique_ptr<Tiro> atirar();

    /**
     * @brief Aplica dano ao jogador.
     *
     * Ignorado durante a invencibilidade. No estado Grande, o jogador volta a ser
     * Pequeno; no estado Pequeno, perde uma vida.
     */
    void levarDano();

    /** @brief Faz o jogador crescer (efeito do cogumelo). */
    void crescer();

    /**
     * @brief Ativa a invencibilidade temporária (efeito da estrela).
     * @param duracao Duração da invencibilidade, em segundos.
     */
    void ativarInvencibilidade(float duracao);

    /**
     * @brief Adiciona uma moeda ao contador do jogador.
     *
     * A cada 100 moedas, o contador é zerado e o jogador ganha uma vida.
     */
    void adicionarMoeda();

    /**
     * @brief Soma pontos à pontuação do jogador.
     * @param pontos Quantidade de pontos a somar.
     */
    void adicionarPontos(int pontos);

    /** @brief Adiciona uma vida ao jogador. */
    void ganharVida();

    /** @brief Desenha o jogador na tela usando Raylib. */
    void desenhar() const;

    /**
     * @brief Obtém a caixa de colisão do jogador.
     * @return Rectangle Retângulo cujo tamanho depende do estado de poder.
     */
    Rectangle getHitbox() const;

    /**
     * @brief Obtém o número de vidas restantes.
     * @return int Vidas restantes.
     */
    int getVidas() const;

    /**
     * @brief Obtém a quantidade de moedas coletadas.
     * @return int Moedas coletadas (0 a 99).
     */
    int getMoedas() const;

    /**
     * @brief Obtém a pontuação atual.
     * @return int Pontuação acumulada.
     */
    int getPontuacao() const;

    /**
     * @brief Obtém o estado de poder atual.
     * @return EstadoPoder Pequeno ou Grande.
     */
    EstadoPoder getEstado() const;

    /**
     * @brief Verifica se o jogador está invencível.
     * @return true Se a invencibilidade estiver ativa.
     * @return false Caso contrário.
     */
    bool estaInvencivel() const;

    /**
     * @brief Verifica se o jogador está apoiado no chão.
     * @return true Se estiver no chão.
     * @return false Se estiver no ar.
     */
    bool estaNoChao() const;

    /**
     * @brief Define se o jogador está apoiado no chão (chamado pela colisão com blocos).
     * @param noChao Novo valor da flag.
     */
    void setNoChao(bool noChao);

    /**
     * @brief Obtém a velocidade vertical atual.
     * @return float Velocidade vertical (positiva para baixo).
     */
    float getVelY() const;

    /**
     * @brief Define a velocidade vertical (usado, por exemplo, ao quicar em um inimigo).
     * @param velY Nova velocidade vertical.
     */
    void setVelY(float velY);

    /**
     * @brief Obtém a direção para a qual o jogador está virado.
     * @return int 1 (direita) ou -1 (esquerda).
     */
    int getDirecao() const;

private:
    int _vidas;               /**< Vidas restantes. */
    int _moedas;              /**< Moedas coletadas (0 a 99). */
    int _pontuacao;           /**< Pontuação acumulada. */
    int _direcao;             /**< Direção para a qual o jogador olha (1 ou -1). */
    EstadoPoder _estado;      /**< Estado de poder atual. */
    float _velY;              /**< Velocidade vertical. */
    float _forcaPulo;         /**< Impulso aplicado ao pular. */
    bool _noChao;             /**< Indica se está apoiado no chão. */
    float _tempoInvencivel;   /**< Tempo restante de invencibilidade, em segundos. */
    float _recargaTiro;       /**< Tempo restante até poder atirar de novo. */
    float _largura;           /**< Largura da caixa de colisão. */
    float _alturaPequeno;     /**< Altura da caixa de colisão no estado Pequeno. */
    float _alturaGrande;      /**< Altura da caixa de colisão no estado Grande. */
};

#endif

