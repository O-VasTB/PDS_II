#include "boss.hpp"

namespace {
const float kLargura = 64.0f;
const float kAltura = 96.0f;
const int kVidaMax = 10;
const int kPontos = 1000;

const float kIntervaloBase = 1.8f;   /**< Segundos entre tiros no começo da luta. */
const float kVelTiroBase = 200.0f;   /**< Velocidade do tiro no começo, em px/s. */
const float kAceleracaoMax = 1.5f;   /**< Ganho extra de velocidade com a vida no mínimo (+150%). */
const float kTempoInvulneravel = 0.6f;

const float kTamTiro = 20.0f;
// Distância do chão até o CENTRO do tiro. Ajuste conforme a altura do jogador:
// o tiro alto precisa passar acima da cabeça dele; o baixo, na altura dos pés.
const float kAlturaTiroAlto = 90.0f;
const float kAlturaTiroBaixo = 12.0f;
}  // namespace

/* ------------------------------- TiroBoss -------------------------------- */

TiroBoss::TiroBoss(float x, float y, float speed, int direcao)
    : EntityData(EntityType::Tiro, x, y, speed),
      _direcao(direcao < 0 ? -1 : 1),
      _dano(1),
      _largura(kTamTiro),
      _altura(kTamTiro) {}

void TiroBoss::atualizar(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _posicao.x += _direcao * _vel * dt;
}

void TiroBoss::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const int cx = static_cast<int>(_posicao.x + _largura / 2.0f);
    const int cy = static_cast<int>(_posicao.y + _altura / 2.0f);
    DrawCircle(cx, cy, _largura / 2.0f, RED);
    DrawCircle(cx, cy, _largura / 4.0f, YELLOW);
}

Rectangle TiroBoss::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

int TiroBoss::getDano() const {
    return _dano;
}

/* --------------------------------- Boss ---------------------------------- */

Boss::Boss(float x, float y)
    : EntityData(EntityType::Inimigo, x, y, 0.0f),
      _vida(kVidaMax),
      _vidaMax(kVidaMax),
      _pontos(kPontos),
      _largura(kLargura),
      _altura(kAltura),
      _cooldown(1.5f),
      _invulneravel(0.0f),
      _proximoAlto(true),
      _direcao(-1) {}

float Boss::fator() const {
    const float perdido = static_cast<float>(_vidaMax - _vida) / static_cast<float>(_vidaMax);
    return 1.0f + kAceleracaoMax * perdido;
}

void Boss::atualizar(float dt, float alvoX, std::vector<std::unique_ptr<TiroBoss> >& tiros) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    if (_invulneravel > 0.0f) {
        _invulneravel -= dt;
    }
    _direcao = (alvoX < _posicao.x + _largura / 2.0f) ? -1 : 1;

    _cooldown -= dt;
    if (_cooldown > 0.0f) {
        return;
    }
    const float f = fator();
    _cooldown = kIntervaloBase / f;

    const float chao = _posicao.y + _altura;
    const float centroY = chao - (_proximoAlto ? kAlturaTiroAlto : kAlturaTiroBaixo);
    const float x = (_direcao < 0) ? _posicao.x - kTamTiro : _posicao.x + _largura;
    tiros.push_back(std::unique_ptr<TiroBoss>(
        new TiroBoss(x, centroY - kTamTiro / 2.0f, kVelTiroBase * f, _direcao)));
    _proximoAlto = !_proximoAlto;
}

void Boss::receberDano(int dano) {
    if (dano <= 0 || !confereVivo() || _invulneravel > 0.0f) {
        return;
    }
    _vida -= dano;
    _invulneravel = kTempoInvulneravel;
    if (_vida <= 0) {
        _vida = 0;
        setAlive(false);
    }
}

void Boss::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    // Pisca enquanto está invulnerável.
    if (_invulneravel > 0.0f && static_cast<int>(_invulneravel * 20.0f) % 2 == 0) {
        return;
    }
    const Rectangle hb = getHitbox();
    DrawRectangleRec(hb, PURPLE);
    DrawRectangleLinesEx(hb, 3.0f, DARKPURPLE);

    const int olhoY = static_cast<int>(hb.y) + 22;
    const int olho1 = static_cast<int>(hb.x) + (_direcao > 0 ? 28 : 8);
    DrawRectangle(olho1, olhoY, 12, 12, WHITE);
    DrawRectangle(olho1 + 20, olhoY, 12, 12, WHITE);
    DrawRectangle(olho1 + (_direcao > 0 ? 6 : 0), olhoY + 4, 6, 6, RED);
    DrawRectangle(olho1 + 20 + (_direcao > 0 ? 6 : 0), olhoY + 4, 6, 6, RED);

    // Barra de vida acima do boss.
    const float larg = hb.width;
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y) - 14, static_cast<int>(larg), 8, DARKGRAY);
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y) - 14,
                  static_cast<int>(larg * _vida / _vidaMax), 8, RED);
}

Rectangle Boss::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

int Boss::getVida() const { return _vida; }
int Boss::getVidaMax() const { return _vidaMax; }
int Boss::getPontos() const { return _pontos; }