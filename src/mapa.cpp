#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include "excecoes.hpp"
#include "mapa.hpp"

namespace {
const float kTamanhoTile = 32.0f; /**< Lado de cada tile, em pixels. */

const std::size_t kMaxColunas = 2000; /**< Largura máxima aceita para um mapa, em tiles. */
const std::size_t kMaxLinhas = 100;   /**< Altura máxima aceita para um mapa, em tiles. */

// Plataformas dinâmicas. Referência de jogo: pulo de ~128 px de altura e ~180 px de alcance.
const float kAlcanceHorizontal = 4.0f * 32.0f; /**< Trajeto das plataformas 'h', em pixels. */
const float kAlcanceVertical = 3.0f * 32.0f;   /**< Trajeto das plataformas 'v', em pixels. */
const float kVelocidadeHorizontal = 80.0f;     /**< Velocidade das plataformas 'h', em pixels/s. */
const float kVelocidadeVertical = 60.0f;       /**< Velocidade das plataformas 'v', em pixels/s. */
const float kTempoAviso = 0.5f;                /**< Tempo entre pisar em um '~' e ele sumir, em segundos. */
const float kTempoAusente = 3.0f;              /**< Tempo que o '~' fica sumido, em segundos. */

/** @brief Converte uma coordenada em pixels no índice do tile correspondente. */
int paraIndice(float valor, float tile) {
    return static_cast<int>(std::floor(valor / tile));
}

/** @brief Indica se o símbolo representa um ponto de surgimento de inimigo ou item. */
bool ehSimboloDeSpawn(char c) {
    return c == 'g' || c == 'k' || c == 'o' || c == 'm' || c == 's';
}

/** @brief Indica se os pés do jogador estão apoiados no topo de uma plataforma. */
bool estaSobre(const Rectangle& jogador, const Rectangle& plataforma) {
    const float pes = jogador.y + jogador.height;
    if (pes > plataforma.y + plataforma.height / 2.0f) {
        return false;  // jogador abaixo do topo da plataforma
    }
    const Rectangle sonda = {jogador.x, pes - 2.0f, jogador.width, 4.0f};
    return CheckCollisionRecs(sonda, plataforma);
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
    _moveis.clear();
    _frageis.clear();
    _colunas = 0;
    _linhas = 0;
    _spawnJogador.x = 0.0f;
    _spawnJogador.y = 0.0f;
    _carregado = false;
}

void Mapa::carregarDeArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho.c_str());
    if (!arquivo.is_open()) {
        throw ErroArquivo("Nao foi possivel abrir o mapa: " + caminho);
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
        throw ErroArquivo("O arquivo do mapa esta vazio: " + caminho);
    }

    if (linhas.size() > kMaxLinhas) {
        throw ErroMapa("Mapa com linhas demais (" + std::to_string(linhas.size()) +
                       "; maximo " + std::to_string(kMaxLinhas) + ") em " + caminho);
    }
    if (linhas[0].size() > kMaxColunas) {
        throw ErroMapa("Mapa com colunas demais (" + std::to_string(linhas[0].size()) +
                       "; maximo " + std::to_string(kMaxColunas) + ") em " + caminho);
    }

    // 2) Valida e converte para estruturas temporárias, para que um arquivo inválido
    //    não corrompa o mapa que já estava carregado.
    const std::size_t largura = linhas[0].size();
    std::vector<std::vector<TipoTile> > tiles;
    std::vector<SpawnEntidade> spawns;
    std::vector<PlataformaMovel> moveis;
    std::vector<PlataformaFragil> frageis;
    Pos spawnJogador = {0.0f, 0.0f};
    bool achouJogador = false;
    bool achouBandeira = false;

    for (std::size_t l = 0; l < linhas.size(); ++l) {
        if (linhas[l].size() != largura) {
            throw ErroMapa("Linha " + std::to_string(l + 1) +
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
                achouBandeira = true;
            } else if (s == '@') {
                if (achouJogador) {
                    throw ErroMapa("Mais de um ponto de partida '@' (linha " + std::to_string(l + 1) +
                                   ", coluna " + std::to_string(c + 1) + ") em " + caminho);
                }
                spawnJogador = pos;
                achouJogador = true;
            } else if (s == 'h' || s == 'v') {
                if (c > 0 && linhas[l][c - 1] == s) {
                    moveis.back().area.width += kTamanhoTile;  // continua a plataforma anterior
                } else {
                    PlataformaMovel pm;
                    pm.area = Rectangle{pos.x, pos.y, kTamanhoTile, kTamanhoTile};
                    pm.origem = pos;
                    pm.horizontal = (s == 'h');
                    pm.alcance = pm.horizontal ? kAlcanceHorizontal : kAlcanceVertical;
                    pm.velocidade = pm.horizontal ? kVelocidadeHorizontal : kVelocidadeVertical;
                    pm.deslocamento = 0.0f;
                    pm.sentido = 1;
                    moveis.push_back(pm);
                }
            } else if (s == '~') {
                PlataformaFragil pf;
                pf.area = Rectangle{pos.x, pos.y, kTamanhoTile, kTamanhoTile};
                pf.estado = EstadoFragil::Estavel;
                pf.tempo = 0.0f;
                frageis.push_back(pf);
            } else if (ehSimboloDeSpawn(s)) {
                SpawnEntidade sp;
                sp.simbolo = s;
                sp.posicao = pos;
                spawns.push_back(sp);
            } else {
                throw ErroMapa(std::string("Simbolo desconhecido '") + s +
                               "' na linha " + std::to_string(l + 1) + ", coluna " +
                               std::to_string(c + 1) + " de " + caminho);
            }
        }
        tiles.push_back(fila);
    }

    if (!achouJogador) {
        throw ErroMapa("O mapa nao possui o ponto de partida '@': " + caminho);
    }

    if (!achouBandeira) {
        throw ErroMapa("O mapa nao possui a bandeira de fim de fase 'F': " + caminho);
    }

    // 3) Tudo certo: substitui o mapa atual.
    limpar();
    _tiles.swap(tiles);
    _spawns.swap(spawns);
    _moveis.swap(moveis);
    _frageis.swap(frageis);
    _colunas = static_cast<int>(largura);
    _linhas = static_cast<int>(linhas.size());
    _spawnJogador = spawnJogador;
    _carregado = true;
}

Pos Mapa::atualizar(float dt, const Rectangle& jogador) {
    Pos carga = {0.0f, 0.0f};
    if (!_carregado || dt <= 0.0f) {
        return carga;
    }

    // Plataformas móveis: o jogador é carregado pela primeira em que estiver apoiado.
    bool carregado = false;
    for (std::size_t i = 0; i < _moveis.size(); ++i) {
        PlataformaMovel& p = _moveis[i];
        const bool apoiado = !carregado && estaSobre(jogador, p.area);
        const Rectangle antes = p.area;

        p.deslocamento += p.sentido * p.velocidade * dt;
        if (p.deslocamento >= p.alcance) {
            p.deslocamento = p.alcance;
            p.sentido = -1;
        } else if (p.deslocamento <= 0.0f) {
            p.deslocamento = 0.0f;
            p.sentido = 1;
        }
        if (p.horizontal) {
            p.area.x = p.origem.x + p.deslocamento;
        } else {
            p.area.y = p.origem.y - p.deslocamento;
        }

        if (apoiado) {
            carga.x = p.area.x - antes.x;
            carga.y = p.area.y - antes.y;
            carregado = true;
        }
    }

    // Plataformas frágeis: pisar dispara a contagem; depois somem e voltam.
    for (std::size_t i = 0; i < _frageis.size(); ++i) {
        PlataformaFragil& f = _frageis[i];
        switch (f.estado) {
            case EstadoFragil::Estavel:
                if (estaSobre(jogador, f.area)) {
                    f.estado = EstadoFragil::Tremendo;
                    f.tempo = kTempoAviso;
                }
                break;
            case EstadoFragil::Tremendo:
                f.tempo -= dt;
                if (f.tempo <= 0.0f) {
                    f.estado = EstadoFragil::Ausente;
                    f.tempo = kTempoAusente;
                }
                break;
            case EstadoFragil::Ausente:
                if (f.tempo > 0.0f) {
                    f.tempo -= dt;
                }
                // Só volta se o jogador não estiver no lugar, para não prendê-lo dentro do tile.
                if (f.tempo <= 0.0f && !CheckCollisionRecs(jogador, f.area)) {
                    f.estado = EstadoFragil::Estavel;
                    f.tempo = 0.0f;
                }
                break;
        }
    }
    return carga;
}

void Mapa::reiniciarDinamicas() {
    for (std::size_t i = 0; i < _moveis.size(); ++i) {
        PlataformaMovel& p = _moveis[i];
        p.deslocamento = 0.0f;
        p.sentido = 1;
        p.area.x = p.origem.x;
        p.area.y = p.origem.y;
    }
    for (std::size_t i = 0; i < _frageis.size(); ++i) {
        _frageis[i].estado = EstadoFragil::Estavel;
        _frageis[i].tempo = 0.0f;
    }
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

    // Plataformas dinâmicas (apenas as que estão na região visível).
    const Rectangle visivel = {canto0.x, canto0.y, canto1.x - canto0.x, canto1.y - canto0.y};
    for (std::size_t i = 0; i < _moveis.size(); ++i) {
        const Rectangle& r = _moveis[i].area;
        if (!CheckCollisionRecs(visivel, r)) {
            continue;
        }
        DrawRectangleRec(r, PURPLE);
        DrawRectangleLinesEx(r, 2.0f, DARKPURPLE);
        DrawRectangle(static_cast<int>(r.x) + 2, static_cast<int>(r.y) + 2,
                      static_cast<int>(r.width) - 4, 4, VIOLET);
    }
    for (std::size_t i = 0; i < _frageis.size(); ++i) {
        const PlataformaFragil& f = _frageis[i];
        if (!CheckCollisionRecs(visivel, f.area)) {
            continue;
        }
        if (f.estado == EstadoFragil::Ausente) {
            DrawRectangleLinesEx(f.area, 1.0f, Fade(DARKGRAY, 0.35f));  // marca onde vai voltar
            continue;
        }
        Rectangle r = f.area;
        Color cor = BEIGE;
        if (f.estado == EstadoFragil::Tremendo) {
            r.x += static_cast<float>(static_cast<int>(GetTime() * 60.0) % 3) - 1.0f;  // treme
            cor = Color{230, 160, 120, 255};
        }
        DrawRectangleRec(r, cor);
        DrawRectangleLinesEx(r, 2.0f, DARKBROWN);
        DrawLine(static_cast<int>(r.x) + 6, static_cast<int>(r.y) + 6,
                 static_cast<int>(r.x) + 16, static_cast<int>(r.y) + 18, DARKBROWN);
        DrawLine(static_cast<int>(r.x) + 16, static_cast<int>(r.y) + 18,
                 static_cast<int>(r.x) + 12, static_cast<int>(r.y) + 28, DARKBROWN);
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
    for (std::size_t i = 0; i < _moveis.size(); ++i) {
        if (CheckCollisionRecs(area, _moveis[i].area)) {
            resultado.push_back(_moveis[i].area);
        }
    }
    for (std::size_t i = 0; i < _frageis.size(); ++i) {
        if (_frageis[i].estado != EstadoFragil::Ausente && CheckCollisionRecs(area, _frageis[i].area)) {
            resultado.push_back(_frageis[i].area);
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

const std::vector<PlataformaMovel>& Mapa::getPlataformasMoveis() const {
    return _moveis;
}

const std::vector<PlataformaFragil>& Mapa::getPlataformasFrageis() const {
    return _frageis;
}