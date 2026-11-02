/**
 * @file mapa.cpp
 * @brief Implementação dos métodos da classe Mapa.
 */

#include "mapa.hpp"
#include <raylib.h>
#include <fstream>
#include <vector>
#include <iostream>

const float TAMANHO_TILE = 32.0f;

Mapa::Mapa() : largura(40), altura(15) {
    // Inicializa a matriz de blocos com um chão padrão na base para testes
    matrizBlocos.resize(altura, std::vector<Bloco>(largura, {false, false, true, 0}));
    
    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            if (y >= altura - 2) {
                matrizBlocos[y][x] = {true, false, false, 1}; // Chão sólido
            }
        }
    }
    
    tileset = { 0 }; // Inicializa textura vazia
}

Mapa::~Mapa() {
    if (tileset.id != 0) {
        UnloadTexture(tileset);
    }
}

bool Mapa::carregarMapa(const std::string& caminhoDados, const std::string& caminhoImagem) {
    if (!caminhoImagem.empty()) {
        tileset = LoadTexture(caminhoImagem.c_str());
    }

    std::ifstream arquivo(caminhoDados);
    if (!arquivo.is_open()) {
        return false;
    }

    matrizBlocos.clear();
    std::string linha;
    std::vector<std::string> linhasTemporarias;

    // 1. Lê todas as linhas e remove automaticamente espaços/tabs à esquerda indesejados
    size_t larguraMaxima = 0;
    while (std::getline(arquivo, linha)) {
        // Remove espaços em branco ou tabs no início da linha gerados pelo editor
        size_t inicio = linha.find_first_not_of(" \t\r");
        if (inicio != std::string::npos) {
            linha = linha.substr(inicio);
        } else {
            linha = ""; // Linha totalmente vazia
        }

        linhasTemporarias.push_back(linha);
        if (linha.size() > larguraMaxima) {
            larguraMaxima = linha.size();
        }
    }
    arquivo.close();

    altura = static_cast<int>(linhasTemporarias.size());
    largura = static_cast<int>(larguraMaxima);

    // 2. Constrói a matriz garantindo o alinhamento perfeito à esquerda
    for (const std::string& l : linhasTemporarias) {
        std::vector<Bloco> linhaBlocos;
        for (size_t j = 0; j < largura; ++j) {
            char c = (j < l.size()) ? l[j] : ' '; 
            Bloco b;
            if (c == '#') {
                b.solido = true;
                b.destrutivel = false;
                b.transponivel = false;
                b.tipo = 1; // Chão
            } else if (c == '?') {
                b.solido = true;
                b.destrutivel = false;
                b.transponivel = false;
                b.tipo = 2; // Bloco interrogação
            } else if (c == 'o') {
                b.solido = false;       
                b.destrutivel = false;
                b.transponivel = true;
                b.tipo = 3; // Moeda
            } else if (c == 'F') {
                b.solido = false;       
                b.destrutivel = false;
                b.transponivel = true;
                b.tipo = 4; // Bandeira / Fim de Fase
            } else {
                b.solido = false;
                b.destrutivel = false;
                b.transponivel = true;
                b.tipo = 0; // Espaço vazio
            }
            linhaBlocos.push_back(b);
        }
        matrizBlocos.push_back(linhaBlocos);
    }

    return true;
}

int Mapa::verificarColetaMoeda(float jogadorX, float jogadorY, float jogadorLargura, float jogadorAltura) {
    const float TAMANHO_TILE = 32.0f;
    int pontosColetados = 0;

    int inicioX = static_cast<int>(jogadorX / TAMANHO_TILE);
    int fimX = static_cast<int>((jogadorX + jogadorLargura) / TAMANHO_TILE);
    int inicioY = static_cast<int>(jogadorY / TAMANHO_TILE);
    int fimY = static_cast<int>((jogadorY + jogadorAltura) / TAMANHO_TILE);

    for (int i = inicioY; i <= fimY; ++i) {
        for (int j = inicioX; j <= fimX; ++j) {
            if (i >= 0 && i < altura && j >= 0 && j < largura) {
                if (matrizBlocos[i][j].tipo == 3) {
                    matrizBlocos[i][j].tipo = 0;       // Transforma em espaço vazio (some)
                    matrizBlocos[i][j].solido = false;
                    matrizBlocos[i][j].transponivel = true;
                    
                    pontosColetados += 100; // Cada moeda vale 100 pontos
                }
            }
        }
    }
    return pontosColetados;
}

bool Mapa::verificarChegadaFim(float jogadorX, float jogadorY, float jogadorLargura, float jogadorAltura) {
    int inicioX = static_cast<int>(jogadorX / TAMANHO_TILE);
    int fimX = static_cast<int>((jogadorX + jogadorLargura) / TAMANHO_TILE);
    int inicioY = static_cast<int>(jogadorY / TAMANHO_TILE);
    int fimY = static_cast<int>((jogadorY + jogadorAltura) / TAMANHO_TILE);

    for (int i = inicioY; i <= fimY; ++i) {
        for (int j = inicioX; j <= fimX; ++j) {
            if (i >= 0 && i < altura && j >= 0 && j < largura) {
                if (matrizBlocos[i][j].tipo == 4) {
                    return true; // Jogador encostou na bandeira!
                }
            }
        }
    }
    return false;
}

void Mapa::desenharBlocosVisiveis(float cameraX, float cameraY) const {
    const float TAMANHO_TILE = 32.0f;

    for (int i = 0; i < matrizBlocos.size(); ++i) {
        for (int j = 0; j < matrizBlocos[i].size(); ++j) {
            const Bloco& bloco = matrizBlocos[i][j];

            if (bloco.tipo == 0) continue;

            float x = j * TAMANHO_TILE;
            float y = i * TAMANHO_TILE;

            float posXFinal = x - cameraX;
            float posYFinal = y - cameraY;

            if (bloco.tipo == 1) { // Chão
                DrawRectangle(static_cast<int>(posXFinal), static_cast<int>(posYFinal), 
                              static_cast<int>(TAMANHO_TILE), static_cast<int>(TAMANHO_TILE), GRAY);
            } 
            else if (bloco.tipo == 2) { // Bloco ?
                DrawRectangle(static_cast<int>(posXFinal), static_cast<int>(posYFinal), 
                              static_cast<int>(TAMANHO_TILE), static_cast<int>(TAMANHO_TILE), GOLD);
            }
            else if (bloco.tipo == 3) { // Moeda
                DrawRectangle(static_cast<int>(posXFinal + 10), static_cast<int>(posYFinal + 10), 
                              12, 12, YELLOW);
            }
            else if (bloco.tipo == 4) { // Bandeira / Fim de Fase
                DrawRectangle(static_cast<int>(posXFinal + 12), static_cast<int>(posYFinal), 
                              8, 32, DARKGRAY); // Mastro
                DrawRectangle(static_cast<int>(posXFinal), static_cast<int>(posYFinal + 4), 
                              14, 14, GREEN);   // Bandeira
            }
        }
    }
}

void Mapa::alterarEstadoBloco(int x, int y) {
    if (x >= 0 && x < largura && y >= 0 && y < altura) {
        if (matrizBlocos[y][x].destrutivel) {
            matrizBlocos[y][x].solido = false;
            matrizBlocos[y][x].destrutivel = false;
            matrizBlocos[y][x].transponivel = true;
        }
    }
}

int Mapa::getLargura() const {
    return largura;
}

int Mapa::getAltura() const {
    return altura;
}