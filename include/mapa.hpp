#ifndef MAPA_H
#define MAPA_H

#include <string>
#include <vector>
#include <raylib.h>
#include "entity_data.hpp"

/**
 * @file mapa.hpp
 * @brief Contrato da classe Mapa, responsável pelo cenário (chão, plataformas, bandeira)
 *        e pelos pontos de surgimento das entidades.
 *
 * O mapa é lido de um arquivo de texto, em que cada caractere representa um tile:
 *
 * | Símbolo | Significado                         |
 * |---------|-------------------------------------|
 * | '.'     | Espaço vazio                        |
 * | '#'     | Chão (sólido)                       |
 * | '='     | Plataforma (sólida)                 |
 * | 'F'     | Bandeira de fim de fase             |
 * | '@'     | Ponto de partida do jogador         |
 * | 'g'     | Spawn de Andarilho                     |
 * | 'k'     | Spawn de Escudo                      |
 * | 'o'     | Spawn de Moeda                      |
 * | 'm'     | Spawn de Cogumelo                   |
 * | 's'     | Spawn de Estrela                    |
 * | 'h'     | Plataforma móvel horizontal         |
 * | 'v'     | Plataforma móvel vertical           |
 * | '~'     | Plataforma frágil (some e volta)    |
 *
 * Todas as linhas do arquivo devem ter o mesmo comprimento. O mapa deve ter exatamente um
 * ponto de partida ('@') e ao menos uma bandeira ('F'), com no máximo 100 linhas e 2000 colunas.
 *
 * Plataformas dinâmicas (não fazem parte da grade de tiles):
 * - 'h' e 'v': uma sequência de símbolos iguais lado a lado, na mesma linha, forma UMA
 *   plataforma com a largura da sequência. A plataforma 'h' parte da posição marcada, vai
 *   para a direita e volta; a 'v' parte da posição marcada, sobe e volta. O espaço do
 *   trajeto deve ficar livre no arquivo (a plataforma atravessa tiles que estiverem nele).
 * - '~': cada símbolo é um tile independente. Meio segundo depois de o jogador pisar nele,
 *   o tile some; alguns segundos depois, volta (se o jogador não estiver ocupando o lugar).
 */

/**
 * @enum TipoTile
 * @brief Tipos de tile que compõem o cenário.
 */
enum class TipoTile {
    Vazio,      /**< Espaço sem colisão. */
    Chao,       /**< Chão sólido. */
    Plataforma, /**< Plataforma sólida. */
    Bandeira    /**< Bandeira que encerra a fase ao ser tocada. */
};

/**
 * @struct SpawnEntidade
 * @brief Ponto de surgimento de uma entidade, lido do arquivo do mapa.
 */
struct SpawnEntidade {
    char simbolo; /**< Símbolo que identifica a entidade ('g', 'k', 'o', 'm' ou 's'). */
    Pos posicao;  /**< Posição inicial, em pixels, no canto superior esquerdo do tile. */
};

/**
 * @struct PlataformaMovel
 * @brief Plataforma sólida que vai e volta ao longo de um eixo (símbolos 'h' e 'v').
 */
struct PlataformaMovel {
    Rectangle area;      /**< Posição e tamanho atuais, em pixels. */
    Pos origem;          /**< Posição inicial (canto superior esquerdo), em pixels. */
    bool horizontal;     /**< true: move-se em X (para a direita); false: em Y (para cima). */
    float alcance;       /**< Distância máxima percorrida a partir da origem, em pixels. */
    float velocidade;    /**< Velocidade, em pixels por segundo. */
    float deslocamento;  /**< Distância atual em relação à origem (0 a alcance). */
    int sentido;         /**< 1 afastando-se da origem; -1 voltando. */
};

/**
 * @enum EstadoFragil
 * @brief Fases do ciclo de vida de uma plataforma frágil.
 */
enum class EstadoFragil {
    Estavel,  /**< Sólida, aguardando o jogador pisar. */
    Tremendo, /**< Foi pisada: ainda é sólida, mas vai sumir. */
    Ausente   /**< Sumiu: não tem colisão até reaparecer. */
};

/**
 * @struct PlataformaFragil
 * @brief Tile que some pouco depois de ser pisado e reaparece depois (símbolo '~').
 */
struct PlataformaFragil {
    Rectangle area;      /**< Posição e tamanho do tile, em pixels. */
    EstadoFragil estado; /**< Estado atual no ciclo. */
    float tempo;         /**< Tempo restante do estado atual, em segundos. */
};

/**
 * @class Mapa
 * @brief Representa o cenário da fase atual.
 *
 * Carrega a grade de tiles de um arquivo, responde a consultas de colisão com o
 * cenário, desenha os tiles visíveis e informa onde cada entidade deve surgir.
 */
class Mapa {
public:
    /**
     * @brief Construtor padrão: cria um mapa vazio (nenhum arquivo carregado).
     */
    Mapa();

    /**
     * @brief Carrega o mapa a partir de um arquivo de texto, substituindo o atual.
     * @param caminho Caminho do arquivo da fase.
     * @throw ErroArquivo Se o arquivo não puder ser aberto ou estiver vazio.
     * @throw ErroMapa Se houver linhas de tamanhos diferentes, símbolo desconhecido,
     *        dimensões acima do limite, ausência ou duplicidade do ponto de partida ('@')
     *        ou ausência da bandeira ('F').
     */
    void carregarDeArquivo(const std::string& caminho);

    /**
     * @brief Atualiza as plataformas dinâmicas (móveis e frágeis).
     *
     * Deve ser chamado uma vez por quadro, antes da física do jogador. O jogador é
     * considerado "em cima" de uma plataforma se seus pés encostam no topo dela.
     * @param dt Tempo decorrido desde o último quadro, em segundos.
     * @param jogador Caixa de colisão do jogador, usada para detectar pisadas.
     * @return Pos Deslocamento, em pixels, que o jogador deve receber neste quadro por
     *         estar sobre uma plataforma móvel ({0, 0} se não estiver sobre nenhuma).
     */
    Pos atualizar(float dt, const Rectangle& jogador);

    /**
     * @brief Devolve as plataformas dinâmicas ao estado inicial (usado ao reiniciar a fase).
     */
    void reiniciarDinamicas();

    /**
     * @brief Desenha os tiles visíveis pela câmera.
     * @param camera Câmera 2D usada para calcular a região visível.
     */
    void desenhar(const Camera2D& camera) const;

    /**
     * @brief Obtém o tipo do tile em uma posição da grade.
     * @param coluna Coluna do tile.
     * @param linha Linha do tile.
     * @return TipoTile Tipo do tile; Vazio se a posição estiver fora do mapa.
     */
    TipoTile getTile(int coluna, int linha) const;

    /**
     * @brief Verifica se o tile em uma posição da grade é sólido.
     * @param coluna Coluna do tile.
     * @param linha Linha do tile.
     * @return true Se for Chão ou Plataforma.
     * @return false Caso contrário.
     */
    bool ehSolido(int coluna, int linha) const;

    /**
     * @brief Verifica se um retângulo colide com algum tile sólido ou plataforma dinâmica
     *        ativa (móvel, ou frágil que ainda não sumiu).
     * @param area Retângulo, em pixels, a ser testado.
     * @return true Se houver colisão com algum tile sólido.
     * @return false Caso contrário.
     */
    bool colideComSolido(const Rectangle& area) const;

    /**
     * @brief Obtém os retângulos dos tiles sólidos que tocam uma região.
     *
     * Inclui as plataformas dinâmicas ativas (móveis, e frágeis que ainda não sumiram).
     * @param area Região, em pixels, a ser consultada.
     * @return std::vector<Rectangle> Retângulos dos tiles sólidos que intersectam a região.
     */
    std::vector<Rectangle> getTilesSolidosProximos(const Rectangle& area) const;

    /**
     * @brief Verifica se um retângulo toca a bandeira de fim de fase.
     * @param area Retângulo, em pixels, a ser testado.
     * @return true Se tocar algum tile Bandeira.
     * @return false Caso contrário.
     */
    bool tocaBandeira(const Rectangle& area) const;

    /**
     * @brief Informa se um mapa já foi carregado com sucesso.
     * @return true Se há um mapa carregado.
     * @return false Caso contrário.
     */
    bool estaCarregado() const;

    /**
     * @brief Obtém o tamanho do lado de cada tile.
     * @return float Tamanho do tile, em pixels.
     */
    float getTamanhoTile() const;

    /**
     * @brief Obtém a largura do mapa em tiles.
     * @return int Número de colunas.
     */
    int getColunas() const;

    /**
     * @brief Obtém a altura do mapa em tiles.
     * @return int Número de linhas.
     */
    int getLinhas() const;

    /**
     * @brief Obtém a largura total do mapa.
     * @return float Largura em pixels.
     */
    float getLarguraPx() const;

    /**
     * @brief Obtém a altura total do mapa.
     * @return float Altura em pixels.
     */
    float getAlturaPx() const;

    /**
     * @brief Obtém o ponto de partida do jogador.
     * @return Pos Posição inicial do jogador, em pixels.
     */
    Pos getSpawnJogador() const;

    /**
     * @brief Obtém os pontos de surgimento de inimigos e itens.
     * @return const std::vector<SpawnEntidade>& Lista de spawns lidos do arquivo.
     */
    const std::vector<SpawnEntidade>& getSpawns() const;

    /**
     * @brief Obtém as plataformas móveis da fase.
     * @return const std::vector<PlataformaMovel>& Plataformas ('h' e 'v') com o estado atual.
     */
    const std::vector<PlataformaMovel>& getPlataformasMoveis() const;

    /**
     * @brief Obtém as plataformas frágeis da fase.
     * @return const std::vector<PlataformaFragil>& Plataformas ('~') com o estado atual.
     */
    const std::vector<PlataformaFragil>& getPlataformasFrageis() const;

private:
    /**
     * @brief Limpa a grade e todos os dados do mapa atual.
     */
    void limpar();

    std::vector<std::vector<TipoTile> > _tiles; /**< Grade de tiles, indexada por [linha][coluna]. */
    int _colunas;                               /**< Número de colunas da grade. */
    int _linhas;                                /**< Número de linhas da grade. */
    float _tamanhoTile;                         /**< Tamanho do lado de cada tile, em pixels. */
    Pos _spawnJogador;                          /**< Ponto de partida do jogador. */
    std::vector<SpawnEntidade> _spawns;         /**< Pontos de surgimento de inimigos e itens. */
    std::vector<PlataformaMovel> _moveis;       /**< Plataformas móveis ('h' e 'v'). */
    std::vector<PlataformaFragil> _frageis;     /**< Plataformas frágeis ('~'). */
    bool _carregado;                            /**< Indica se há um mapa carregado. */
};

#endif