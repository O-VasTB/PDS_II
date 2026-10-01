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
     * @brief Verifica se uma determinada tecla foi pressionada.
     * @return Verdadeiro se a tecla estiver pressionada.
     */
    bool isTeclaPressionada() const;
};

#endif // GERENCIADOR_INPUT_HPP