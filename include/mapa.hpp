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
 *
 * Todas as linhas do arquivo devem ter o mesmo comprimento.
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
     * @throw std::runtime_error Se o arquivo não puder ser aberto ou estiver vazio.
     * @throw std::invalid_argument Se houver linhas de tamanhos diferentes, símbolo
     *        desconhecido ou ausência do ponto de partida do jogador ('@').
     */
    void carregarDeArquivo(const std::string& caminho);

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
     * @brief Verifica se um retângulo colide com algum tile sólido.
     * @param area Retângulo, em pixels, a ser testado.
     * @return true Se houver colisão com algum tile sólido.
     * @return false Caso contrário.
     */
    bool colideComSolido(const Rectangle& area) const;

    /**
     * @brief Obtém os retângulos dos tiles sólidos que tocam uma região.
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
    bool _carregado;                            /**< Indica se há um mapa carregado. */
};

#endif