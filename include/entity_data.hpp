#ifndef ENTITY_H
#define ENTITY_H


#include <iostream>
#include <vector>
#include <raylib.h>
#include <string>

/**
 * @file entity.hpp
 * @brief Definição da estrutura de dados base e da classe abstrata de entidades.
 * 
 * Este arquivo contém as estruturas fundamentais para a representação de entidades
 * no jogo 2D, incluindo o enum de tipos, a struct de posição e a classe base EntityData.
 */

/**
 * @enum EntityType
 * @brief Enumeração que identifica o tipo de entidade no jogo.
 * 
 * Utilizada pela lógica de colisão e renderização para determinar o comportamento
 * específico de cada objeto ativo.
 */
enum class EntityType {
    Jogador, /**< Entidade do jogador principal. */
    Tiro,    /**< Entidade de tiro disparado pelo jogador. */
    Inimigo, /**< Entidade de inimigo no mapa. */
    Item    /**< Entidade de colecionáveis. */
};

/**
 * @struct Pos
 * @brief Estrutura bidimensional para coordenadas no espaço 2D.
 */
struct Pos {
    float x; /**< Posição no eixo horizontal X. */
    float y; /**< Posição no eixo vertical Y. */
};

/**
 * @class EntityData
 * @brief Classe base genérica para representação de dados de entidades do jogo.
 * 
 * Agrupa atributos comuns a todas as entidades, como tipo, posição, velocidade
 * e estado de vida, servindo como base polimórfica para as classes derivadas.
 */
class EntityData {
public:
    /**
     * @brief Construtor da classe EntityData e destrutor logo abaixo.
     * @param type Tipo da entidade definido pelo enum EntityType.
     * @param x Posição inicial no eixo X.
     * @param y Posição inicial no eixo Y.
     * @param speed Velocidade de movimentação da entidade.
     */
    EntityData(EntityType type, float x, float y, float speed);
    virtual ~EntityData() = default;

    /**
     * @brief Obtém o tipo da entidade.
     * @return EntityType O tipo da entidade para validação de regras de negócio e colisões.
     */
    EntityType getType() const;

    /**
     * @brief Obtém a posição atual da entidade.
     * @return Pos Estrutura com as coordenadas X e Y atuais.
     */
    Pos getPosition() const;

    /**
     * @brief Atualiza as coordenadas da posição da entidade.
     * @param x Nova posição no eixo X.
     * @param y Nova posição no eixo Y.
     */
    void setPosition(float x, float y);
    
    /**
     * @brief Verifica se a entidade está ativa/viva no jogo.
     * @return true Se a entidade estiver viva.
     * @return false Se a entidade deve ser removida/destruída.
     */
    bool confereVivo() const;

    /**
     * @brief Altera o estado de vida da entidade.
     * @param alive Booleano indicando o novo estado de vida da entidade.
     */
    void setAlive(bool alive);

    /** @brief Carrega (uma única vez) e devolve a textura do arquivo; id == 0 se não existir. */
    static const Texture2D& textura(const char* caminho);

    /** @brief Libera todas as texturas carregadas (chamar antes de CloseWindow). */
    static void liberarTexturas();

    /** @brief Desenha um quadro (coluna, linha) da sheet dentro de 'destino', espelhando se pedido. */
    static void desenharQuadro(const Texture2D& tex, int coluna, int linha, int larguraQuadro,
                               int alturaQuadro, const Rectangle& destino, bool espelhar = false);

    /** @brief Desenha um quadro centralizado na hitbox e alinhado pelos pés, em tamanho natural x escala. */
    static void desenharNosPes(const Texture2D& tex, int coluna, int linha, int larguraQuadro,
                               int alturaQuadro, const Rectangle& hitbox, float escala = 1.0f,
                               bool espelhar = false);

protected:
    EntityType _type; /**< Tipo identificador da entidade. */
    Pos _posicao;     /**< Coordenadas X e Y da entidade no mapa. */
    float _vel;       /**< Velocidade de deslocamento. */
    bool vivo;        /**< Flag indicando se a entidade está ativa no jogo. */
};


//teste de git


#endif