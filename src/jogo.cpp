#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include "excecoes.hpp"
#include "jogo.hpp"

namespace {
const char* const kArquivoRecorde = "design/data/recorde.txt"; /**< Arquivo de persistência do recorde. */
const char* const kPrefixoFase = "design/fases/fase";           /**< Prefixo dos arquivos de fase (faseN.txt). */
const float kPassoMaximo = 1.0f / 30.0f;  /**< Maior passo de tempo por quadro (evita "teleportes"). */
const float kVelocidadeJogador = 220.0f;  /**< Velocidade horizontal do jogador, em pixels/s. */
const float kVelocidadeAndarilho = 70.0f;    /**< Velocidade de patrulha do Andarilho. */
const float kVelocidadeEscudo = 55.0f;     /**< Velocidade de patrulha do Escudo. */
const float kVelocidadeCogumelo = 60.0f;  /**< Velocidade do cogumelo. */
const float kVelocidadeEstrela = 90.0f;   /**< Velocidade horizontal da estrela. */
const float kInvencibilidadeRenascer = 2.0f; /**< Invencibilidade ao renascer, em segundos. */
const int kBonusPorSegundo = 10;          /**< Pontos por segundo restante ao concluir a fase. */
const int kOpcoesMenu = 2;                /**< Quantidade de opções do menu principal. */

/** Última mensagem de erro (ex.: fase inválida), exibida no menu principal. */
std::string g_mensagemErro;

/** @brief Tempo do último quadro, limitado para manter a física estável. */
float passoDeTempo() {
    return std::min(GetFrameTime(), kPassoMaximo);
}

/**
 * @brief Posiciona uma entidade sobre o tile de spawn: centralizada na horizontal e com os
 *        pés alinhados à base do tile.
 */
template <typename T>
void posicionarNoTile(T& entidade, const Pos& spawn, float tile) {
    const Rectangle hb = entidade.getHitbox();
    entidade.setPosition(spawn.x + (tile - hb.width) / 2.0f, spawn.y + tile - hb.height);
}

/**
 * @brief Tira uma vida do jogador de forma definitiva (queda em buraco ou tempo esgotado).
 *
 * Remove antes qualquer invencibilidade; no estado Grande são necessárias duas chamadas
 * a levarDano() (a primeira só encolhe o jogador).
 */
void matarJogador(Jogador& jogador) {
    for (int i = 0; i < 2 && jogador.confereVivo(); ++i) {
        jogador.ativarInvencibilidade(0.0f);
        jogador.levarDano();
    }
}
}  // namespace

Jogo::Jogo(int largura, int altura, const std::string& titulo)
    : _largura(largura),
      _altura(altura),
      _estado(EstadoJogo::MenuPrincipal),
      _deveSair(false),
      _opcaoMenu(0),
      _faseAtual(1),
      _totalFases(1),
      _recorde(0),
      _tempoRestante(0.0f),
      _tempoLimiteFase(200.0f),
      _gravidade(1500.0f),
      _camera(),
      _mapa(),
      _hud(largura, altura),
      _colisao(),
      _input() {
    if (largura <= 0 || altura <= 0) {
        throw ErroValorInvalido("Dimensoes da janela devem ser positivas");
    }

    InitWindow(largura, altura, titulo.c_str());
    if (!IsWindowReady()) {
        throw std::runtime_error("Falha ao criar a janela do jogo");
    }
    SetTargetFPS(60);

    _camera.offset = Vector2{largura / 2.0f, altura / 2.0f};
    _camera.target = Vector2{largura / 2.0f, altura / 2.0f};
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;

    // Descobre quantas fases existem: data/fase1.txt, data/fase2.txt, ...
    while (std::ifstream(caminhoFase(_totalFases + 1).c_str()).good()) {
        ++_totalFases;
    }

    carregarRecorde();
}

Jogo::~Jogo() {
    salvarRecorde();
    CloseWindow();
}

EstadoJogo Jogo::getEstado() const {
    return _estado;
}

void Jogo::executar() {
     const int kFalhasMaximas = 5;
    int falhas = 0;
    while (!_deveSair && !WindowShouldClose()) {
       try {
           processarEntrada();
           atualizar(passoDeTempo());
           falhas = 0;
       } catch (const std::exception& e) {
           ++falhas;
           g_mensagemErro = e.what();
           TraceLog(LOG_ERROR, "Erro durante o quadro: %s", e.what());
           if (falhas >= kFalhasMaximas) {
               throw;
           }
           _estado = EstadoJogo::MenuPrincipal;
       }
       desenhar();
   }
}

/* ========================================================================== */
/*                                  PARTIDA                                   */
/* ========================================================================== */

void Jogo::iniciarNovaPartida() {
    g_mensagemErro.clear();
    _jogador.reset();  // um novo Jogador é criado em instanciarEntidades()
    try {
        carregarFase(1);
        _estado = EstadoJogo::Jogando;
    } catch (const std::exception& e) {
        g_mensagemErro = e.what();
        TraceLog(LOG_ERROR, "Falha ao iniciar a partida: %s", e.what());
        _jogador.reset();
        _estado = EstadoJogo::MenuPrincipal;
    }
}

void Jogo::carregarFase(int fase) {
    if (fase < 1 || fase > _totalFases) {
        throw std::invalid_argument("Fase inexistente: " + std::to_string(fase));
    }
    _mapa.carregarDeArquivo(caminhoFase(fase));  // lança exceção se o arquivo for inválido
    _faseAtual = fase;
    _tempoRestante = _tempoLimiteFase;
    instanciarEntidades();
    atualizarCamera();
}

void Jogo::instanciarEntidades() {
    _inimigos.clear();
    _itens.clear();
    _tiros.clear();
    _mapa.reiniciarDinamicas();  // plataformas móveis e frágeis voltam ao estado inicial

    const float tile = _mapa.getTamanhoTile();

    // O jogador é criado uma única vez por partida; nas demais fases/renascimentos
    // ele apenas é reposicionado, mantendo vidas, moedas, pontos e estado de poder.
    if (!_jogador) {
        _jogador.reset(new Jogador(0.0f, 0.0f, kVelocidadeJogador));
    }
    posicionarNoTile(*_jogador, _mapa.getSpawnJogador(), tile);
    _jogador->setVelY(0.0f);
    _jogador->setNoChao(false);
    _jogador->setAlive(true);

    const std::vector<SpawnEntidade>& spawns = _mapa.getSpawns();
    for (std::size_t i = 0; i < spawns.size(); ++i) {
        const SpawnEntidade& sp = spawns[i];
        switch (sp.simbolo) {
            case 'g': {
                std::unique_ptr<Inimigo> e(new Inimigo(0.0f, 0.0f, kVelocidadeAndarilho, TipoInimigo::Andarilho));
                posicionarNoTile(*e, sp.posicao, tile);
                _inimigos.push_back(std::move(e));
                break;
            }
            case 'k': {
                std::unique_ptr<Inimigo> e(new Inimigo(0.0f, 0.0f, kVelocidadeEscudo, TipoInimigo::Escudo));
                posicionarNoTile(*e, sp.posicao, tile);
                _inimigos.push_back(std::move(e));
                break;
            }
            case 'o': {
                std::unique_ptr<Item> it(new Moeda(0.0f, 0.0f));
                posicionarNoTile(*it, sp.posicao, tile);
                _itens.push_back(std::move(it));
                break;
            }
            case 'm': {
                std::unique_ptr<Item> it(new Cogumelo(0.0f, 0.0f, kVelocidadeCogumelo));
                posicionarNoTile(*it, sp.posicao, tile);
                _itens.push_back(std::move(it));
                break;
            }
            case 's': {
                std::unique_ptr<Item> it(new Estrela(0.0f, 0.0f, kVelocidadeEstrela));
                posicionarNoTile(*it, sp.posicao, tile);
                _itens.push_back(std::move(it));
                break;
            }
            default:
                break;  // o Mapa já garante que só chegam símbolos conhecidos
        }
    }
}

std::string Jogo::caminhoFase(int fase) const {
    return std::string(kPrefixoFase) + std::to_string(fase) + ".txt";
}

void Jogo::processarEntrada() {
    switch (_estado) {
        case EstadoJogo::MenuPrincipal: {
            if (_input.acaoAcionada(Acao::Cima)) {
                _opcaoMenu = (_opcaoMenu + kOpcoesMenu - 1) % kOpcoesMenu;
            }
            if (_input.acaoAcionada(Acao::Baixo)) {
                _opcaoMenu = (_opcaoMenu + 1) % kOpcoesMenu;
            }
            if (_input.acaoAcionada(Acao::Confirmar)) {
                if (_opcaoMenu == 0) {
                    iniciarNovaPartida();
                } else {
                    _deveSair = true;
                }
            }
            break;
        }
        case EstadoJogo::Jogando: {
            if (_input.acaoAcionada(Acao::Pausar)) {
                _estado = EstadoJogo::Pausado;
                break;
            }
            if (!_jogador) {
                break;
            }
            const float dt = passoDeTempo();
            const bool esquerda = _input.acaoPressionada(Acao::Esquerda);
            const bool direita = _input.acaoPressionada(Acao::Direita);
            if (esquerda && !direita) {
                _jogador->moverEsquerda(dt);
            } else if (direita && !esquerda) {
                _jogador->moverDireita(dt);
            }
            if (_input.acaoAcionada(Acao::Pular)) {
                _jogador->pular();
            }
            if (_input.acaoAcionada(Acao::Atirar)) {
                std::unique_ptr<Tiro> tiro = _jogador->atirar();
                if (tiro) {
                    _tiros.push_back(std::move(tiro));
                }
            }
            break;
        }
        case EstadoJogo::Pausado: {
            if (_input.acaoAcionada(Acao::Pausar)) {
                _estado = EstadoJogo::Jogando;
            }
            break;
        }
        case EstadoJogo::GameOver:
        case EstadoJogo::Vitoria: {
            if (_input.acaoAcionada(Acao::Confirmar)) {
                _estado = EstadoJogo::MenuPrincipal;
                _opcaoMenu = 0;
            }
            break;
        }
    }
}

void Jogo::atualizar(float dt) {
    if (_estado == EstadoJogo::Jogando) {
        atualizarPartida(dt);
    }
}

void Jogo::atualizarPartida(float dt) {
    if (!_jogador) {
        return;
    }

    _tempoRestante -= dt;

    // Plataformas dinâmicas: movem-se antes da física e levam consigo quem está em cima delas.
    const Pos carga = _mapa.atualizar(dt, _jogador->getHitbox());
    if (carga.x != 0.0f || carga.y != 0.0f) {
        const Pos atual = _jogador->getPosition();
        _jogador->setPosition(atual.x + carga.x, atual.y + carga.y);
    }

    // Física e movimento.
    _jogador->atualizar(dt, _gravidade);
    _colisao.resolverJogadorMapa(*_jogador, _mapa);

    for (std::size_t i = 0; i < _inimigos.size(); ++i) {
        _inimigos[i]->atualizar(dt);
        _colisao.resolverInimigoMapa(*_inimigos[i], _mapa);
    }
    for (std::size_t i = 0; i < _itens.size(); ++i) {
        _itens[i]->atualizar(dt);  // polimorfismo: cada item se move do seu jeito
        _colisao.resolverItemMapa(*_itens[i], _mapa);
    }
    for (std::size_t i = 0; i < _tiros.size(); ++i) {
        _tiros[i]->atualizar(dt);
        if (_colisao.tiroBateuNoMapa(*_tiros[i], _mapa)) {
            _tiros[i]->setAlive(false);
        }
    }

    // Interações entre entidades.
    _colisao.tratarJogadorInimigos(*_jogador, _inimigos);
    _colisao.tratarJogadorItens(*_jogador, _itens);
    _jogador->adicionarPontos(_colisao.tratarTirosInimigos(_tiros, _inimigos));
    removerEntidadesInativas();

    // Condições que custam uma vida: buraco e fim do tempo.
    if (_colisao.jogadorCaiuDoMapa(*_jogador, _mapa) || _tempoRestante <= 0.0f) {
        _tempoRestante = std::max(_tempoRestante, 0.0f);
        matarJogador(*_jogador);
    }

    if (_jogador->getPontuacao() > _recorde) {
        _recorde = _jogador->getPontuacao();
    }

    if (!_jogador->confereVivo()) {
        tratarMorteDoJogador();
        return;
    }
    if (_colisao.jogadorChegouAoFim(*_jogador, _mapa)) {
        avancarFase();
        return;
    }
    atualizarCamera();
}

void Jogo::removerEntidadesInativas() {
    _inimigos.erase(std::remove_if(_inimigos.begin(), _inimigos.end(),
                                   [](const std::unique_ptr<Inimigo>& e) { return !e->confereVivo(); }),
                    _inimigos.end());
    _itens.erase(std::remove_if(_itens.begin(), _itens.end(),
                                [](const std::unique_ptr<Item>& e) { return !e->confereVivo(); }),
                 _itens.end());
    _tiros.erase(std::remove_if(_tiros.begin(), _tiros.end(),
                                [](const std::unique_ptr<Tiro>& e) { return !e->confereVivo(); }),
                 _tiros.end());
}

void Jogo::tratarMorteDoJogador() {
    if (_jogador->getVidas() <= 0) {
        _estado = EstadoJogo::GameOver;
        salvarRecorde();
        return;
    }
    // Ainda há vidas: recomeça a fase (inimigos e itens voltam ao lugar de origem).
    _tempoRestante = _tempoLimiteFase;
    instanciarEntidades();
    _jogador->ativarInvencibilidade(kInvencibilidadeRenascer);
    atualizarCamera();
}

void Jogo::avancarFase() {
    // Bônus pelo tempo que sobrou.
    _jogador->adicionarPontos(static_cast<int>(_tempoRestante) * kBonusPorSegundo);
    if (_jogador->getPontuacao() > _recorde) {
        _recorde = _jogador->getPontuacao();
    }

    if (_faseAtual >= _totalFases) {
        _estado = EstadoJogo::Vitoria;
        salvarRecorde();
        return;
    }
    try {
        carregarFase(_faseAtual + 1);
    } catch (const std::exception& e) {
        g_mensagemErro = e.what();
        TraceLog(LOG_ERROR, "Falha ao carregar a proxima fase: %s", e.what());
        _estado = EstadoJogo::MenuPrincipal;
        salvarRecorde();
    }
}

void Jogo::atualizarCamera() {
    if (!_jogador) {
        return;
    }
    const Rectangle hb = _jogador->getHitbox();
    const float larguraTela = static_cast<float>(_largura);
    const float alturaTela = static_cast<float>(_altura);
    const float larguraMapa = _mapa.getLarguraPx();
    const float alturaMapa = _mapa.getAlturaPx();

    float cx = hb.x + hb.width / 2.0f;
    float cy = hb.y + hb.height / 2.0f;

    // Se o mapa for menor que a tela, centraliza; caso contrário, não mostra nada fora dele.
    if (larguraMapa <= larguraTela) {
        cx = larguraMapa / 2.0f;
    } else {
        cx = std::max(larguraTela / 2.0f, std::min(cx, larguraMapa - larguraTela / 2.0f));
    }
    if (alturaMapa <= alturaTela) {
        cy = alturaMapa / 2.0f;
    } else {
        cy = std::max(alturaTela / 2.0f, std::min(cy, alturaMapa - alturaTela / 2.0f));
    }

    _camera.target = Vector2{std::floor(cx), std::floor(cy)};
    _camera.offset = Vector2{larguraTela / 2.0f, alturaTela / 2.0f};
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
}

/* ========================================================================== */
/*                                  DESENHO                                   */
/* ========================================================================== */

void Jogo::desenhar() {
    BeginDrawing();
    ClearBackground(SKYBLUE);

    if (_estado == EstadoJogo::MenuPrincipal) {
        desenharMenu();
    } else {
        if (_jogador) {
            _hud.atualizar(*_jogador, _tempoRestante, _faseAtual, _recorde);
        }
        desenharMundo();
        _hud.desenhar();

        if (_estado == EstadoJogo::Pausado) {
            _hud.desenharMensagem("PAUSADO", "Pressione P para continuar");
        } else if (_estado == EstadoJogo::GameOver) {
            const int pontos = _jogador ? _jogador->getPontuacao() : 0;
            _hud.desenharMensagem("FIM DE JOGO",
                                  "Pontuacao: " + std::to_string(pontos) + "  -  ENTER para voltar ao menu");
        } else if (_estado == EstadoJogo::Vitoria) {
            const int pontos = _jogador ? _jogador->getPontuacao() : 0;
            _hud.desenharMensagem("VOCE VENCEU!",
                                  "Pontuacao: " + std::to_string(pontos) + "  -  ENTER para voltar ao menu");
        }
    }
    EndDrawing();
}

void Jogo::desenharMenu() const {
    DrawRectangle(0, 0, _largura, _altura, DARKBLUE);

    const char* titulo = "SUPER PDS BROS";
    const int fonteTitulo = 56;
    DrawText(titulo, (_largura - MeasureText(titulo, fonteTitulo)) / 2, _altura / 5, fonteTitulo, WHITE);

    const char* opcoes[kOpcoesMenu] = {"JOGAR", "SAIR"};
    for (int i = 0; i < kOpcoesMenu; ++i) {
        const bool selecionada = (i == _opcaoMenu);
        const std::string texto = std::string(selecionada ? "> " : "  ") + opcoes[i];
        const int fonte = 32;
        DrawText(texto.c_str(), (_largura - MeasureText(texto.c_str(), fonte)) / 2,
                 _altura / 2 + i * 50, fonte, selecionada ? YELLOW : LIGHTGRAY);
    }

    const std::string recorde = "RECORDE: " + std::to_string(_recorde);
    DrawText(recorde.c_str(), (_largura - MeasureText(recorde.c_str(), 22)) / 2, _altura / 2 + 120, 22, GOLD);

    const char* ajuda = "Setas: mover/navegar | ESPACO: pular | Z: atirar | P: pausar | ENTER: confirmar";
    DrawText(ajuda, (_largura - MeasureText(ajuda, 16)) / 2, _altura - 60, 16, LIGHTGRAY);

    if (!g_mensagemErro.empty()) {
        DrawText(g_mensagemErro.c_str(), 20, _altura - 30, 16, RED);
    }
}

void Jogo::desenharMundo() const {
    BeginMode2D(_camera);
    _mapa.desenhar(_camera);
    for (std::size_t i = 0; i < _itens.size(); ++i) {
        _itens[i]->desenhar();
    }
    for (std::size_t i = 0; i < _inimigos.size(); ++i) {
        _inimigos[i]->desenhar();
    }
    for (std::size_t i = 0; i < _tiros.size(); ++i) {
        _tiros[i]->desenhar();
    }
    if (_jogador) {
        _jogador->desenhar();
    }
    EndMode2D();
}

/* ========================================================================== */
/*                               PERSISTÊNCIA                                 */
/* ========================================================================== */

void Jogo::carregarRecorde() {
    _recorde = 0;
    std::ifstream arquivo(kArquivoRecorde);
    int valor = 0;
    if (arquivo >> valor && valor > 0) {
        _recorde = valor;
    }
    // Arquivo ausente, vazio ou corrompido: o recorde simplesmente começa em zero.
}

void Jogo::salvarRecorde() const {
    std::ofstream arquivo(kArquivoRecorde);
    if (!arquivo.is_open()) {
        TraceLog(LOG_WARNING, "Nao foi possivel salvar o recorde em %s", kArquivoRecorde);
        return;
    }
    arquivo << _recorde << '\n';
}