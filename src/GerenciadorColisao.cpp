/**
 * @file GerenciadorColisao.cpp
 * @brief Implementação das colisões do jogo.
 */

#include "GerenciadorColisao.hpp"
#include "mapa.hpp"
#include <raylib.h>
#include <algorithm>

bool GerenciadorColisao::verificarColisao(Rectangle a, Rectangle b) const {
    return CheckCollisionRecs(a, b);
}

void GerenciadorColisao::aplicarGravidade(EntityData& entidade, float dt) const {
    const float VELOCIDADE_QUEDA = 400.0f; 
    
    Pos pos = entidade.getPosition();
    pos.y += VELOCIDADE_QUEDA * dt;
    
    entidade.setPosition(pos.x, pos.y);
}

// 1. Colisão do Jogador com o Mapa
bool GerenciadorColisao::resolverColisaoComMapa(Jogador& jogador, const Mapa& mapa) const {
    Pos pos = jogador.getPosition();
    float largura = jogador.getLargura();
    float altura = jogador.getAltura();
    const float TAMANHO_TILE = 32.0f;

    float peY = pos.y + altura;
    float peEsquerdaX = pos.x + 4.0f;
    float peDireitaX = pos.x + largura - 4.0f;

    int tileX1 = static_cast<int>(peEsquerdaX / TAMANHO_TILE);
    int tileX2 = static_cast<int>(peDireitaX / TAMANHO_TILE);
    int tileY = static_cast<int>(peY / TAMANHO_TILE);

    bool colidiuComSolido = false;

    if (tileY >= 0 && tileY < mapa.getAltura()) {
        for (int tx = tileX1; tx <= tileX2; ++tx) {
            if (tx >= 0 && tx < mapa.getLargura()) {
                const Bloco& bloco = mapa.getBloco(tileY, tx);
                
                if (bloco.solido) {
                    pos.y = (tileY * TAMANHO_TILE) - altura;
                    jogador.setPosition(pos.x, pos.y);
                    
                    jogador.setNoChao(true);
                    jogador.pararQueda();
                    colidiuComSolido = true;
                    break;
                }
            }
        }
    }

    if (!colidiuComSolido) {
        jogador.setNoChao(false);
    }

    if (pos.x < 0) {
        jogador.setPosition(0, pos.y);
    }

    return colidiuComSolido;
}

// 2. Tratamento genérico para os Inimigos (com proteção de spawn e limites)
bool GerenciadorColisao::resolverColisaoComMapa(EntityData& entidade, const Mapa& mapa) const {
    Pos pos = entidade.getPosition();
    float largura = 32.0f; 
    float altura = 32.0f;  
    const float TAMANHO_TILE = 32.0f;

    float peY = pos.y + altura;
    float peEsquerdaX = pos.x + 2.0f;
    float peDireitaX = pos.x + largura - 2.0f;

    int tileX1 = static_cast<int>(peEsquerdaX / TAMANHO_TILE);
    int tileX2 = static_cast<int>(peDireitaX / TAMANHO_TILE);
    int tileY = static_cast<int>(peY / TAMANHO_TILE);

    if (tileY >= mapa.getAltura()) {
        pos.y = (mapa.getAltura() * TAMANHO_TILE) - altura;
        entidade.setPosition(pos.x, pos.y);
        return true;
    }

    if (tileY >= 0 && tileY < mapa.getAltura()) {
        for (int tx = tileX1; tx <= tileX2; ++tx) {
            if (tx >= 0 && tx < mapa.getLargura()) {
                const Bloco& bloco = mapa.getBloco(tileY, tx);
                
                if (bloco.solido) {
                    pos.y = (tileY * TAMANHO_TILE) - altura;
                    entidade.setPosition(pos.x, pos.y);
                    return true;
                }
            }
        }
    }

    // Segurança contra spawn dentro do bloco: empurra o inimigo para cima se ele nascer preso
    int centroY = static_cast<int>((pos.y + (altura / 2.0f)) / TAMANHO_TILE);
    int centroX = static_cast<int>((pos.x + (largura / 2.0f)) / TAMANHO_TILE);
    if (centroY >= 0 && centroY < mapa.getAltura() && centroX >= 0 && centroX < mapa.getLargura()) {
        if (mapa.getBloco(centroY, centroX).solido) {
            pos.y = (centroY * TAMANHO_TILE) - altura;
            entidade.setPosition(pos.x, pos.y);
            return true;
        }
    }

    return false;
}

void GerenciadorColisao::tratarJogadorInimigos(Jogador& jogador, std::vector<Inimigo>& inimigos) const {
    Pos posJog = jogador.getPosition();
    Rectangle recJogador = { posJog.x, posJog.y, jogador.getLargura(), jogador.getAltura() };

    for (auto& inimigo : inimigos) {
        if (!inimigo.confereVivo()) continue;

        Pos posIni = inimigo.getPosition();
        Rectangle recInimigo = { posIni.x, posIni.y, inimigo.getLargura(), inimigo.getAltura() };

        if (verificarColisao(recJogador, recInimigo)) {
            if (inimigo.estaAndando()) {
                bool foiPisao = posJog.y < posIni.y;

                if (foiPisao) {
                    inimigo.esmagar();
                } else {
                    jogador.tomarDano(inimigo.getDano());
                }
            }
        }
    }
}

void GerenciadorColisao::tratarJogadorItens(Jogador& jogador, std::vector<Item>& itens) const {
    Pos posJog = jogador.getPosition();
    Rectangle recJogador = { posJog.x, posJog.y, 32.0f, 32.0f };

    for (auto& item : itens) {
        if (item.foiColetado() || !item.confereVivo()) continue;

        Pos posItem = item.getPosition();
        Rectangle recItem = { posItem.x, posItem.y, item.getLargura(), item.getAltura() };

        if (verificarColisao(recJogador, recItem)) {
            int pontosObtidos = item.coletar();
            jogador.adicionarPontos(pontosObtidos);
        }
    }
}

void GerenciadorColisao::atualizar(Jogador& jogador,
                                   std::vector<Inimigo>& inimigos,
                                   std::vector<Item>& itens,
                                   const Mapa& mapa,
                                   float dt) {
    
    resolverColisaoComMapa(jogador, mapa);

    for (auto& inimigo : inimigos) {
        if (inimigo.confereVivo()) {
            aplicarGravidade(inimigo, dt);
            resolverColisaoComMapa(inimigo, mapa);
        }
    }

    tratarJogadorInimigos(jogador, inimigos);
    tratarJogadorItens(jogador, itens);
}