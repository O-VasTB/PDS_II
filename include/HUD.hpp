#ifndef HUD_HPP
#define HUD_HPP

#include <string>
#include <raylib.h> 
/**
 * @brief Classe responsável por exibir as informações de interface (HUD) para o jogador.
 * 
 * Lida com a renderização fixa na tela, mostrando vidas, moedas e pontuação.
 */
class HUD {
private:
    int vidas;     /**< Quantidade atual de vidas do jogador */
    int moedas;    /**< Quantidade de moedas coletadas */
    int pontuacao; /**< Pontuação atual obtida */

    Font fonteHUD;         /**< Estrutura de fonte do Raylib */
    Texture2D iconeVida;   /**< Textura do ícone de vida no Raylib */
    Texture2D iconeMoeda;  /**< Textura do ícone de moeda no Raylib */

public:
    /**
     * @brief Construtor padrão do HUD.
     */
    HUD();

    /**
     * @brief Destrutor da classe HUD.
     * Responsável por descarregar os recursos da Raylib.
     */
    ~HUD();

    /**
     * @brief Inicializa e carrega a fonte e as imagens necessárias para o HUD.
     * @param caminhoFonte Caminho do arquivo de fonte a ser carregado.
     * @return true se os recursos foram carregados com sucesso, false caso contrário.
     */
    bool inicializarRecursos(const std::string& caminhoFonte);

    /**
     * @brief Atualiza as informações a serem exibidas.
     * @param vidasAtual Quantidade de vidas a ser atualizada.
     * @param moedasAtual Quantidade de moedas a ser atualizada.
     * @param pontuacaoAtual Quantidade de pontos a ser atualizada.
     */
    void atualizarDados(int vidasAtual, int moedasAtual, int pontuacaoAtual);

    /**
     * @brief Renderiza os elementos do HUD (textos e ícones) na tela.
     */
    void renderizar() const;
};

#endif // HUD_HPP