#include <cmath>
#include "hud.hpp"

namespace {
const int kAlturaBarra = 40;       /**< Altura da barra superior da Hud. */
const int kTamanhoFonteHud = 20;   /**< Tamanho padrão da fonte. */
const int kColunasHud = 6;         /**< Quantidade de campos exibidos na barra. */
}  // namespace

Hud::Hud(int larguraTela, int alturaTela)
    : _larguraTela(larguraTela),
      _alturaTela(alturaTela),
      _vidas(0),
      _moedas(0),
      _pontuacao(0),
      _recorde(0),
      _fase(1),
      _tempoRestante(0.0f),
      _tamanhoFonte(kTamanhoFonteHud) {}

void Hud::atualizar(const Jogador& jogador, float tempoRestante, int fase, int recorde) {
    _vidas = jogador.getVidas();
    _moedas = jogador.getMoedas();
    _pontuacao = jogador.getPontuacao();
    _tempoRestante = (tempoRestante > 0.0f) ? tempoRestante : 0.0f;
    _fase = fase;
    _recorde = recorde;
}

void Hud::desenhar() const {
    DrawRectangle(0, 0, _larguraTela, kAlturaBarra, Fade(BLACK, 0.65f));

    const int largColuna = _larguraTela / kColunasHud;
    const int y = (kAlturaBarra - _tamanhoFonte) / 2;
    const int tempo = static_cast<int>(std::ceil(_tempoRestante));

    DrawText(TextFormat("VIDAS %d", _vidas), 12 + 0 * largColuna, y, _tamanhoFonte, WHITE);
    DrawText(TextFormat("MOEDAS %02d", _moedas), 12 + 1 * largColuna, y, _tamanhoFonte, GOLD);
    DrawText(TextFormat("PONTOS %06d", _pontuacao), 12 + 2 * largColuna, y, _tamanhoFonte, WHITE);
    DrawText(TextFormat("RECORDE %06d", _recorde), 12 + 3 * largColuna, y, _tamanhoFonte, LIGHTGRAY);
    DrawText(TextFormat("FASE %d", _fase), 12 + 4 * largColuna, y, _tamanhoFonte, WHITE);
    DrawText(TextFormat("TEMPO %03d", tempo), 12 + 5 * largColuna, y, _tamanhoFonte,
             tempo <= 30 ? RED : WHITE);
}

void Hud::desenharMensagem(const std::string& titulo, const std::string& subtitulo) const {
    DrawRectangle(0, 0, _larguraTela, _alturaTela, Fade(BLACK, 0.6f));

    const int fonteTitulo = 48;
    const int larguraTitulo = MeasureText(titulo.c_str(), fonteTitulo);
    DrawText(titulo.c_str(), (_larguraTela - larguraTitulo) / 2, _alturaTela / 2 - 50, fonteTitulo, WHITE);

    if (!subtitulo.empty()) {
        const int larguraSub = MeasureText(subtitulo.c_str(), _tamanhoFonte);
        DrawText(subtitulo.c_str(), (_larguraTela - larguraSub) / 2, _alturaTela / 2 + 20,
                 _tamanhoFonte, LIGHTGRAY);
    }
}