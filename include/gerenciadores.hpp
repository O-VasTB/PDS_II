#ifndef GERENCIADORES_H
#define GERENCIADORES_H

#include <map>
#include <memory>
#include <vector>
#include <raylib.h>
#include "entity_data.hpp"
#include "jogador.hpp"
#include "inimigo.hpp"
#include "itens.hpp"
#include "ataque.hpp"
#include "mapa.hpp"

/**
 * @file gerenciadores.hpp
 * @brief Contratos dos gerenciadores de colisão e de entrada (input).
 *
 * Ambos isolam responsabilidades que não pertencem às entidades: o
 * GerenciadorColisao decide o que acontece quando duas coisas se encontram, e o
 * GerenciadorInput traduz teclas em ações do jogo.
 */

/* ========================================================================== */
/*                             GERENCIADOR DE COLISÃO                         */
/* ========================================================================== */

/**
 * @class GerenciadorColisao
 * @brief Centraliza a detecção e a resolução de colisões do jogo.
 *
 * Não remove entidades das listas: apenas altera o estado delas (por exemplo,
 * marcando-as como mortas com setAlive(false)). A remoção é feita pelo Jogo.
 */
class GerenciadorColisao {
public:
    /**
     * @brief Construtor padrão.
     */
    GerenciadorColisao() = default;

    /**
     * @brief Verifica se dois retângulos se sobrepõem.
     * @param a Primeiro retângulo.
     * @param b Segundo retângulo.
     * @return true Se houver sobreposição.
     * @return false Caso contrário.
     */
    static bool colide(const Rectangle& a, const Rectangle& b);

    /**
     * @brief Resolve a colisão do jogador com o cenário, corrigindo posição e velocidade.
     *
     * Atualiza a flag de contato com o chão e zera a velocidade vertical ao bater no
     * chão ou no teto.
     * @param jogador Jogador a ser corrigido.
     * @param mapa Mapa com os tiles sólidos.
     */
    void resolverJogadorMapa(Jogador& jogador, const Mapa& mapa) const;

    /**
     * @brief Resolve a colisão de um inimigo com o cenário, invertendo sua direção ao
     *        bater em paredes ou ao chegar à borda de uma plataforma.
     * @param inimigo Inimigo a ser corrigido.
     * @param mapa Mapa com os tiles sólidos.
     */
    void resolverInimigoMapa(Inimigo& inimigo, const Mapa& mapa) const;

    /**
     * @brief Resolve a colisão de um item com o cenário.
     * @param item Item a ser corrigido.
     * @param mapa Mapa com os tiles sólidos.
     */
    void resolverItemMapa(Item& item, const Mapa& mapa) const;

    /**
     * @brief Verifica se um tiro bateu em um tile sólido ou saiu do mapa.
     * @param tiro Tiro a ser testado.
     * @param mapa Mapa com os tiles sólidos.
     * @return true Se o tiro deve ser destruído.
     * @return false Caso contrário.
     */
    bool tiroBateuNoMapa(const Tiro& tiro, const Mapa& mapa) const;

    /**
     * @brief Trata as colisões entre o jogador e todos os inimigos.
     *
     * Se o jogador atinge o inimigo por cima, o inimigo recebe dano, o jogador
     * ganha pontos e quica. Nos demais casos, o jogador leva dano.
     * @param jogador Jogador do jogo.
     * @param inimigos Lista de inimigos ativos.
     */
    void tratarJogadorInimigos(Jogador& jogador,
                               std::vector<std::unique_ptr<Inimigo> >& inimigos) const;

    /**
     * @brief Trata as colisões entre o jogador e os itens, aplicando o efeito de cada
     *        item coletado de forma polimórfica.
     * @param jogador Jogador do jogo.
     * @param itens Lista de itens ativos.
     */
    void tratarJogadorItens(Jogador& jogador,
                            std::vector<std::unique_ptr<Item> >& itens) const;

    /**
     * @brief Trata as colisões entre os tiros e os inimigos.
     *
     * O tiro que acerta é marcado como morto, e o inimigo recebe o dano do tiro.
     * @param tiros Lista de tiros ativos.
     * @param inimigos Lista de inimigos ativos.
     * @return int Pontos ganhos pelos inimigos derrotados nesta chamada.
     */
    int tratarTirosInimigos(std::vector<std::unique_ptr<Tiro> >& tiros,
                            std::vector<std::unique_ptr<Inimigo> >& inimigos) const;

    /**
     * @brief Verifica se o jogador chegou à bandeira de fim de fase.
     * @param jogador Jogador do jogo.
     * @param mapa Mapa da fase.
     * @return true Se o jogador tocou a bandeira.
     * @return false Caso contrário.
     */
    bool jogadorChegouAoFim(const Jogador& jogador, const Mapa& mapa) const;

    /**
     * @brief Verifica se o jogador caiu para fora do mapa.
     * @param jogador Jogador do jogo.
     * @param mapa Mapa da fase.
     * @return true Se o jogador está abaixo do limite inferior do mapa.
     * @return false Caso contrário.
     */
    bool jogadorCaiuDoMapa(const Jogador& jogador, const Mapa& mapa) const;
};

/* ========================================================================== */
/*                               GERENCIADOR DE INPUT                         */
/* ========================================================================== */

/**
 * @enum Acao
 * @brief Ações que o jogador pode realizar por meio do teclado.
 */
enum class Acao {
    Esquerda,  /**< Mover para a esquerda. */
    Direita,   /**< Mover para a direita. */
    Pular,     /**< Pular. */
    Atirar,    /**< Disparar um tiro. */
    Pausar,    /**< Pausar ou retomar a partida. */
    Confirmar, /**< Confirmar uma opção de menu. */
    Cima,      /**< Navegar para cima nos menus. */
    Baixo      /**< Navegar para baixo nos menus. */
};

/**
 * @class GerenciadorInput
 * @brief Traduz as teclas do teclado em ações do jogo.
 *
 * Mantém o mapeamento entre Acao e teclas do Raylib, que pode ser reconfigurado
 * em tempo de execução.
 */
class GerenciadorInput {
public:
    /**
     * @brief Construtor: inicia o mapeamento com as teclas padrão.
     */
    GerenciadorInput();

    /**
     * @brief Verifica se a tecla da ação está sendo mantida pressionada.
     * @param acao Ação a ser consultada.
     * @return true Se a tecla está pressionada neste quadro.
     * @return false Caso contrário.
     */
    bool acaoPressionada(Acao acao) const;

    /**
     * @brief Verifica se a tecla da ação foi apertada neste quadro (evento único).
     * @param acao Ação a ser consultada.
     * @return true Se a tecla acabou de ser apertada.
     * @return false Caso contrário.
     */
    bool acaoAcionada(Acao acao) const;

    /**
     * @brief Associa uma nova tecla a uma ação.
     * @param acao Ação a ser reconfigurada.
     * @param tecla Código da tecla do Raylib (KEY_*).
     * @throw std::invalid_argument Se a tecla for inválida (KEY_NULL ou negativa).
     */
    void setTecla(Acao acao, int tecla);

    /**
     * @brief Obtém a tecla associada a uma ação.
     * @param acao Ação a ser consultada.
     * @return int Código da tecla do Raylib.
     */
    int getTecla(Acao acao) const;

    /**
     * @brief Restaura o mapeamento padrão de teclas.
     */
    void restaurarPadrao();

private:
    std::map<Acao, int> _teclas;    /**< Mapeamento entre ações e teclas do Raylib. */
    std::map<Acao, int> _teclasAlt; /**< Teclas alternativas (WASD). */
};

#endif