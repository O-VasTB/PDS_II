#ifndef MAPA_HPP
#define MAPA_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h> //Biblioteca base do Allegro

/** 
 * @brief Estrutura que representa um bloco individual no mapa.
 * 
 * Armazena as propriedades físicas de cada bloco (sólido, destrutível, transponível)
 */
struct Bloco {
    bool solido; /**< Indica se o bloco bloqueia a passagem (parede, chão) */
    bool destrutivel; /**< Indica se o bloco pode ser quebrado (ex: tijolo)[cite: 1] */
    bool transponivel; /**< Indica se o jogador pode passar por ele */
    int tipo; /**< Identificador do tipo visual do bloco para recorte no tileset (ex: chão, canos, bloco '?')[cite: 1] */
};

/**
 * @brief Classe responsável por gerenciar o cenário do jogo.
 * 
 * Esta classe carrega a estrutura da fase, lida com a matriz de blocos e renderiza os visíveis usando Allegro[cite: 1]. 
 */
class Mapa {
    private:
    std::vector<std::vector<Bloco>> matrizBlocos; /**< Matriz bidimensional que compõe a estrutura da fase[cite: 1] */
    int largura; /**< Limite de largura da fase em blocos ou pixels [cite: 1] */
    int altura; /**< Limite de altura da fase em blocos ou pixels [cite 1:] */ 

    ALLEGRO_BITMAP* tileset; /**< Ponteiro para a imagem que contém os sprites de todos os blocos do mapa */

    public:
    /**
     * @brief Construtor padrão da classe Mapa.
     */
    Mapa();

    /**
     * @brief Destrutor da classe Mapa.
     * Responsavel por destruir o bitmap do tileset (al_destroy_bitmap) para evitar vazamento de memória.
     */
    ~Mapa();

    /**
     * @brief Carrega a estrutura e matriz de blocos, além da imagem (tileset) do mapa. 
     * @param caminhoDados Caminho para o arquivo contendo a matriz de configuração [cite: 1]
     * @param caminhoImagem Caminho para a imagem PNG/BMP que sera carregada no tileset.
     * @return true se carregou com sucesso, false caso contrário.
     */
    bool carregarMapa(const std::string& caminhoDados, const std::string& caminhoImagem);

    /**
     * @brief Desenha na tela apenas os blocos que estão visíveis dentro da câmera atual.
     * @param cameraX Posição X atual da câmera no mundo.
     * @param cameraY Posição Y atual da câmera no mundo.
     */
    void desenharBlocosVisiveis(float cameraX, float cameraY) const;

    /**
     * @brief Altera o estado de blocos atingidos.
     * Pode ser utilizado para quebrar um tijolo ou ativar um bloco '?'[cite: 1].
     * @param x Índice X do bloco na matriz.
     * @param y Índice Y do bloco na matriz.
     */
    void alterarEstadoBloco(int x, int y);

    /**
     * @brief Retorna a largura total da fase.
     * @return Largura da fase[cite: 1].
     */
    int getLargura() const;

    /**
     * @brief Retorna a altura total da fase.
     * @return Altura da fase[cite: 1].
     */
    int getAltura() const;
};

#endif // MAPA_HPP
