/**
 * @file Item.hpp
 * @brief Contrato da classe Item (itens coletáveis do jogo).
 *
 * Item herda de EntityData (entity.hpp) e usa o tipo EntityType::Item.
 *
 * O item fica parado no mapa esperando ser coletado pelo Jogador. Quem
 * detecta a colisão Jogador x Item é o GerenciadorColisao; quando ela
 * acontece, ele chama Item::coletar() e usa o valor retornado para
 * atualizar a pontuação. O Item não conhece o Jogador: só informa quantos
 * pontos vale e de que tipo é (o efeito de crescer/invencibilidade fica
 * a cargo do Jogador).
 */

#ifndef ITEM_HPP
#define ITEM_HPP

#include "entity.hpp"

/**
 * @enum TipoItem
 * @brief Variações de item existentes no jogo.
 */
enum class TipoItem {
    MOEDA,     /**< Dá pontos simples. */
    COGUMELO,  /**< Dá mais pontos; o Jogador decide o efeito (crescer). */
    ESTRELA    /**< Dá muitos pontos; o Jogador decide o efeito (invencibilidade). */
};

/**
 * @class Item
 * @brief Item coletável posicionado no mapa.
 */
class Item : public EntityData {
public:
    /**
     * @brief Cria um item numa posição do mapa.
     * @param x    Posição horizontal (canto superior esquerdo), em pixels.
     * @param y    Posição vertical (canto superior esquerdo), em pixels.
     * @param tipo Tipo do item; define a pontuação.
     */
    Item(float x, float y, TipoItem tipo);

    /**
     * @brief Atualiza o item a cada frame.
     *
     * Por enquanto só avança o relógio de animação (usado pela renderização
     * para trocar de sprite). Item já coletado ou removido não é atualizado.
     *
     * @param dt Tempo desde o último frame, em segundos.
     */
    void atualizar(float dt);

    /**
     * @brief Marca o item como coletado e o remove do jogo.
     *
     * Depois da primeira chamada, confereVivo() passa a retornar false.
     * Chamadas seguintes não dão pontos de novo.
     *
     * @return Pontos ganhos (0 se o item já tinha sido coletado).
     */
    int coletar();

    /** @brief Retorna o tipo do item (a renderização usa para escolher o sprite). */
    TipoItem getTipoItem() const;

    /** @brief Retorna true se o item já foi coletado. */
    bool foiColetado() const;

    /** @brief Retorna quantos pontos o item vale. */
    int getPontos() const;

    /** @brief Retorna a largura da caixa de colisão, em pixels. */
    float getLargura() const;

    /** @brief Retorna a altura da caixa de colisão, em pixels. */
    float getAltura() const;

    /** @brief Retorna há quantos segundos o item existe (para animação). */
    float getTempoAnimacao() const;

private:
    TipoItem _tipoItem;     /**< Tipo do item. */
    int _pontos;            /**< Pontuação concedida ao ser coletado. */
    bool _coletado;         /**< Já foi coletado? */
    float _largura;         /**< Largura da caixa de colisão. */
    float _altura;          /**< Altura da caixa de colisão. */
    float _tempoAnimacao;   /**< Acumulador de tempo para animação (segundos). */

    /**
     * @brief Tabela de pontuação por tipo.
     * @param tipo Tipo do item.
     * @return Pontos que esse tipo vale.
     */
    static int pontosPorTipo(TipoItem tipo);
};

#endif  // ITEM_HPP