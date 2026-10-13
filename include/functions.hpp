#ifndef ENGINE_H
#define ENGINE_H

/**
 * @file engine_head.hpp
 * @brief Declaração de classes auxiliares para subsistemas da engine (Colisão e Renderização).
 * 
 * Este arquivo define os módulos responsáveis por gerenciar a lógica de interações 
 * físicas entre entidades e a renderização gráfica via biblioteca Raylib.
 */

#include "entity_head.hpp"
#include <raylib.h>

/**
 * @class ColisionAssist
 * @brief Classe responsável pelo processamento e tratamento de colisões entre entidades.
 * 
 * Centraliza as regras de negócio de colisão do jogo, aplicando os efeitos
 * apropriados (como alteração de vida ou remoção) nas entidades envolvidas.
 */
class ColisionAssist {
public:
    /**
     * @brief Calcula e aplica o resultado da colisão entre duas entidades.
     * 
     * Avalia o tipo do receptor e do agressor para determinar os danos
     * ou efeitos colaterais resultantes da interação.
     * 
     * @param receptor Referência para a entidade que está recebendo o impacto/ação.
     * @param agressor Referência para a entidade que causou a colisão.
     */
    void calcColisao(EntityData& receptor, EntityData& agressor);
};

/**
 * @class RenderAssist
 * @brief Subsystema responsável pela renderização visual dos elementos do jogo.
 * 
 * Utiliza as rotinas gráficas da Raylib para desenhar as entidades no buffer de tela
 * de acordo com seus tipos e posições atuais.
 */
class RenderAssist {
public:
    /**
     * @brief Desenha uma entidade individual na tela.
     * 
     * Mapeia o tipo da entidade recebida para a sua forma e cor correspondentes
     * na biblioteca Raylib (ex: retângulos para jogador/inimigos, círculos para moedas).
     * 
     * @param entidade Referência constante para a entidade a ser desenhada.
     */
    void desenharEntidade(const EntityData& entidade);
};

#endif