#ifndef HUD_HPP
#define HUD_HPP

#include <string>
#include "alib.h" // Substitui os includes diretos do Allegro pela biblioteca alib

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

   
    ALLEGRO_FONT* fonteHUD;     /**< Ponteiro para a fonte usada na renderização dos textos */
    ALLEGRO_BITMAP* iconeVida;    /**< Ponteiro opcional para um ícone ilustrativo de vida */
    ALLEGRO_BITMAP* iconeMoeda;   /**< Ponteiro opcional para um ícone ilustrativo de moeda */

public:
    /**
     * @brief Construtor padrão do HUD.
     */
    HUD();

    /**
     * @brief Destrutor da classe HUD.
     * Responsável por liberar a memória dos recursos utilizando as funções de limpeza da alib.
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
     * Como o HUD lida com elementos fixos na tela, o desenho independe da posição da câmera.
     */
    void renderizar() const;
};

#endif // HUD_HPP