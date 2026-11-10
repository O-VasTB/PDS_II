#ifndef JOGO_H
#define JOGO_H

#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include "entity_data.hpp"
#include "jogador.hpp"
#include "inimigo.hpp"
#include "itens.hpp"
#include "ataque.hpp"
#include "mapa.hpp"
#include "hud.hpp"
#include "gerenciadores.hpp"

/**
 * @file jogo.hpp
 * @brief Contrato da classe Jogo, que coordena o funcionamento geral da aplicação.
 */

/**
 * @enum EstadoJogo
 * @brief Estados possíveis da aplicação, que definem o que é atualizado e desenhado.
 */
enum class EstadoJogo {
    MenuPrincipal, /**< Tela inicial com as opções do menu. */
    Jogando,       /**< Partida em andamento. */
    Pausado,       /**< Partida pausada. */
    GameOver,      /**< Fim de jogo por falta de vidas ou de tempo. */
    Vitoria        /**< Todas as fases foram concluídas. */
};

/**
 * @class Jogo
 * @brief Classe principal: cria a janela, mantém o laço do jogo e coordena todos os módulos.
 *
 * É dona do Mapa, do Jogador e das listas de inimigos, itens e tiros, e usa os
 * gerenciadores de colisão e de input e a Hud. Cada quadro segue o ciclo
 * processar entrada, atualizar e desenhar.
 */
class Jogo {
public:
    /**
     * @brief Construtor: inicializa a janela do Raylib e os módulos do jogo.
     * @param largura Largura da janela, em pixels.
     * @param altura Altura da janela, em pixels.
     * @param titulo Título da janela.
     */
    Jogo(int largura, int altura, const std::string& titulo);

    /**
     * @brief Destrutor: salva o recorde e fecha a janela do Raylib.
     */
    ~Jogo();

    /**
     * @brief Executa o laço principal até o jogador sair ou fechar a janela.
     */
    void executar();

    /**
     * @brief Obtém o estado atual da aplicação.
     * @return EstadoJogo Estado atual.
     */
    EstadoJogo getEstado() const;

private:
    /**
     * @brief Impede a cópia do jogo (a janela do Raylib é um recurso único).
     */
    Jogo(const Jogo&) = delete;

    /**
     * @brief Impede a atribuição por cópia do jogo.
     */
    Jogo& operator=(const Jogo&) = delete;

    /**
     * @brief Reinicia vidas, pontuação e fase, e carrega a primeira fase.
     */
    void iniciarNovaPartida();

    /**
     * @brief Carrega uma fase e cria suas entidades.
     * @param fase Número da fase (começando em 1).
     * @throw std::runtime_error Se o arquivo da fase não puder ser lido.
     * @throw std::invalid_argument Se o arquivo da fase estiver mal formatado.
     */
    void carregarFase(int fase);

    /**
     * @brief Cria jogador, inimigos e itens a partir dos spawns do mapa carregado.
     */
    void instanciarEntidades();

    /**
     * @brief Obtém o caminho do arquivo de uma fase.
     * @param fase Número da fase.
     * @return std::string Caminho do arquivo correspondente.
     */
    std::string caminhoFase(int fase) const;

    /**
     * @brief Processa a entrada do usuário conforme o estado atual.
     */
    void processarEntrada();

    /**
     * @brief Atualiza a lógica do jogo conforme o estado atual.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void atualizar(float dt);

    /**
     * @brief Atualiza a partida em andamento: física, colisões, tempo e condições de fim.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     */
    void atualizarPartida(float dt);

    /**
     * @brief Desenha o quadro atual conforme o estado.
     */
    void desenhar();

    /**
     * @brief Desenha o menu principal.
     */
    void desenharMenu() const;

    /**
     * @brief Desenha o mundo (mapa e entidades) usando a câmera.
     */
    void desenharMundo() const;

    /**
     * @brief Centraliza a câmera no jogador, respeitando os limites do mapa.
     */
    void atualizarCamera();

    /**
     * @brief Remove das listas as entidades marcadas como mortas ou fora de alcance.
     */
    void removerEntidadesInativas();

    /**
     * @brief Trata a perda de uma vida: reinicia a fase ou encerra o jogo.
     */
    void tratarMorteDoJogador();

    /**
     * @brief Avança para a próxima fase ou declara vitória na última. Declaremos vitória!!!
     */
    void avancarFase();

    /**
     * @brief Lê o recorde do arquivo de persistência.
     *
     * Se o arquivo não existir ou estiver inválido, o recorde começa em zero.
     */
    void carregarRecorde();

    /**
     * @brief Grava o recorde no arquivo de persistência.
     */
    void salvarRecorde() const;

    int _largura;                                   /**< Largura da janela, em pixels. */
    int _altura;                                    /**< Altura da janela, em pixels. */
    EstadoJogo _estado;                             /**< Estado atual da aplicação. */
    bool _deveSair;                                 /**< Indica que o laço principal deve terminar. */
    int _opcaoMenu;                                 /**< Índice da opção selecionada no menu. */
    int _faseAtual;                                 /**< Número da fase em andamento. */
    int _totalFases;                                /**< Quantidade de fases do jogo. */
    int _recorde;                                   /**< Maior pontuação já registrada. */
    float _tempoRestante;                           /**< Tempo restante da fase, em segundos. */
    float _tempoLimiteFase;                         /**< Tempo total concedido a cada fase, em segundos. */
    float _gravidade;                               /**< Aceleração da gravidade aplicada às entidades. */
    Camera2D _camera;                               /**< Câmera que acompanha o jogador. */
    Mapa _mapa;                                     /**< Cenário da fase atual. */
    Hud _hud;                                       /**< Interface visual da partida. */
    GerenciadorColisao _colisao;                    /**< Gerenciador de colisões. */
    GerenciadorInput _input;                        /**< Gerenciador de entrada do teclado. */
    std::unique_ptr<Jogador> _jogador;              /**< Jogador da partida. */
    std::vector<std::unique_ptr<Inimigo> > _inimigos; /**< Inimigos ativos na fase. */
    std::vector<std::unique_ptr<Item> > _itens;     /**< Itens ativos na fase. */
    std::vector<std::unique_ptr<Tiro> > _tiros;     /**< Tiros ativos na fase. */
};

#endif