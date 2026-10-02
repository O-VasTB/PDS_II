/**
 * @file Jogo.hpp
 * @brief Contrato da classe Jogo (loop principal e orquestração).
 *
 * O Jogo é dono do jogador, do mapa, dos inimigos, dos itens e dos
 * subsistemas. A cada frame ele lê a entrada, atualiza as entidades,
 * pede ao GerenciadorColisao que trate física e colisões e desenha tudo
 * via RenderAssist (Raylib).
 */
 
#ifndef JOGO_HPP
#define JOGO_HPP
 
#include <vector>
#include <raylib.h>
 
#include "entity.hpp"
#include "Jogador.hpp"
#include "inimigo.hpp"
#include "item.hpp"
#include "mapa.hpp"
#include "HUD.hpp"
#include "GerenciadorInput.hpp"
#include "GerenciadorColisao.hpp"
#include "functions.hpp"
 
/**
 * @enum EstadoJogo
 * @brief Estados possíveis do jogo.
 */
enum class EstadoJogo {
    MENU,       /**< Tela inicial. */
    JOGANDO,    /**< Fase em andamento. */
    PAUSADO,    /**< Jogo pausado. */
    GAME_OVER,  /**< Jogador perdeu todas as vidas. */
    VITORIA     /**< Jogador completou a fase. */
};
 
/**
 * @class Jogo
 * @brief Classe principal que coordena entrada, atualização, colisão e desenho.
 */
class Jogo {
public:
    /**
     * @brief Cria o jogo no estado MENU.
     */
    Jogo();
 
    /**
     * @brief Abre a janela e mantém o loop principal até o encerramento.
     */
    void executar();
 
    /** @brief Retorna o estado atual do jogo. */
    EstadoJogo getEstado() const;
 
    /**
     * @brief Troca o estado atual (ex.: MENU para JOGANDO).
     * @param novoEstado Estado a ser assumido.
     */
    void setEstado(EstadoJogo novoEstado);
 
    /** @brief Indica se o loop principal ainda está ativo. */
    bool estaRodando() const;
 
    /** @brief Encerra o loop principal. */
    void encerrar();
 
private:
    EstadoJogo _estado;               /**< Estado atual. */
    bool _rodando;                    /**< true enquanto o loop deve continuar. */
 
    Mapa _mapa;                       /**< Mapa da fase. */
    Jogador _jogador;                 /**< Jogador. */
    std::vector<Inimigo> _inimigos;   /**< Inimigos da fase. */
    std::vector<Item> _itens;         /**< Itens da fase. */
 
    HUD _hud;                         /**< Pontos, vidas e demais informações. */
    GerenciadorInput _input;          /**< Leitura do teclado. */
    GerenciadorColisao _colisao;      /**< Gravidade e colisões. */
    RenderAssist _renderizador;       /**< Desenho das entidades. */
 
    /** @brief Carrega o mapa e posiciona jogador, inimigos e itens. */
    void inicializar();
 
    /**
     * @brief Chama GerenciadorInput::processarEntradas() e consulta
     *        isTeclaPressionada() para mover o jogador ou mudar o estado.
     */
    void processarEntrada();
 
    /**
     * @brief Atualiza entidades e colisões e verifica vitória ou derrota.
     * @param dt Tempo desde o último frame, em segundos.
     */
    void atualizar(float dt);
 
    /** @brief Desenha mapa, entidades e HUD. */
    void desenhar();
 
    /** @brief Remove entidades que já saíram do jogo (confereVivo() falso). */
    void removerInativos();
 
    /** @brief Reinicia a fase após game over ou vitória. */
    void reiniciar();
};
 
#endif  // JOGO_HPP