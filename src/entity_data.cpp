#include "entity_data.hpp"
#include "excecoes.hpp"
#include <cmath>
#include <map>
#include <string>

EntityData::EntityData(EntityType type, float x, float y, float speed)
    : _type(type), _vel(speed), vivo(true) {
        if (!std::isfinite(x) || !std::isfinite(y)) {
            throw ErroValorInvalido("Posicao inicial invalida (NaN ou infinito)");
        }
        if (!std::isfinite(speed) || speed < 0.0f) {
            throw ErroValorInvalido("A velocidade deve ser um numero finito e nao negativo");
        }
    _posicao.x = x;
    _posicao.y = y;
}

EntityType EntityData::getType() const {
    return _type;
}

Pos EntityData::getPosition() const {
    return _posicao;
}

void EntityData::setPosition(float x, float y) {
    _posicao.x = x;
    _posicao.y = y;
}

bool EntityData::confereVivo() const {
    return vivo;
}

void EntityData::setAlive(bool alive) {
    vivo = alive;
}

namespace {
std::map<std::string, Texture2D> g_texturas; /**< Cache de texturas, por caminho do arquivo. */
}  // namespace

const Texture2D& EntityData::textura(const char* caminho) {
    const std::string chave(caminho);
    std::map<std::string, Texture2D>::iterator it = g_texturas.find(chave);
    if (it == g_texturas.end()) {
        Texture2D t = LoadTexture(caminho);
        if (t.id != 0) {
            SetTextureFilter(t, TEXTURE_FILTER_POINT);  // pixel art nítido
        } else {
            TraceLog(LOG_WARNING, "Sprite nao encontrado: %s (usando desenho simples)", caminho);
        }
        it = g_texturas.insert(std::make_pair(chave, t)).first;  // guarda mesmo se falhar, para avisar só uma vez
    }
    return it->second;
}

void EntityData::liberarTexturas() {
    for (std::map<std::string, Texture2D>::iterator it = g_texturas.begin(); it != g_texturas.end(); ++it) {
        if (it->second.id != 0) {
            UnloadTexture(it->second);
        }
    }
    g_texturas.clear();
}

void EntityData::desenharQuadro(const Texture2D& tex, int coluna, int linha, int larguraQuadro,
                                int alturaQuadro, const Rectangle& destino, bool espelhar) {
    if (tex.id == 0) {
        return;
    }
    const Rectangle origem = {static_cast<float>(coluna * larguraQuadro),
                              static_cast<float>(linha * alturaQuadro),
                              static_cast<float>(espelhar ? -larguraQuadro : larguraQuadro),
                              static_cast<float>(alturaQuadro)};
    DrawTexturePro(tex, origem, destino, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

void EntityData::desenharNosPes(const Texture2D& tex, int coluna, int linha, int larguraQuadro,
                                int alturaQuadro, const Rectangle& hitbox, float escala, bool espelhar) {
    const float w = larguraQuadro * escala;
    const float h = alturaQuadro * escala;
    const Rectangle destino = {hitbox.x + (hitbox.width - w) / 2.0f, hitbox.y + hitbox.height - h, w, h};
    desenharQuadro(tex, coluna, linha, larguraQuadro, alturaQuadro, destino, espelhar);
                                }