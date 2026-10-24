/**
 * @file Jogo.cpp
 * @brief Implementação da classe Jogo (loop principal e orquestração).
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
    const char* const CAMINHO_FONTE = "assets/fonte.ttf";
    int _pontos;
}

Jogo::Jogo()
    : _estado(EstadoJogo::MENU),
      _rodando(true),
      _jogador(EntityType::Jogador, JOGADOR_X_INICIAL, JOGADOR_Y_INICIAL,
               JOGADOR_VELOCIDADE) {
      _mapa.carregarMapa("fase1.txt", "");
      _pontos = 0;
      _vidas = 3;
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

    // --- RECARREGA O MAPA PARA RESTAURAR AS MOEDAS ---
    _mapa.carregarMapa("fase1.txt", "");

    _hud.inicializarRecursos(CAMINHO_FONTE);

    _jogador = Jogador(EntityType::Jogador, JOGADOR_X_INICIAL,
                       JOGADOR_Y_INICIAL, JOGADOR_VELOCIDADE);

    _vidas = 3;
    _pontos = 0;

    // Inimigos: (x, y, tipo, limiteEsquerdo, limiteDireito)
    _inimigos.push_back(
        Inimigo(300.0f, 300.0f, TipoInimigo::GOOMBA, 250.0f, 450.0f));
    _inimigos.push_back(
        Inimigo(600.0f, 300.0f, TipoInimigo::KOOPA, 550.0f, 750.0f));

    // Configuração inicial da câmera
    _camera.target = { _jogador.getPosition().x, _jogador.getPosition().y };
    _camera.offset = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f };
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
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
    if (_estado != EstadoJogo::JOGANDO) return;

    _jogador.atualizar(dt);

    for (Inimigo& inimigo : _inimigos) {
        inimigo.atualizar(dt);

        // --- ADICIONA PONTOS AO ESMAGAR O INIMIGO ---
        if (inimigo.getEstado() == EstadoInimigo::ESMAGADO && !inimigo.foiPontuado()) {
            _pontos += inimigo.getPontos();
            inimigo.marcarPontuado();
        }
    }
    
    for (Item& item : _itens) {
        item.atualizar(dt);
    }

    int vidasAntes = _vidas;

    // O GerenciadorColisao trata das colisões (inimigos, itens, mapa)
    _colisao.atualizar(_jogador, _inimigos, _itens, _mapa, dt);

    _vidas = _jogador.getVidas();

    // Se perdeu vida neste frame por causa do inimigo
    if (_vidas < vidasAntes) {
        if (_vidas > 0) {
            _jogador.setPosition(64.0f, 100.0f); // Reposiciona para evitar dano contínuo
        } else {
            _vidas = 0;
            setEstado(EstadoJogo::GAME_OVER);
        }
    }

   // Verificação de moedas
    int pontosGanhos = _mapa.verificarColetaMoeda(_jogador.getPosition().x, _jogador.getPosition().y, 
                                                 _jogador.getLargura(), _jogador.getAltura());
    if (pontosGanhos > 0) {
        _pontos += pontosGanhos; 
    }

    // --- VERIFICAÇÃO DE VITÓRIA (BANDEIRA NO MAPA) ---
    if (_mapa.verificarChegadaFim(_jogador.getPosition().x, _jogador.getPosition().y, 
                                  _jogador.getLargura(), _jogador.getAltura())) {
        setEstado(EstadoJogo::VITORIA);
    }

    removerInativos();

// --- ATUALIZAÇÃO DA CÂMERA ---
    _camera.target.x = std::max(400.0f, _jogador.getPosition().x); 
    _camera.target.y = 100.0f; // Trava a câmera verticalmente no meio da tela (450 / 2)

    // --- VERIFICAÇÃO DE QUEDA (BURACO) ---
    float alturaMaximaMapa = _mapa.getAltura() * 32.0f;

    if (_jogador.getPosition().y > alturaMaximaMapa) {
        _vidas--;                  
        _jogador.setVidas(_vidas); 

        if (_vidas > 0) {
            _jogador.setPosition(64.0f, 100.0f); 
        } else {
            _vidas = 0;
            setEstado(EstadoJogo::GAME_OVER);
        }
    }

    if (_vidas <= 0) {
        setEstado(EstadoJogo::GAME_OVER);
    }

    _hud.atualizarDados(_vidas, 0, _pontos);
    
}

void Jogo::desenhar() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (_estado == EstadoJogo::MENU) {
        DrawText("MARIO", 330, 150, 40, DARKGRAY);
        DrawText("Pressione ENTER para jogar", 260, 230, 20, GRAY);
    }
    else if (_estado == EstadoJogo::GAME_OVER) {
        DrawText("GAME OVER", 300, 180, 40, RED);
        DrawText("Pressione ENTER para reiniciar", 240, 240, 20, GRAY);
    }
    else if (_estado == EstadoJogo::VITORIA) {
        DrawText("VITORIA!", 320, 180, 40, DARKGREEN);
        DrawText("Pressione ENTER para jogar de novo", 220, 240, 20, GRAY);
    }
    else if (_estado == EstadoJogo::JOGANDO || _estado == EstadoJogo::PAUSADO) {
        BeginMode2D(_camera);

        _mapa.desenharBlocosVisiveis(0.0f, 0.0f);
        
        for (const Inimigo& inimigo : _inimigos) {
            Pos p = inimigo.getPosition();
            DrawRectangle(static_cast<int>(p.x), static_cast<int>(p.y), 
                          static_cast<int>(inimigo.getLargura()), static_cast<int>(inimigo.getAltura()), RED);
        }
        
        for (const Item& item : _itens) {
            Pos p = item.getPosition();
            DrawRectangle(static_cast<int>(p.x), static_cast<int>(p.y), 
                          static_cast<int>(item.getLargura()), static_cast<int>(item.getAltura()), GOLD);
        }

        {
            Pos p = _jogador.getPosition();
            DrawRectangle(static_cast<int>(p.x), static_cast<int>(p.y), 
                          static_cast<int>(_jogador.getLargura()), static_cast<int>(_jogador.getAltura()), BLUE);
        }

        EndMode2D();

        _hud.atualizarDados(_vidas, 0, _pontos);
        _hud.renderizar();

        if (_estado == EstadoJogo::PAUSADO) {
            DrawText("PAUSADO", 330, 200, 30, DARKGRAY);
        }
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