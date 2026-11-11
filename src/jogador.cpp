#include "jogador.hpp"

namespace {
const int kVidasIniciais = 3;          /**< Vidas no início de uma partida. */
const float kForcaPulo = 620.0f;       /**< Impulso vertical do pulo, em pixels/s. */
const float kVelocidadeQuedaMax = 900.0f; /**< Velocidade terminal de queda, em pixels/s. */
const float kRecargaTiro = 0.35f;      /**< Intervalo mínimo entre dois tiros, em segundos. */
const float kVelocidadeTiro = 420.0f;  /**< Velocidade horizontal do tiro, em pixels/s. */
const float kTamanhoTiro = 10.0f;      /**< Lado do tiro (deve coincidir com o de Tiro). */
const float kInvencibilidadeDano = 1.5f; /**< Invencibilidade após encolher, em segundos. */
const int kMoedasPorVida = 100;        /**< Moedas necessárias para ganhar uma vida. */
const float kLargura = 26.0f;          /**< Largura da caixa de colisão. */
const float kAlturaPequeno = 30.0f;    /**< Altura no estado Pequeno. */
const float kAlturaGrande = 58.0f;     /**< Altura no estado Grande. */
const int kPontuacaoMaxima = 999999999; /**< Define pontuação máxima para não dar */
bool g_andando = false;                    /**< Houve movimento horizontal neste quadro. */
float g_tempoAnim = 0.0f;                  /**< Tempo contínuo andando, para a animação. */
const float kQuadrosPorSegundo = 8.0f;     /**< Velocidade da animação de andar. */
}  // namespace

Jogador::Jogador(float x, float y, float speed)
    : EntityData(EntityType::Jogador, x, y, speed),
      _vidas(kVidasIniciais),
      _moedas(0),
      _pontuacao(0),
      _direcao(1),
      _estado(EstadoPoder::Pequeno),
      _velY(0.0f),
      _forcaPulo(kForcaPulo),
      _noChao(false),
      _tempoInvencivel(0.0f),
      _recargaTiro(0.0f),
      _largura(kLargura),
      _alturaPequeno(kAlturaPequeno),
      _alturaGrande(kAlturaGrande) {}

void Jogador::moverEsquerda(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _direcao = -1;
    g_andando = true;
    _posicao.x -= _vel * dt;
}

void Jogador::moverDireita(float dt) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }
    _direcao = 1;
    g_andando = true;
    _posicao.x += _vel * dt;
}

void Jogador::pular() {
    if (!confereVivo() || !_noChao) {
        return;
    }
    _velY = -_forcaPulo;
    _noChao = false;
}

void Jogador::atualizar(float dt, float gravidade) {
    if (!confereVivo() || dt <= 0.0f) {
        return;
    }

    _velY += gravidade * dt;
    if (_velY > kVelocidadeQuedaMax) {
        _velY = kVelocidadeQuedaMax;
    }
    _posicao.y += _velY * dt;

    if (_tempoInvencivel > 0.0f) {
        _tempoInvencivel -= dt;
        if (_tempoInvencivel < 0.0f) {
            _tempoInvencivel = 0.0f;
        }
    }
    if (_recargaTiro > 0.0f) {
        _recargaTiro -= dt;
        if (_recargaTiro < 0.0f) {
            _recargaTiro = 0.0f;
        }
    }
    if (g_andando && _noChao) {
        g_tempoAnim += dt;
    } else if (!g_andando) {
        g_tempoAnim = 0.0f;
    }
    g_andando = false;
}

std::unique_ptr<Tiro> Jogador::atirar() {
    if (!confereVivo() || _recargaTiro > 0.0f) {
        return std::unique_ptr<Tiro>();
    }
    _recargaTiro = kRecargaTiro;

    const Rectangle hb = getHitbox();
    const float x = (_direcao > 0) ? hb.x + hb.width : hb.x - kTamanhoTiro;
    const float y = hb.y + hb.height / 2.0f - kTamanhoTiro / 2.0f;
    return std::unique_ptr<Tiro>(new Tiro(x, y, kVelocidadeTiro, _direcao));
}

void Jogador::levarDano() {
    if (!confereVivo() || _tempoInvencivel > 0.0f) {
        return;
    }
    if (_estado == EstadoPoder::Grande) {
        // Encolhe mantendo os pés no mesmo lugar.
        _estado = EstadoPoder::Pequeno;
        _posicao.y += _alturaGrande - _alturaPequeno;
        _tempoInvencivel = kInvencibilidadeDano;
    } else {
        --_vidas;
        setAlive(false);
    }
}

void Jogador::crescer() {
    if (_estado == EstadoPoder::Grande) {
        return;
    }
    // Cresce para cima, mantendo os pés no mesmo lugar.
    _estado = EstadoPoder::Grande;
    _posicao.y -= _alturaGrande - _alturaPequeno;
}

void Jogador::ativarInvencibilidade(float duracao) {
    // Substitui o tempo restante; passar 0 desativa a invencibilidade.
    _tempoInvencivel = (duracao > 0.0f) ? duracao : 0.0f;
}

void Jogador::adicionarMoeda() {
    ++_moedas;
    if (_moedas >= kMoedasPorVida) {
        _moedas -= kMoedasPorVida;
        ganharVida();
    }
}

void Jogador::adicionarPontos(int pontos) {
     if (pontos <= 0) {
       return;
   }
   _pontuacao = (pontos > kPontuacaoMaxima - _pontuacao) ? kPontuacaoMaxima : _pontuacao + pontos;
}

void Jogador::ganharVida() {
    ++_vidas;
}

void Jogador::desenhar() const {
    if (!confereVivo()) {
        return;
    }
    // Pisca enquanto estiver invencível.
    if (_tempoInvencivel > 0.0f && (static_cast<int>(GetTime() * 12.0) % 2) == 0) {
        return;
    }

    const Rectangle hb = getHitbox();
    const bool grande = (_estado == EstadoPoder::Grande);
    const Texture2D& tex = textura(grande ? "Sprites/Mauro_2.png" : "Sprites/Mauro_1.png");
    if (tex.id != 0) {
        const int colunas = grande ? 3 : 2;
        const int altura = grande ? 58 : 30;
        int quadro = 0;
        if (_noChao && g_tempoAnim > 0.0f) {
            quadro = static_cast<int>(g_tempoAnim * kQuadrosPorSegundo) % 4;
        }
        desenharNosPes(tex, quadro % colunas, quadro / colunas, 26, altura, hb, 1.0f, _direcao < 0);
        return;
    }
    DrawRectangleRec(hb, RED);
    DrawRectangle(static_cast<int>(hb.x), static_cast<int>(hb.y), static_cast<int>(hb.width), 8, MAROON);

    const int olhoX = static_cast<int>((_direcao > 0) ? hb.x + hb.width - 10.0f : hb.x + 4.0f);
    const int olhoY = static_cast<int>(hb.y) + 12;
    DrawRectangle(olhoX, olhoY, 6, 6, WHITE);
    DrawRectangle(olhoX + (_direcao > 0 ? 3 : 1), olhoY + 2, 3, 3, BLACK);
}

Rectangle Jogador::getHitbox() const {
    const float altura = (_estado == EstadoPoder::Grande) ? _alturaGrande : _alturaPequeno;
    Rectangle r = {_posicao.x, _posicao.y, _largura, altura};
    return r;
}

int Jogador::getVidas() const {
    return _vidas;
}

int Jogador::getMoedas() const {
    return _moedas;
}

int Jogador::getPontuacao() const {
    return _pontuacao;
}

EstadoPoder Jogador::getEstado() const {
    return _estado;
}

bool Jogador::estaInvencivel() const {
    return _tempoInvencivel > 0.0f;
}

bool Jogador::estaNoChao() const {
    return _noChao;
}

void Jogador::setNoChao(bool noChao) {
    _noChao = noChao;
}

float Jogador::getVelY() const {
    return _velY;
}

void Jogador::setVelY(float velY) {
    _velY = velY;
}

int Jogador::getDirecao() const {
    return _direcao;
}