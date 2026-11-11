/**
 * @file gerenciadores.cpp
 * @brief Implementação dos gerenciadores de colisão e de input.
 */

#include <cmath>
#include <stdexcept>
#include "excecoes.hpp"
#include "gerenciadores.hpp"
#include "itens.hpp"

namespace {
const float kQuiqueNoInimigo = -350.0f; /**< Velocidade vertical do jogador após pisar em um inimigo. */
const float kFracaoPisao = 0.85f;       /**< Pisão válido: pés dentro dos 85% superiores do inimigo (tolera quedas rápidas). */

/** @brief Converte uma coordenada em pixels no índice do tile correspondente. */
int paraIndice(float valor, float tile) {
    return static_cast<int>(std::floor(valor / tile));
}
}  // namespace

/* ========================================================================== */
/*                             GERENCIADOR DE COLISÃO                         */
/* ========================================================================== */

bool GerenciadorColisao::colide(const Rectangle& a, const Rectangle& b) {
    return CheckCollisionRecs(a, b);
}

void GerenciadorColisao::resolverJogadorMapa(Jogador& jogador, const Mapa& mapa) const {
    if (!jogador.confereVivo() || !mapa.estaCarregado()) {
        return;
    }
    jogador.setNoChao(false);

    // Limites laterais do mapa.
    Rectangle hb = jogador.getHitbox();
    Pos p = jogador.getPosition();
    if (p.x < 0.0f) {
        jogador.setPosition(0.0f, p.y);
    } else if (p.x + hb.width > mapa.getLarguraPx()) {
        jogador.setPosition(mapa.getLarguraPx() - hb.width, p.y);
    }

    // Para cada tile sólido sobreposto, empurra o jogador pelo eixo de menor penetração.
    const std::vector<Rectangle> tiles = mapa.getTilesSolidosProximos(jogador.getHitbox());
    for (std::size_t i = 0; i < tiles.size(); ++i) {
        hb = jogador.getHitbox();  // pode ter mudado após a correção anterior
        if (!CheckCollisionRecs(hb, tiles[i])) {
            continue;
        }
        const Rectangle sobre = GetCollisionRec(hb, tiles[i]);
        p = jogador.getPosition();

        if (sobre.width < sobre.height) {
            // Colisão lateral.
            const bool vindoDaEsquerda = (hb.x + hb.width / 2.0f) < (tiles[i].x + tiles[i].width / 2.0f);
            p.x += vindoDaEsquerda ? -sobre.width : sobre.width;
            jogador.setPosition(p.x, p.y);
        } else {
            const bool porCima = (hb.y + hb.height / 2.0f) < (tiles[i].y + tiles[i].height / 2.0f);
            if (porCima) {
                // Pousou sobre o tile.
                p.y -= sobre.height;
                jogador.setPosition(p.x, p.y);
                if (jogador.getVelY() >= 0.0f) {
                    jogador.setNoChao(true);
                    jogador.setVelY(0.0f);
                }
            } else {
                // Bateu a cabeça no tile.
                p.y += sobre.height;
                jogador.setPosition(p.x, p.y);
                if (jogador.getVelY() < 0.0f) {
                    jogador.setVelY(0.0f);
                }
            }
        }
    }
}

void GerenciadorColisao::resolverInimigoMapa(Inimigo& inimigo, const Mapa& mapa) const {
    if (!inimigo.confereVivo() || !mapa.estaCarregado()) {
        return;
    }
    const float tile = mapa.getTamanhoTile();
    const Rectangle hb = inimigo.getHitbox();

    // Parede: o corpo é reduzido em 1 px na vertical para não colidir com o chão em que pisa.
    Rectangle corpo = hb;
    corpo.height -= 1.0f;
    const std::vector<Rectangle> tiles = mapa.getTilesSolidosProximos(corpo);
    if (!tiles.empty()) {
        const Rectangle sobre = GetCollisionRec(corpo, tiles[0]);
        const bool vindoDaEsquerda = (hb.x + hb.width / 2.0f) < (tiles[0].x + tiles[0].width / 2.0f);
        const Pos p = inimigo.getPosition();
        inimigo.setPosition(p.x + (vindoDaEsquerda ? -sobre.width : sobre.width), p.y);
        inimigo.inverterDirecao();
        return;
    }

    // Borda: se não houver chão logo à frente dos pés, dá meia-volta.
    const float frente = (inimigo.getDirecao() > 0) ? hb.x + hb.width + 1.0f : hb.x - 1.0f;
    const float pes = hb.y + hb.height + 1.0f;
    if (!mapa.ehSolido(paraIndice(frente, tile), paraIndice(pes, tile))) {
        inimigo.inverterDirecao();
    }
}

void GerenciadorColisao::resolverItemMapa(Item& item, const Mapa& mapa) const {
    if (!item.confereVivo() || !mapa.estaCarregado()) {
        return;
    }
    const float tile = mapa.getTamanhoTile();
    Rectangle hb = item.getHitbox();

    // Item que caiu para fora do mapa some.
    if (hb.y > mapa.getAlturaPx()) {
        item.setAlive(false);
        return;
    }

    // Limites laterais: itens móveis dão meia-volta ao chegar à borda do mapa.
    const bool passouEsquerda = hb.x < 0.0f;
    const bool passouDireita = hb.x + hb.width > mapa.getLarguraPx();
    const Pos origem = item.getPosition();

    Cogumelo* cogumelo = dynamic_cast<Cogumelo*>(&item);
    if (cogumelo != NULL) {
        if (passouEsquerda || passouDireita) {
            item.setPosition(passouEsquerda ? 0.0f : mapa.getLarguraPx() - hb.width, origem.y);
            cogumelo->inverterDirecao();
            return;
        }
        Rectangle corpo = hb;
        corpo.height -= 1.0f;
        const std::vector<Rectangle> tiles = mapa.getTilesSolidosProximos(corpo);
        if (!tiles.empty()) {
            const Rectangle sobre = GetCollisionRec(corpo, tiles[0]);
            const bool vindoDaEsquerda = (hb.x + hb.width / 2.0f) < (tiles[0].x + tiles[0].width / 2.0f);
            const Pos p = item.getPosition();
            item.setPosition(p.x + (vindoDaEsquerda ? -sobre.width : sobre.width), p.y);
            cogumelo->inverterDirecao();
            return;
        }
        // Borda: sem chão sob o centro do cogumelo, dá meia-volta.
        const float centro = hb.x + hb.width / 2.0f;
        if (!mapa.ehSolido(paraIndice(centro, tile), paraIndice(hb.y + hb.height + 1.0f, tile))) {
            cogumelo->inverterDirecao();
        }
        return;
    }

    Estrela* estrela = dynamic_cast<Estrela*>(&item);
    if (estrela != NULL) {
        if (passouEsquerda || passouDireita) {
            item.setPosition(passouEsquerda ? 0.0f : mapa.getLarguraPx() - hb.width, origem.y);
            hb = item.getHitbox();
        }
        const std::vector<Rectangle> tiles = mapa.getTilesSolidosProximos(hb);
        for (std::size_t i = 0; i < tiles.size(); ++i) {
            hb = item.getHitbox();
            if (!CheckCollisionRecs(hb, tiles[i])) {
                continue;
            }
            const Rectangle sobre = GetCollisionRec(hb, tiles[i]);
            Pos p = item.getPosition();
            if (sobre.width < sobre.height) {
                const bool vindoDaEsquerda = (hb.x + hb.width / 2.0f) < (tiles[i].x + tiles[i].width / 2.0f);
                p.x += vindoDaEsquerda ? -sobre.width : sobre.width;
                item.setPosition(p.x, p.y);
            } else if ((hb.y + hb.height / 2.0f) < (tiles[i].y + tiles[i].height / 2.0f)) {
                p.y -= sobre.height;
                item.setPosition(p.x, p.y);
            } else {
                p.y += sobre.height;
                item.setPosition(p.x, p.y);
            }
        }
    }
}

bool GerenciadorColisao::tiroBateuNoMapa(const Tiro& tiro, const Mapa& mapa) const {
    const Rectangle hb = tiro.getHitbox();
    if (hb.x < 0.0f || hb.x + hb.width > mapa.getLarguraPx() || hb.y < 0.0f ||
        hb.y > mapa.getAlturaPx()) {
        return true;
    }
    return mapa.colideComSolido(hb);
}

void GerenciadorColisao::tratarJogadorInimigos(
    Jogador& jogador, std::vector<std::unique_ptr<Inimigo> >& inimigos) const {
    for (std::size_t i = 0; i < inimigos.size(); ++i) {
        Inimigo& inimigo = *inimigos[i];
        if (!inimigo.confereVivo() || !jogador.confereVivo()) {
            continue;
        }
        const Rectangle jh = jogador.getHitbox();
        const Rectangle ih = inimigo.getHitbox();
        if (!colide(jh, ih)) {
            continue;
        }

        // Pisão: o jogador está caindo e os pés estão na parte de cima do inimigo.
        const bool pisao = jogador.getVelY() > 0.0f &&
                           (jh.y + jh.height) - ih.y < ih.height * kFracaoPisao;
        if (pisao) {
            inimigo.receberDano(1);
            if (!inimigo.confereVivo()) {
                jogador.adicionarPontos(inimigo.getPontos());
            }
            // Coloca o jogador em cima do inimigo para não colidir de novo no quadro seguinte.
            jogador.setPosition(jogador.getPosition().x, ih.y - jh.height);
            jogador.setVelY(kQuiqueNoInimigo);
        } else {
            jogador.levarDano();  // ignorado se o jogador estiver invencível
        }
    }
}

void GerenciadorColisao::tratarJogadorItens(Jogador& jogador,
                                            std::vector<std::unique_ptr<Item> >& itens) const {
    for (std::size_t i = 0; i < itens.size(); ++i) {
        Item& item = *itens[i];
        if (!item.confereVivo() || !jogador.confereVivo()) {
            continue;
        }
        if (colide(jogador.getHitbox(), item.getHitbox())) {
            item.aplicarEfeito(jogador);  // polimorfismo: cada item tem o seu efeito
        }
    }
}

int GerenciadorColisao::tratarTirosInimigos(
    std::vector<std::unique_ptr<Tiro> >& tiros,
    std::vector<std::unique_ptr<Inimigo> >& inimigos) const {
    int pontos = 0;
    for (std::size_t t = 0; t < tiros.size(); ++t) {
        Tiro& tiro = *tiros[t];
        if (!tiro.confereVivo()) {
            continue;
        }
        for (std::size_t i = 0; i < inimigos.size(); ++i) {
            Inimigo& inimigo = *inimigos[i];
            if (!inimigo.confereVivo() || !colide(tiro.getHitbox(), inimigo.getHitbox())) {
                continue;
            }
            inimigo.receberDano(tiro.getDano());
            tiro.setAlive(false);
            if (!inimigo.confereVivo()) {
                pontos += inimigo.getPontos();
            }
            break;  // cada tiro acerta no máximo um inimigo
        }
    }
    return pontos;
}

bool GerenciadorColisao::jogadorChegouAoFim(const Jogador& jogador, const Mapa& mapa) const {
    return mapa.tocaBandeira(jogador.getHitbox());
}

bool GerenciadorColisao::jogadorCaiuDoMapa(const Jogador& jogador, const Mapa& mapa) const {
    return jogador.getHitbox().y > mapa.getAlturaPx();
}

/* ========================================================================== */
/*                               GERENCIADOR DE INPUT                         */
/* ========================================================================== */

GerenciadorInput::GerenciadorInput() {
    restaurarPadrao();
}

bool GerenciadorInput::acaoPressionada(Acao acao) const {
    const int tecla = getTecla(acao);
    std::map<Acao, int>::const_iterator alt = _teclasAlt.find(acao);
    return (tecla != KEY_NULL && IsKeyDown(tecla)) ||
           (alt != _teclasAlt.end() && IsKeyDown(alt->second));
}

bool GerenciadorInput::acaoAcionada(Acao acao) const {
    const int tecla = getTecla(acao);
    std::map<Acao, int>::const_iterator alt = _teclasAlt.find(acao);
    return (tecla != KEY_NULL && IsKeyPressed(tecla)) ||
           (alt != _teclasAlt.end() && IsKeyPressed(alt->second));
}

void GerenciadorInput::setTecla(Acao acao, int tecla) {
    if (tecla <= KEY_NULL) {
        throw ErroValorInvalido("Tecla invalida para a acao informada");
    }
    _teclas[acao] = tecla;
}

int GerenciadorInput::getTecla(Acao acao) const {
    std::map<Acao, int>::const_iterator it = _teclas.find(acao);
    return (it == _teclas.end()) ? static_cast<int>(KEY_NULL) : it->second;
}

void GerenciadorInput::restaurarPadrao() {
    _teclas.clear();
    _teclas[Acao::Esquerda] = KEY_LEFT;
    _teclas[Acao::Direita] = KEY_RIGHT;
    _teclas[Acao::Pular] = KEY_SPACE;
    _teclas[Acao::Atirar] = KEY_Z;
    _teclas[Acao::Pausar] = KEY_P;
    _teclas[Acao::Confirmar] = KEY_ENTER;
    _teclas[Acao::Cima] = KEY_UP;
    _teclas[Acao::Baixo] = KEY_DOWN;
     _teclasAlt.clear();
    _teclasAlt[Acao::Esquerda] = KEY_A;
    _teclasAlt[Acao::Direita] = KEY_D;
    _teclasAlt[Acao::Pular] = KEY_W;
    _teclasAlt[Acao::Cima] = KEY_W;
    _teclasAlt[Acao::Baixo] = KEY_S;
}