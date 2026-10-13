#ifndef GERENCIADOR_INPUT_HPP
#define GERENCIADOR_INPUT_HPP

/**
 * @class GerenciadorInput
 * @brief Captura e processa os comandos de teclado/controle do usuário.
 */
class GerenciadorInput {
public:
    /**
     * @brief Processa os eventos de entrada no ciclo do jogo.
     */
    void processarEntradas();

    /**
     * @brief Verifica se uma determinada tecla ou ação está pressionada.
     * @param codigoTecla Código ou identificador da tecla/comando a ser verificado.
     * @return Verdadeiro se a tecla solicitada estiver pressionada.
     */
    bool isTeclaPressionada(int codigoTecla) const;
};

#endif // GERENCIADOR_INPUT_HPP