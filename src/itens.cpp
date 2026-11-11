#include <cmath>
#include "itens.hpp"

/* ========================================================================== */
/*                                    ITEM                                    */

namespace {
const float kTamanhoPadrao = 24.0f; /**< Lado padrão da caixa de colisão de um item. */
}  // namespace

Item::Item(float x, float y, float speed, int pontos)
    : EntityData(EntityType::Item, x, y, speed),
      _pontos(pontos < 0 ? 0 : pontos),
      _largura(kTamanhoPadrao),
      _altura(kTamanhoPadrao) {}

void Item::atualizar(float dt) {
    // Comportamento padrão: item estático. Itens móveis sobrescrevem este método.
    (void)dt;
}

Rectangle Item::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

int Item::getPontos() const {
    return _pontos;
}

/* ========================================================================== */
/*                                   MOEDA                                    */

namespace {
const int kPontosMoeda = 200;      /**< Pontos concedidos por moeda. */
const float kLarguraMoeda = 20.0f; /**< Largura da caixa de colisão. */
const float kAlturaMoeda = 24.0f;  /**< Altura da caixa de colisão. */
}  // namespace

Moeda::Moeda(float x, float y) : Item(x, y, 0.0f, kPontosMoeda) {
    _largura = kLarguraMoeda;
    _altura = kAlturaMoeda;
}

void Moeda::aplicarEfeito(Jogador& jogador) {
    if (!confereVivo()) {
        return;
    }
    jogador.adicionarMoeda();
    jogador.adicionarPontos(_pontos);
    setAlive(false);
}

void Moeda::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const Texture2D& tex = textura("Sprites/Moeda.png");
    if (tex.id != 0) {
        const float giro = std::fabs(std::sin(static_cast<float>(GetTime()) * 6.0f));
        const float largura = _largura * (0.25f + 0.75f * giro);  // mantém o efeito de girar
        const Rectangle destino = {_posicao.x + (_largura - largura) / 2.0f, _posicao.y, largura, _altura};
        desenharQuadro(tex, 0, 0, 20, 24, destino);
        return;
    }
    // Largura oscila para simular a moeda girando.
    const float giro = std::fabs(std::sin(static_cast<float>(GetTime()) * 6.0f));
    const int cx = static_cast<int>(_posicao.x + _largura / 2.0f);
    const int cy = static_cast<int>(_posicao.y + _altura / 2.0f);
    const float raioH = (_largura / 2.0f) * (0.25f + 0.75f * giro);
    DrawEllipse(cx, cy, raioH, _altura / 2.0f, GOLD);
    DrawEllipse(cx, cy, raioH * 0.55f, _altura / 2.0f - 4.0f, YELLOW);
}

/* ========================================================================== */
/*                                  COGUMELO                                  */

namespace {
const int kPontosCogumelo = 1000;  /**< Pontos concedidos ao coletar. */
const float kTamanhoCogumelo = 26.0f; /**< Lado da caixa de colisão. */
}  // namespace

Cogumelo::Cogumelo(float x, float y, float speed)
    : Item(x, y, speed, kPontosCogumelo), _direcao(1) {
    _largura = kTamanhoCogumelo;
    _altura = kTamanhoCogumelo;
}

void Cogumelo::aplicarEfeito(Jogador& jogador) {
    if (!confereVivo()) {
        return;
    }
    if (jogador.getEstado() == EstadoPoder::Pequeno) {
        jogador.crescer();
        jogador.adicionarPontos(_pontos);
    } else {
        // Já está grande: o cogumelo vale o dobro de pontos.
        jogador.adicionarPontos(_pontos * 2);
    }
    setAlive(false);
}

void Cogumelo::atualizar(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _posicao.x += _direcao * _vel * dt;
}

void Cogumelo::inverterDirecao() {
    _direcao = -_direcao;
}

void Cogumelo::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const Texture2D& tex = textura("Sprites/Cogumelo.png");
    if (tex.id != 0) {
        desenharQuadro(tex, 0, 0, 26, 26, getHitbox());
        return;
    }
    const int x = static_cast<int>(_posicao.x);
    const int y = static_cast<int>(_posicao.y);
    const int w = static_cast<int>(_largura);
    const int h = static_cast<int>(_altura);

    DrawRectangle(x + 5, y + h / 2, w - 10, h / 2, BEIGE);                 // caule
    DrawRectangle(x, y + 2, w, h / 2 + 2, RED);                            // chapéu
    DrawRectangle(x + 4, y + 5, 6, 6, WHITE);                              // bolinhas
    DrawRectangle(x + w - 10, y + 5, 6, 6, WHITE);
}

/* ========================================================================== */
/*                                  ESTRELA                                   */

namespace {
const int kPontosEstrela = 1000;        /**< Pontos concedidos ao coletar. */
const float kTamanhoEstrela = 26.0f;    /**< Lado da caixa de colisão. */
const float kDuracaoInvenc = 8.0f;      /**< Duração da invencibilidade concedida, em segundos. */
const float kGravidadeEstrela = 1200.0f; /**< Gravidade aplicada à estrela, em pixels/s². */
const float kImpulsoQuique = 420.0f;    /**< Impulso vertical do quique, em pixels/s. */
const float kVelocidadeQuedaMax = 700.0f; /**< Velocidade terminal de queda. */

/** @brief Desenha um triângulo independentemente da ordem (horária ou anti-horária) dos vértices. */
void desenharTriangulo(Vector2 a, Vector2 b, Vector2 c, Color cor) {
    DrawTriangle(a, b, c, cor);
    DrawTriangle(a, c, b, cor);
}
}  // namespace

Estrela::Estrela(float x, float y, float speed)
    : Item(x, y, speed, kPontosEstrela), _direcao(1), _velY(0.0f), _duracao(kDuracaoInvenc) {
    _largura = kTamanhoEstrela;
    _altura = kTamanhoEstrela;
}

void Estrela::aplicarEfeito(Jogador& jogador) {
    if (!confereVivo()) {
        return;
    }
    jogador.ativarInvencibilidade(_duracao);
    jogador.adicionarPontos(_pontos);
    setAlive(false);
}

void Estrela::atualizar(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _velY += kGravidadeEstrela * dt;
    if (_velY > kVelocidadeQuedaMax) {
        _velY = kVelocidadeQuedaMax;
    }
    _posicao.x += _direcao * _vel * dt;
    _posicao.y += _velY * dt;
}


void Estrela::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const float pi = 3.14159265f;
    const Vector2 centro = {_posicao.x + _largura / 2.0f, _posicao.y + _altura / 2.0f};
    const float raioExterno = _largura / 2.0f;
    const float raioInterno = raioExterno * 0.45f;
    const float giro = static_cast<float>(GetTime()) * 2.0f;

    // Estrela de 5 pontas: 10 vértices alternando raio externo e interno.
    Vector2 pontos[10];
    for (int i = 0; i < 10; ++i) {
        const float angulo = -pi / 2.0f + i * (pi / 5.0f);
        const float raio = (i % 2 == 0) ? raioExterno : raioInterno;
        pontos[i].x = centro.x + std::cos(angulo) * raio;
        pontos[i].y = centro.y + std::sin(angulo) * raio;
    }
    // Brilho alterna entre amarelo e dourado.
    const Color cor = (static_cast<int>(giro * 3.0f) % 2 == 0) ? YELLOW : GOLD;
    for (int i = 0; i < 10; ++i) {
        desenharTriangulo(centro, pontos[i], pontos[(i + 1) % 10], cor);
    }
}