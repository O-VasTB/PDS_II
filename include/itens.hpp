#ifndef ITEM_H
#define ITEM_H

#include <raylib.h>
#include "entity_data.hpp"
#include "jogador.hpp"

/**
 * @file item.hpp
 * @brief Contrato da classe abstrata Item, base de todos os coletáveis.
 */

/**
 * @class Item
 * @brief Classe abstrata base para todos os coletáveis do jogo.
 *
 * Define a interface comum dos itens. Cada item concreto implementa
 * aplicarEfeito() com o seu comportamento, permitindo ao jogo tratar qualquer
 * coletável de forma polimórfica (por meio de ponteiros ou referências para Item).
 */
class Item : public EntityData {
public:
    /**
     * @brief Construtor de Item.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade de movimentação do item (0 para itens estáticos).
     * @param pontos Pontuação concedida ao coletar o item.
     */
    Item(float x, float y, float speed, int pontos);

    /** @brief Destrutor virtual. */
    virtual ~Item() = default;

    /**
     * @brief Aplica o efeito do item ao jogador que o coletou.
     * @param jogador Jogador que coletou o item.
     */
    virtual void aplicarEfeito(Jogador& jogador) = 0;

    /**
     * @brief Atualiza o estado do item (movimento, animação, etc.).
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    virtual void atualizar(float dt);

    /** @brief Desenha o item na tela usando Raylib. */
    virtual void desenhar() const = 0;

    /**
     * @brief Obtém a caixa de colisão do item.
     * @return Rectangle Retângulo com posição e dimensões do item.
     */
    Rectangle getHitbox() const;

    /**
     * @brief Obtém a pontuação concedida pelo item.
     * @return int Pontos concedidos ao coletar.
     */
    int getPontos() const;

protected:
    int _pontos;    /**< Pontuação concedida ao coletar o item. */
    float _largura; /**< Largura da caixa de colisão. */
    float _altura;  /**< Altura da caixa de colisão. */
};


/**
 * @file moeda.hpp
 * @brief Contrato da classe Moeda.
 */

/**
 * @class Moeda
 * @brief Item estático que incrementa o contador de moedas e a pontuação.
 */
class Moeda : public Item {
public:
    /**
     * @brief Construtor de Moeda.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     */
    Moeda(float x, float y);

    /**
     * @brief Adiciona uma moeda e os pontos correspondentes ao jogador, e remove o item.
     * @param jogador Jogador que coletou a moeda.
     */
    void aplicarEfeito(Jogador& jogador) override;

    /** @brief Desenha a moeda na tela usando Raylib. */
    void desenhar() const override;
};


/**
 * @file cogumelo.hpp
 * @brief Contrato da classe Cogumelo.
 */

/**
 * @class Cogumelo
 * @brief Item que se move pelo mapa e faz o jogador crescer ao ser coletado.
 *
 * Se o jogador já estiver grande, concede pontos extras em vez de crescer.
 */
class Cogumelo : public Item {
public:
    /**
     * @brief Construtor de Cogumelo.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade de deslocamento horizontal.
     */
    Cogumelo(float x, float y, float speed);

    /**
     * @brief Faz o jogador crescer (ou concede pontos extras) e remove o item.
     * @param jogador Jogador que coletou o cogumelo.
     */
    void aplicarEfeito(Jogador& jogador) override;

    /**
     * @brief Atualiza a posição horizontal do cogumelo.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void atualizar(float dt) override;

    /** @brief Inverte a direção de deslocamento (ao bater em parede). */
    void inverterDirecao();

    /** @brief Desenha o cogumelo na tela usando Raylib. */
    void desenhar() const override;

private:
    int _direcao; /**< Direção atual de deslocamento (1 ou -1). */
};


/**
 * @file estrela.hpp
 * @brief Contrato da classe Estrela.
 */

/**
 * @class Estrela
 * @brief Item que quica pelo mapa e torna o jogador temporariamente invencível.
 */
class Estrela : public Item {
public:
    /**
     * @brief Construtor de Estrela.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade de deslocamento horizontal.
     */
    Estrela(float x, float y, float speed);

    /**
     * @brief Ativa a invencibilidade temporária do jogador e remove o item.
     * @param jogador Jogador que coletou a estrela.
     */
    void aplicarEfeito(Jogador& jogador) override;

    /**
     * @brief Atualiza a posição da estrela (deslocamento horizontal e quique vertical).
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void atualizar(float dt) override;

    /** @brief Desenha a estrela na tela usando Raylib. */
    void desenhar() const override;

private:
    int _direcao;   /**< Direção atual de deslocamento (1 ou -1). */
    float _velY;    /**< Velocidade vertical usada no quique. */
    float _duracao; /**< Duração, em segundos, da invencibilidade concedida. */
};

#endif

