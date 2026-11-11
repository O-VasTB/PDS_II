#include "inimigo.hpp"

namespace {
const float kLargura = 28.0f;       /**< Largura da caixa de colisão. */
const float kAlturaAndarilho = 28.0f;  /**< Altura do Andarilho. */
const float kAlturaEscudo = 30.0f;   /**< Altura do Escudo. */
const int kVidaAndarilho = 1;          /**< Vida do Andarilho. */
const int kVidaEscudo = 2;           /**< Vida do Escudo. */
const int kPontosAndarilho = 100;      /**< Recompensa por derrotar um Andarilho. */
const int kPontosEscudo = 200;       /**< Recompensa por derrotar um Escudo. */
}  // namespace

Inimigo::Inimigo(float x, float y, float speed, TipoInimigo tipo)
    : EntityData(EntityType::Inimigo, x, y, speed),
      _tipoInimigo(tipo),
      _direcao(-1),
      _vida(tipo == TipoInimigo::Escudo ? kVidaEscudo : kVidaAndarilho),
      _pontos(tipo == TipoInimigo::Escudo ? kPontosEscudo : kPontosAndarilho),
      _largura(kLargura),
      _altura(tipo == TipoInimigo::Escudo ? kAlturaEscudo : kAlturaAndarilho) {}

void Inimigo::atualizar(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _posicao.x += _direcao * _vel * dt;
}

void Inimigo::inverterDirecao() {
    _direcao = -_direcao;
}

void Inimigo::receberDano(int dano) {
    if (dano <= 0 || !confereVivo()) {
        return;
    }
    _vida -= dano;
    if (_vida <= 0) {
        _vida = 0;
        setAlive(false);
    }
}

void Inimigo::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    const Rectangle hb = getHitbox();
    const Texture2D& tex = textura(_tipoInimigo == TipoInimigo::Escudo ? "Sprites/Inimigo_2.png"
                                                                         : "Sprites/Inimigo_1.png");
    if (tex.id != 0) {
        const int lado = (_tipoInimigo == TipoInimigo::Escudo) ? 30 : 28;
        const int quadro = static_cast<int>(GetTime() * 5.0 + _posicao.x / 16.0) & 1;
        desenharNosPes(tex, 0, quadro, lado, lado, hb);
        return;
    }
    const Color corpo = (_tipoInimigo == TipoInimigo::Escudo) ? DARKGREEN : BROWN;
    DrawRectangleRec(hb, corpo);

    // "Pés" e olhos viram para o lado em que o inimigo anda.
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y + hb.height - 6.0f),
                  static_cast<int>(hb.width), 6, BLACK);
    const int olhoY = static_cast<int>(hb.y) + 7;
    const int olho1 = static_cast<int>(hb.x) + (_direcao > 0 ? 12 : 4);
    DrawRectangle(olho1, olhoY, 6, 6, WHITE);
    DrawRectangle(olho1 + 9, olhoY, 6, 6, WHITE);
    DrawRectangle(olho1 + (_direcao > 0 ? 3 : 0), olhoY + 2, 3, 3, BLACK);
    DrawRectangle(olho1 + 9 + (_direcao > 0 ? 3 : 0), olhoY + 2, 3, 3, BLACK);
}

Rectangle Inimigo::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

TipoInimigo Inimigo::getTipo() const {
    return _tipoInimigo;
}

int Inimigo::getVida() const {
    return _vida;
}

int Inimigo::getPontos() const {
    return _pontos;
}

int Inimigo::getDirecao() const {
    return _direcao;
}

/* ========================================================================== */
/*                                  TIRO DO BOSS                              */
/* ========================================================================== */

namespace {
const float kBossLargura = 64.0f;
const float kBossAltura = 96.0f;
const int kBossVidaMax = 10;
const int kBossPontos = 1000;
const float kBossIntervaloBase = 1.8f;    /**< Segundos entre tiros no começo da luta. */
const float kBossVelTiroBase = 200.0f;    /**< Velocidade do tiro no começo, em px/s. */
const float kBossAceleracaoMax = 1.5f;    /**< Ganho extra de velocidade com a vida no mínimo (+150%). */
const float kBossTempoInvulneravel = 0.6f;
const float kBossTempoBoca = 0.4f;        /**< Tempo com a boca aberta depois de atirar. */
const float kTiroBossTam = 20.0f;
// Distância do chão até o CENTRO do tiro. O alto passa acima da cabeça do jogador; o baixo, na altura dos pés.
const float kTiroBossAlturaAlto = 90.0f;
const float kTiroBossAlturaBaixo = 12.0f;
}  // namespace

TiroBoss::TiroBoss(float x, float y, float speed, int direcao)
    : EntityData(EntityType::Tiro, x, y, speed),
      _direcao(direcao < 0 ? -1 : 1),
      _dano(1),
      _largura(kTiroBossTam),
      _altura(kTiroBossTam) {}

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
    const Texture2D& tex = textura("Sprites/Ataque_boss.png");
    if (tex.id != 0) {
        const int q = static_cast<int>(GetTime() * 12.0) % 4;
        desenharQuadro(tex, q % 2, q / 2, 20, 20, getHitbox(), _direcao > 0);
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

/* ========================================================================== */
/*                                    BOSS                                    */
/* ========================================================================== */

Boss::Boss(float x, float y)
    : EntityData(EntityType::Inimigo, x, y, 0.0f),
      _vida(kBossVidaMax),
      _vidaMax(kBossVidaMax),
      _pontos(kBossPontos),
      _largura(kBossLargura),
      _altura(kBossAltura),
      _cooldown(1.5f),
      _invulneravel(0.0f),
      _proximoAlto(true),
      _direcao(-1),
      _tempoBoca(0.0f) {}

float Boss::fator() const {
    const float perdido = static_cast<float>(_vidaMax - _vida) / static_cast<float>(_vidaMax);
    return 1.0f + kBossAceleracaoMax * perdido;
}

void Boss::atualizar(float dt, float alvoX, std::vector<std::unique_ptr<TiroBoss> >& tiros) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    if (_invulneravel > 0.0f) {
        _invulneravel -= dt;
    }
    if (_tempoBoca > 0.0f) {
        _tempoBoca -= dt;
    }
    _direcao = (alvoX < _posicao.x + _largura / 2.0f) ? -1 : 1;

    _cooldown -= dt;
    if (_cooldown > 0.0f) {
        return;
    }
    const float f = fator();
    _cooldown = kBossIntervaloBase / f;

    const float chao = _posicao.y + _altura;
    const float centroY = chao - (_proximoAlto ? kTiroBossAlturaAlto : kTiroBossAlturaBaixo);
    const float x = (_direcao < 0) ? _posicao.x - kTiroBossTam : _posicao.x + _largura;
    _tempoBoca = kBossTempoBoca;
    tiros.push_back(std::unique_ptr<TiroBoss>(
        new TiroBoss(x, centroY - kTiroBossTam / 2.0f, kBossVelTiroBase * f, _direcao)));
    _proximoAlto = !_proximoAlto;
}

void Boss::receberDano(int dano) {
    if (dano <= 0 || !confereVivo() || _invulneravel > 0.0f) {
        return;
    }
    _vida -= dano;
    _invulneravel = kBossTempoInvulneravel;
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

    const Texture2D& tex = textura("Sprites/boss.png");
    if (tex.id != 0) {
        desenharQuadro(tex, _tempoBoca > 0.0f ? 1 : 0, 0, 32, 48, hb, _direcao > 0);
    } else {
        DrawRectangleRec(hb, PURPLE);
        DrawRectangleLinesEx(hb, 3.0f, DARKPURPLE);
        const int olhoY = static_cast<int>(hb.y) + 22;
        const int olho1 = static_cast<int>(hb.x) + (_direcao > 0 ? 28 : 8);
        DrawRectangle(olho1, olhoY, 12, 12, WHITE);
        DrawRectangle(olho1 + 20, olhoY, 12, 12, WHITE);
        DrawRectangle(olho1 + (_direcao > 0 ? 6 : 0), olhoY + 4, 6, 6, RED);
        DrawRectangle(olho1 + 20 + (_direcao > 0 ? 6 : 0), olhoY + 4, 6, 6, RED);
    }

    // Barra de vida acima do boss.
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y) - 14, static_cast<int>(hb.width), 8, DARKGRAY);
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y) - 14,
                  static_cast<int>(hb.width * _vida / _vidaMax), 8, RED);
}

Rectangle Boss::getHitbox() const {
    Rectangle r = {_posicao.x, _posicao.y, _largura, _altura};
    return r;
}

int Boss::getVida() const { return _vida; }
int Boss::getVidaMax() const { return _vidaMax; }
int Boss::getPontos() const { return _pontos; }