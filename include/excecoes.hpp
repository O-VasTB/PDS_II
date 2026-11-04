#ifndef EXCECOES_H
#define EXCECOES_H

#include <stdexcept>
#include <string>

/**
 * @file excecoes.hpp
 * @brief Hierarquia de exceções do jogo.
 *
 * Todas herdam de exceções da biblioteca padrão, então um `catch (const std::exception&)`
 * também as captura, e cada categoria de erro pode ser tratada separadamente:
 *
 * @code
 * std::exception
 *   |-- std::runtime_error
 *   |     `-- ErroArquivo         arquivo ausente, ilegível ou vazio
 *   `-- std::invalid_argument
 *         |-- ErroMapa            conteúdo inválido em um arquivo de fase
 *         `-- ErroValorInvalido   parâmetro ou argumento fora do domínio válido
 * @endcode
 */

/**
 * @class ErroArquivo
 * @brief Falha ao abrir ou ler um arquivo (inexistente, sem permissão, vazio ou corrompido).
 */
class ErroArquivo : public std::runtime_error {
public:
    /**
     * @brief Construtor.
     * @param mensagem Descrição do erro, incluindo o caminho do arquivo.
     */
    explicit ErroArquivo(const std::string& mensagem) : std::runtime_error(mensagem) {}
};

/**
 * @class ErroMapa
 * @brief Arquivo de fase com conteúdo inválido (linhas desiguais, símbolo desconhecido,
 *        falta do ponto de partida ou da bandeira, dimensões acima do limite).
 */
class ErroMapa : public std::invalid_argument {
public:
    /**
     * @brief Construtor.
     * @param mensagem Descrição do erro, incluindo o arquivo e, quando possível, a linha e a coluna.
     */
    explicit ErroMapa(const std::string& mensagem) : std::invalid_argument(mensagem) {}
};

/**
 * @class ErroValorInvalido
 * @brief Valor fora do domínio válido (NaN, velocidade negativa, tecla inválida, argumento
 *        de linha de comando inválido, etc.).
 */
class ErroValorInvalido : public std::invalid_argument {
public:
    /**
     * @brief Construtor.
     * @param mensagem Descrição do valor inválido.
     */
    explicit ErroValorInvalido(const std::string& mensagem) : std::invalid_argument(mensagem) {}
};

#endif
