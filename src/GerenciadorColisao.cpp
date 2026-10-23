#include "GerenciadorColisao.hpp"
#include <raylib.h>


bool GerenciadorColisao::verificarColisao(Rectangle a, Rectangle b) const {
    return CheckCollisionRecs(a, b);
}

void GerenciadorColisao::aplicarGravidade(EntityData& entidade, float dt) const {
    const float VELOCIDADE_QUEDA = 400.0f; 
    
    Pos pos = entidade.getPosition();
    pos.y += VELOCIDADE_QUEDA * dt;
    
    entidade.setPosition(pos.x, pos.y);
}

bool GerenciadorColisao::resolverColisaoComMapa(EntityData& entidade, const Mapa& mapa) const {
    Pos pos = entidade.getPosition();
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



//Aqui embaixo fica o gerenciador real das colisões, essa função atualizar que confere todas as colisões em cada frame

void GerenciadorColisao::atualizar(Jogador& jogador,
                                   std::vector<Inimigo>& inimigos,
                                   std::vector<Item>& itens,
                                   const Mapa& mapa,
                                   float dt) {
    
    aplicarGravidade(jogador, dt);
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