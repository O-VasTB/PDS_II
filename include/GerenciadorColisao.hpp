/**
 * @file GerenciadorColisao.hpp
 * @brief Contrato da classe GerenciadorColisao (física e colisões do jogo).
 *
 * Concentra o que Jogador, Inimigo e Item não fazem sozinhos: gravidade,
 * colisão com chão/paredes do Mapa e colisão entre entidades. Para cada
 * colisão entre jogador e inimigo, decide se foi pisão (chama
 * Inimigo::esmagar()) ou toque (lê Inimigo::getDano()).
 *
 * A detecção é por caixas retangulares (Rectangle da Raylib) e não depende
 * de desenho, o que facilita os testes com doctest.
 */
 
#ifndef GERENCIADOR_COLISAO_HPP
#define GERENCIADOR_COLISAO_HPP
 
#include <vector>
#include <raylib.h>
 
#include "entity.hpp"
#include "Jogador.hpp"
#include "inimigo.hpp"
#include "item.hpp"
#include "mapa.hpp"
#include "functions.hpp"
 
/**
 * @class GerenciadorColisao
 * @brief Aplica gravidade e trata todas as colisões a cada frame.
 */
class GerenciadorColisao {
public:
    /**
     * @brief Verifica se duas caixas de colisão se sobrepõem.
     * @param a Primeira caixa.
     * @param b Segunda caixa.
     * @return true se há sobreposição.
     */
    bool verificarColisao(Rectangle a, Rectangle b) const;
 
    /**
     * @brief Aplica a gravidade a uma entidade.
     * @param entidade Entidade afetada.
     * @param dt Tempo desde o último frame, em segundos.
     */
    void aplicarGravidade(EntityData& entidade, float dt) const;
 
    /**
     * @brief Impede que a entidade atravesse chão e paredes do mapa.
     *
     * Corrige a posição e zera a velocidade no eixo da colisão. Quando um
     * Inimigo bate numa parede, chama inverterDirecao().
     *
     * @param entidade Entidade a ser corrigida.
     * @param mapa Mapa com os blocos sólidos.
     * @return true se houve colisão com algum bloco.
     */
    bool resolverColisaoComMapa(EntityData& entidade, const Mapa& mapa) const;
 
    /**
     * @brief Trata colisões entre o jogador e os inimigos.
     *
     * Se o jogador cai sobre um inimigo ANDANDO, chama esmagar(); caso
     * contrário, aplica o dano de getDano() ao jogador.
     *
     * @param jogador Jogador atual.
     * @param inimigos Inimigos da fase.
     */
    void tratarJogadorInimigos(Jogador& jogador,
                               std::vector<Inimigo>& inimigos) const;
 
    /**
     * @brief Trata colisões entre o jogador e os itens (coleta).
     * @param jogador Jogador atual.
     * @param itens Itens da fase.
     */
    void tratarJogadorItens(Jogador& jogador,
                            std::vector<Item>& itens) const;
 
    /**
     * @brief Executa gravidade e todas as colisões de um frame.
     *
     * Chamado pelo Jogo a cada tick, depois de atualizar as entidades.
     *
     * @param jogador Jogador atual.
     * @param inimigos Inimigos da fase.
     * @param itens Itens da fase.
     * @param mapa Mapa da fase.
     * @param dt Tempo desde o último frame, em segundos.
     */
    void atualizar(Jogador& jogador,
                   std::vector<Inimigo>& inimigos,
                   std::vector<Item>& itens,
                   const Mapa& mapa,
                   float dt);
 
private:
    ColisionAssist _assistente;  /**< Aplica o efeito de colisões genéricas. */
};
 
#endif  // GERENCIADOR_COLISAO_HPP
 