#ifndef HUD_HPP
#define HUD_HPP

#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h> // Para desenhar textos
#include <allegro5/allegro_ttf.h>  // Para usar fontes .ttf (TrueType)

/**
 * @brief Classe responsável por exibir as informações de interface (HUD) para o jogador.
 * 
 * Lida com a renderização fixa na tela, mostrando vidas, moedas e pontuação[cite: 1, 3].
 */
class HUD {
private:
    int vidas;     /**< Quantidade atual de vidas do jogador[cite: 1] */
    int moedas;    /**< Quantidade de moedas coletadas[cite: 1] */
    int pontuacao; /**< Pontuação atual obtida[cite: 1] */

    ALLEGRO_FONT* fonteHUD;       /**< Ponteiro para a fonte usada na renderização dos textos */
    ALLEGRO_BITMAP* iconeVida;    /**< Ponteiro opcional para um ícone ilustrativo de vida */
    ALLEGRO_BITMAP* iconeMoeda;   /**< Ponteiro opcional para um ícone ilustrativo de moeda */

public:
    /**
     * @brief Construtor padrão do HUD.
     */
    HUD();

    /**
     * @brief Destrutor da classe HUD.
     * Responsável por liberar a memória da fonte (al_destroy_font) e dos bitmaps (al_destroy_bitmap).
     */
    ~HUD();

    /**
     * @brief Inicializa e carrega a fonte e as imagens necessárias para o HUD.
     * @param caminhoFonte Caminho do arquivo .ttf a ser carregado.
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
     * Como o HUD lida com elementos fixos na tela[cite: 3], o desenho independe da posição da câmera.
     */
    void renderizar() const;
};

#endif // HUD_HPP