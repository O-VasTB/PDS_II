#ifndef HUD_H
#define HUD_H

#include <string>
#include <raylib.h>
#include "jogador.hpp"

/**
 * @file hud.hpp
 * @brief Contrato da classe Hud, a interface visual exibida durante o jogo.
 */

/**
 * @class Hud
 * @brief Exibe na tela vidas, moedas, pontuação, recorde, fase e tempo restante.
 *
 * A Hud guarda uma cópia dos valores exibidos, atualizados a cada quadro por
 * atualizar(), e não modifica o estado do jogo.
 */
class Hud {
public:
    /**
     * @brief Construtor da Hud.
     * @param larguraTela Largura da janela, em pixels.
     * @param alturaTela Altura da janela, em pixels.
     */
    Hud(int larguraTela, int alturaTela);

    /**
     * @brief Atualiza os valores exibidos.
     * @param jogador Jogador cujos dados serão exibidos.
     * @param tempoRestante Tempo restante da fase, em segundos.
     * @param fase Número da fase atual.
     * @param recorde Maior pontuação já registrada.
     */
    void atualizar(const Jogador& jogador, float tempoRestante, int fase, int recorde);

    /**
     * @brief Desenha a Hud na tela, em coordenadas de tela (fora da câmera).
     */
    void desenhar() const;

    /**
     * @brief Desenha uma mensagem centralizada, usada em pausa, game over e vitória.
     * @param titulo Texto principal, em destaque.
     * @param subtitulo Texto secundário com instruções (pode ser vazio).
     */
    void desenharMensagem(const std::string& titulo, const std::string& subtitulo) const;

private:
    int _larguraTela;      /**< Largura da janela, em pixels. */
    int _alturaTela;       /**< Altura da janela, em pixels. */
    int _vidas;            /**< Vidas exibidas. */
    int _moedas;           /**< Moedas exibidas. */
    int _pontuacao;        /**< Pontuação exibida. */
    int _recorde;          /**< Recorde exibido. */
    int _fase;             /**< Número da fase exibido. */
    float _tempoRestante;  /**< Tempo restante exibido, em segundos. */
    int _tamanhoFonte;     /**< Tamanho da fonte dos textos da Hud. */
};

#endif