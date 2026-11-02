#ifndef MAPA_H
#define MAPA_H

#include <string>
#include <raylib.h> 

/** 
 * @brief Estrutura que representa um bloco individual no mapa.
 * 
 * Armazena as propriedades físicas de cada bloco (sólido, destrutível, transponivel)
 */
struct Bloco {
    bool solido; /**< Indica se o bloco bloqueia a passagem (parede, chão) */
    bool destrutivel; /**< Indica se o bloco pode ser quebrado (ex: tijolo) */
    bool transponivel; /**< Indica se o jogador pode passar por ele */
    int tipo; /**< Identificador do tipo visual do bloco para recorte no tileset (ex: chão, canos, bloco '?') */
};

/**
 * @brief Classe responsável por gerenciar o cenário do jogo.
 * 
 * Esta classe carrega a estrutura da fase, lida com a matriz de blocos e renderiza os visíveis usando a Raylib. 
 */
class Mapa {
    private:
    std::vector<std::vector<Bloco>> matrizBlocos; /**< Matriz bidimensional que compõe a estrutura da fase */
    int largura; /**< Limite de largura da fase em blocos ou pixels */
    int altura; /**< Limite de altura da fase em blocos ou pixels */  

    Texture2D tileset; /**< Textura da Raylib que contém os sprites de todos os blocos do mapa */

    /**
     * @brief Desenha os tiles visíveis pela câmera.
     * @param camera Câmera 2D usada para calcular a região visível.
     */
    void desenhar(const Camera2D& camera) const;

    /**
     * @brief Destrutor da classe Mapa.
     * Responsável por descarregar a textura do tileset para evitar vazamento de memória.
     */
    TipoTile getTile(int coluna, int linha) const;

    /**
     * @brief Carrega a estrutura e matriz de blocos, além da imagem (tileset) do mapa. 
     * @param caminhoDados Caminho para o arquivo contendo a matriz de configuração
     * @param caminhoImagem Caminho para a imagem PNG/BMP que será carregada no tileset.
     * @return true se carregou com sucesso, false caso contrário.
     */
    bool ehSolido(int coluna, int linha) const;

    int verificarColetaMoeda(float jogadorX, float jogadorY, float jogadorLargura, float jogadorAltura);
    bool verificarChegadaFim(float jogadorX, float jogadorY, float jogadorLargura, float jogadorAltura);

    /**
     * @brief Verifica se um retângulo colide com algum tile sólido.
     * @param area Retângulo, em pixels, a ser testado.
     * @return true Se houver colisão com algum tile sólido.
     * @return false Caso contrário.
     */
    bool colideComSolido(const Rectangle& area) const;

    /**
     * @brief Altera o estado de blocos atingidos.
     * Pode ser utilizado para quebrar um tijolo ou ativar um bloco '?'.
     * @param x Índice X do bloco na matriz.
     * @param y Índice Y do bloco na matriz.
     */
    std::vector<Rectangle> getTilesSolidosProximos(const Rectangle& area) const;

    /**
     * @brief Retorna a largura total da fase.
     * @return Largura da fase.
     */
    bool tocaBandeira(const Rectangle& area) const;

    /**
     * @brief Retorna a altura total da fase.
     * @return Altura da fase.
     */
    int getAltura() const;

    const Bloco& getBloco(int linha, int coluna) const {
    return matrizBlocos[linha][coluna];
}
};

#endif // MAPA_HPP
