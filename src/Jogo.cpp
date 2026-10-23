/**
 * @file Jogo.cpp
 * @brief Implementação da classe Jogo (loop principal e orquestração).
 *
 * Trechos marcados com TODO dependem de Jogador.hpp e mapa.hpp, que ainda
 * não têm os métodos necessários, ou dos caminhos reais dos arquivos.
 */

#include "Jogo.hpp"

#include <algorithm>
#include <raylib.h>

namespace {
    constexpr int LARGURA_JANELA = 800;
    constexpr int ALTURA_JANELA = 450;
    constexpr int FPS = 60;
    constexpr float JOGADOR_X_INICIAL = 50.0f;
    constexpr float JOGADOR_Y_INICIAL = 300.0f;
    constexpr float JOGADOR_VELOCIDADE = 200.0f;
    // TODO(HUD): trocar pelo caminho real da fonte.
    const char* const CAMINHO_FONTE = "assets/fonte.ttf";
}

Jogo::Jogo()
    : _estado(EstadoJogo::MENU),
      _rodando(true),
      _jogador(EntityType::Jogador, JOGADOR_X_INICIAL, JOGADOR_Y_INICIAL,
               JOGADOR_VELOCIDADE) {
}

EstadoJogo Jogo::getEstado() const {
    return _estado;
}

void Jogo::setEstado(EstadoJogo novoEstado) {
    _estado = novoEstado;
}

bool Jogo::estaRodando() const {
    return _rodando;
}

void Jogo::encerrar() {
    _rodando = false;
}

void Jogo::executar() {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Mario");
    SetTargetFPS(FPS);

    inicializar();

    while (_rodando && !WindowShouldClose()) {
        float dt = GetFrameTime();

        processarEntrada();
        if (_estado == EstadoJogo::JOGANDO) {
            atualizar(dt);
        }
        desenhar();
    }

    CloseWindow();
}

void Jogo::inicializar() {
    _inimigos.clear();
    _itens.clear();

    _hud.inicializarRecursos(CAMINHO_FONTE);

    // TODO(Mapa): trocar pelos caminhos reais dos arquivos da fase.
    // _mapa.carregarMapa("assets/fase1.txt", "assets/tileset.png");

    _jogador = Jogador(EntityType::Jogador, JOGADOR_X_INICIAL,
                       JOGADOR_Y_INICIAL, JOGADOR_VELOCIDADE);

    // Inimigos: (x, y, tipo, limiteEsquerdo, limiteDireito)
    _inimigos.push_back(
        Inimigo(300.0f, 300.0f, TipoInimigo::GOOMBA, 250.0f, 450.0f));
    _inimigos.push_back(
        Inimigo(600.0f, 300.0f, TipoInimigo::KOOPA, 550.0f, 750.0f));

    // Itens: (x, y, tipo)
    _itens.push_back(Item(200.0f, 280.0f, TipoItem::MOEDA));
    _itens.push_back(Item(400.0f, 280.0f, TipoItem::COGUMELO));
}

void Jogo::processarEntrada() {
    _input.processarEntradas();

    switch (_estado) {
        case EstadoJogo::MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                setEstado(EstadoJogo::JOGANDO);
            }
            break;

        case EstadoJogo::JOGANDO:
            if (IsKeyPressed(KEY_P)) {
                setEstado(EstadoJogo::PAUSADO);
            }
            if (IsKeyPressed(KEY_SPACE)) {
                _jogador.pular();
            }
            {
                // Movimento horizontal: desloca a posição pela velocidade.
                Pos p = _jogador.getPosition();
                float dt = GetFrameTime();
                if (_input.isTeclaPressionada(KEY_RIGHT)) {
                    p.x += JOGADOR_VELOCIDADE * dt;
                }
                if (_input.isTeclaPressionada(KEY_LEFT)) {
                    p.x -= JOGADOR_VELOCIDADE * dt;
                }
                _jogador.setPosition(p.x, p.y);
            }
            break;

        case EstadoJogo::PAUSADO:
            if (IsKeyPressed(KEY_P)) {
                setEstado(EstadoJogo::JOGANDO);
            }
            break;

        case EstadoJogo::GAME_OVER:
        case EstadoJogo::VITORIA:
            if (IsKeyPressed(KEY_ENTER)) {
                reiniciar();
            }
            break;
    }
}

void Jogo::atualizar(float dt) {
    for (Inimigo& inimigo : _inimigos) {
        inimigo.atualizar(dt);
    }
    for (Item& item : _itens) {
        item.atualizar(dt);
    }

    _colisao.atualizar(_jogador, _inimigos, _itens, _mapa, dt);

    removerInativos();

    // TODO(Jogador): trocar os zeros por moedas e pontuação reais quando o
    // Jogador tiver getters (ex.: getPontuacao()).
    _hud.atualizarDados(_jogador.getVidas(), 0, 0);

    if (_jogador.getVidas() <= 0) {
        setEstado(EstadoJogo::GAME_OVER);
    }
    // TODO(Mapa): se o jogador chegou ao fim da fase,
    // setEstado(EstadoJogo::VITORIA).
}

void Jogo::desenhar() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (_estado) {
        case EstadoJogo::MENU:
            DrawText("MARIO", 330, 150, 40, DARKGRAY);
            DrawText("Pressione ENTER para jogar", 260, 230, 20, GRAY);
            break;

       case EstadoJogo::JOGANDO:
        case EstadoJogo::PAUSADO:
            _mapa.desenharBlocosVisiveis(0.0f, 0.0f);
            
            // Desenha os inimigos (vermelho)
            for (const Inimigo& inimigo : _inimigos) {
                Pos p = inimigo.getPosition();
                DrawRectangle(static_cast<int>(p.x), static_cast<int>(p.y), 
                              static_cast<int>(inimigo.getLargura()), static_cast<int>(inimigo.getAltura()), RED);
            }
            
            // Desenha os itens (dourado)
            for (const Item& item : _itens) {
                Pos p = item.getPosition();
                DrawRectangle(static_cast<int>(p.x), static_cast<int>(p.y), 
                              static_cast<int>(item.getLargura()), static_cast<int>(item.getAltura()), GOLD);
            }
            
            // Desenha o jogador (azul)
            {
                Pos p = _jogador.getPosition();
                DrawRectangle(static_cast<int>(p.x), static_cast<int>(p.y), 
                              static_cast<int>(_jogador.getLargura()), static_cast<int>(_jogador.getAltura()), BLUE);
            }

            _hud.renderizar();
            if (_estado == EstadoJogo::PAUSADO) {
                DrawText("PAUSADO", 330, 200, 30, DARKGRAY);
            }
            break;

        case EstadoJogo::GAME_OVER:
            DrawText("GAME OVER", 300, 180, 40, RED);
            DrawText("Pressione ENTER para reiniciar", 240, 240, 20, GRAY);
            break;

        case EstadoJogo::VITORIA:
            DrawText("VITORIA!", 320, 180, 40, DARKGREEN);
            DrawText("Pressione ENTER para jogar de novo", 220, 240, 20, GRAY);
            break;
    }

    EndDrawing();
}

void Jogo::removerInativos() {
    _inimigos.erase(
        std::remove_if(_inimigos.begin(), _inimigos.end(),
                       [](const Inimigo& i) { return !i.confereVivo(); }),
        _inimigos.end());

    _itens.erase(
        std::remove_if(_itens.begin(), _itens.end(),
                       [](const Item& i) { return i.foiColetado(); }),
        _itens.end());
}

void Jogo::reiniciar() {
    inicializar();
    setEstado(EstadoJogo::JOGANDO);
}