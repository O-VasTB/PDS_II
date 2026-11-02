#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include "mapa.hpp"

namespace {
const float kTamanhoTile = 32.0f; /**< Lado de cada tile, em pixels. */

/** @brief Converte uma coordenada em pixels no índice do tile correspondente. */
int paraIndice(float valor, float tile) {
    return static_cast<int>(std::floor(valor / tile));
}

/** @brief Indica se o símbolo representa um ponto de surgimento de inimigo ou item. */
bool ehSimboloDeSpawn(char c) {
    return c == 'g' || c == 'k' || c == 'o' || c == 'm' || c == 's';
}

/** @brief Desenha um triângulo independentemente da ordem dos vértices. */
void desenharTriangulo(Vector2 a, Vector2 b, Vector2 c, Color cor) {
    DrawTriangle(a, b, c, cor);
    DrawTriangle(a, c, b, cor);
}
}  // namespace

Mapa::Mapa()
    : _colunas(0), _linhas(0), _tamanhoTile(kTamanhoTile), _carregado(false) {
    _spawnJogador.x = 0.0f;
    _spawnJogador.y = 0.0f;
}

void Mapa::limpar() {
    _tiles.clear();
    _spawns.clear();
    _colunas = 0;
    _linhas = 0;
    _spawnJogador.x = 0.0f;
    _spawnJogador.y = 0.0f;
    _carregado = false;
}

void Mapa::carregarDeArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho.c_str());
    if (!arquivo.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o mapa: " + caminho);
    }

    // 1) Lê todas as linhas, descartando '\r' e espaços no fim de cada uma.
    std::vector<std::string> linhas;
    std::string linha;
    while (std::getline(arquivo, linha)) {
        while (!linha.empty() &&
               (linha[linha.size() - 1] == '\r' || linha[linha.size() - 1] == ' ' ||
                linha[linha.size() - 1] == '\t')) {
            linha.erase(linha.size() - 1);
        }
        linhas.push_back(linha);
    }
    while (!linhas.empty() && linhas.back().empty()) {
        linhas.pop_back();  // ignora linhas em branco no final do arquivo
    }
    if (linhas.empty()) {
        throw std::runtime_error("O arquivo do mapa esta vazio: " + caminho);
    }

    // 2) Valida e converte para estruturas temporárias, para que um arquivo inválido
    //    não corrompa o mapa que já estava carregado.
    const std::size_t largura = linhas[0].size();
    std::vector<std::vector<TipoTile> > tiles;
    std::vector<SpawnEntidade> spawns;
    Pos spawnJogador = {0.0f, 0.0f};
    bool achouJogador = false;

    for (std::size_t l = 0; l < linhas.size(); ++l) {
        if (linhas[l].size() != largura) {
            throw std::invalid_argument("Linha " + std::to_string(l + 1) +
                                        " com tamanho diferente das demais em " + caminho);
        }
        std::vector<TipoTile> fila(largura, TipoTile::Vazio);
        for (std::size_t c = 0; c < largura; ++c) {
            const char s = linhas[l][c];
            Pos pos = {static_cast<float>(c) * kTamanhoTile, static_cast<float>(l) * kTamanhoTile};

            if (s == '.') {
                fila[c] = TipoTile::Vazio;
            } else if (s == '#') {
                fila[c] = TipoTile::Chao;
            } else if (s == '=') {
                fila[c] = TipoTile::Plataforma;
            } else if (s == 'F') {
                fila[c] = TipoTile::Bandeira;
            } else if (s == '@') {
                if (!achouJogador) {  // se houver mais de um '@', vale o primeiro
                    spawnJogador = pos;
                    achouJogador = true;
                }
            } else if (ehSimboloDeSpawn(s)) {
                SpawnEntidade sp;
                sp.simbolo = s;
                sp.posicao = pos;
                spawns.push_back(sp);
            } else {
                throw std::invalid_argument(std::string("Simbolo desconhecido '") + s +
                                            "' na linha " + std::to_string(l + 1) + ", coluna " +
                                            std::to_string(c + 1) + " de " + caminho);
            }
        }
        tiles.push_back(fila);
    }

    if (!achouJogador) {
        throw std::invalid_argument("O mapa nao possui o ponto de partida '@': " + caminho);
    }

    // 3) Tudo certo: substitui o mapa atual.
    limpar();
    _tiles.swap(tiles);
    _spawns.swap(spawns);
    _colunas = static_cast<int>(largura);
    _linhas = static_cast<int>(linhas.size());
    _spawnJogador = spawnJogador;
    _carregado = true;
}

void Mapa::desenhar(const Camera2D& camera) const {
    if (!_carregado) {
        return;
    }
    const Vector2 canto0 = GetScreenToWorld2D(Vector2{0.0f, 0.0f}, camera);
    const Vector2 canto1 = GetScreenToWorld2D(
        Vector2{static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, camera);

    const int c0 = std::max(0, paraIndice(canto0.x, _tamanhoTile));
    const int c1 = std::min(_colunas - 1, paraIndice(canto1.x, _tamanhoTile));
    const int l0 = std::max(0, paraIndice(canto0.y, _tamanhoTile));
    const int l1 = std::min(_linhas - 1, paraIndice(canto1.y, _tamanhoTile));

    for (int l = l0; l <= l1; ++l) {
        for (int c = c0; c <= c1; ++c) {
            const TipoTile tipo = _tiles[l][c];
            const Rectangle r = {c * _tamanhoTile, l * _tamanhoTile, _tamanhoTile, _tamanhoTile};

            if (tipo == TipoTile::Chao) {
                DrawRectangleRec(r, BROWN);
                DrawRectangleLinesEx(r, 1.0f, DARKBROWN);
                if (!ehSolido(c, l - 1)) {
                    DrawRectangle(static_cast<int>(r.x), static_cast<int>(r.y),
                                  static_cast<int>(r.width), 6, DARKGREEN);
                }
            } else if (tipo == TipoTile::Plataforma) {
                DrawRectangleRec(r, ORANGE);
                DrawRectangleLinesEx(r, 1.0f, MAROON);
            } else if (tipo == TipoTile::Bandeira) {
                DrawRectangle(static_cast<int>(r.x) + 14, static_cast<int>(r.y), 4,
                              static_cast<int>(r.height), LIGHTGRAY);
                if (getTile(c, l - 1) != TipoTile::Bandeira) {
                    // Topo do mastro: desenha a flâmula.
                    desenharTriangulo(Vector2{r.x + 18.0f, r.y + 2.0f}, Vector2{r.x + 18.0f, r.y + 16.0f},
                                      Vector2{r.x + 32.0f, r.y + 9.0f}, RED);
                }
            }
        }
    }
}

TipoTile Mapa::getTile(int coluna, int linha) const {
    if (!_carregado || coluna < 0 || linha < 0 || coluna >= _colunas || linha >= _linhas) {
        return TipoTile::Vazio;
    }
    return _tiles[linha][coluna];
}

bool Mapa::ehSolido(int coluna, int linha) const {
    const TipoTile t = getTile(coluna, linha);
    return t == TipoTile::Chao || t == TipoTile::Plataforma;
}

bool Mapa::colideComSolido(const Rectangle& area) const {
    return !getTilesSolidosProximos(area).empty();
}

std::vector<Rectangle> Mapa::getTilesSolidosProximos(const Rectangle& area) const {
    std::vector<Rectangle> resultado;
    if (!_carregado) {
        return resultado;
    }
    const int c0 = std::max(0, paraIndice(area.x, _tamanhoTile));
    const int c1 = std::min(_colunas - 1, paraIndice(area.x + area.width, _tamanhoTile));
    const int l0 = std::max(0, paraIndice(area.y, _tamanhoTile));
    const int l1 = std::min(_linhas - 1, paraIndice(area.y + area.height, _tamanhoTile));

    for (int l = l0; l <= l1; ++l) {
        for (int c = c0; c <= c1; ++c) {
            if (!ehSolido(c, l)) {
                continue;
            }
            const Rectangle tile = {c * _tamanhoTile, l * _tamanhoTile, _tamanhoTile, _tamanhoTile};
            if (CheckCollisionRecs(area, tile)) {
                resultado.push_back(tile);
            }
        }
    }
    return resultado;
}

bool Mapa::tocaBandeira(const Rectangle& area) const {
    if (!_carregado) {
        return false;
    }
    const int c0 = std::max(0, paraIndice(area.x, _tamanhoTile));
    const int c1 = std::min(_colunas - 1, paraIndice(area.x + area.width, _tamanhoTile));
    const int l0 = std::max(0, paraIndice(area.y, _tamanhoTile));
    const int l1 = std::min(_linhas - 1, paraIndice(area.y + area.height, _tamanhoTile));

    for (int l = l0; l <= l1; ++l) {
        for (int c = c0; c <= c1; ++c) {
            if (_tiles[l][c] != TipoTile::Bandeira) {
                continue;
            }
            const Rectangle tile = {c * _tamanhoTile, l * _tamanhoTile, _tamanhoTile, _tamanhoTile};
            if (CheckCollisionRecs(area, tile)) {
                return true;
            }
        }
    }
    return false;
}

bool Mapa::estaCarregado() const {
    return _carregado;
}

float Mapa::getTamanhoTile() const {
    return _tamanhoTile;
}

int Mapa::getColunas() const {
    return _colunas;
}

int Mapa::getLinhas() const {
    return _linhas;
}

float Mapa::getLarguraPx() const {
    return _colunas * _tamanhoTile;
}

float Mapa::getAlturaPx() const {
    return _linhas * _tamanhoTile;
}

Pos Mapa::getSpawnJogador() const {
    return _spawnJogador;
}

const std::vector<SpawnEntidade>& Mapa::getSpawns() const {
    return _spawns;
}