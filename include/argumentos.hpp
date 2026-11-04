#ifndef ARGUMENTOS_H
#define ARGUMENTOS_H

#include <string>

/**
 * @file argumentos.hpp
 * @brief Leitura e validação dos argumentos de linha de comando do jogo.
 *
 * Uso: jogo [--largura N] [--altura N] [--ajuda]
 */

/** @brief Limites e valores padrão da janela, em pixels. */
namespace limites_janela {
const int kLarguraPadrao = 1280;  /**< Largura padrão. */
const int kAlturaPadrao = 720;   /**< Altura padrão. */
const int kLarguraMin = 320;     /**< Menor largura aceita. */
const int kLarguraMax = 3840;    /**< Maior largura aceita. */
const int kAlturaMin = 240;      /**< Menor altura aceita. */
const int kAlturaMax = 2160;     /**< Maior altura aceita. */
}  // namespace limites_janela

/**
 * @struct ConfiguracaoJogo
 * @brief Configuração obtida a partir da linha de comando.
 */
struct ConfiguracaoJogo {
    int largura;       /**< Largura da janela, em pixels. */
    int altura;        /**< Altura da janela, em pixels. */
    bool mostrarAjuda; /**< Verdadeiro se o usuário pediu --ajuda (o jogo não deve iniciar). */
};

/**
 * @brief Converte um texto em inteiro, validando formato e intervalo.
 * @param texto Texto digitado pelo usuário.
 * @param nome Nome do parâmetro (usado nas mensagens de erro).
 * @param minimo Menor valor aceito.
 * @param maximo Maior valor aceito.
 * @return int Valor convertido.
 * @throw ErroValorInvalido Se o texto não for um inteiro, tiver lixo no final ou estiver fora do intervalo.
 */
int lerInteiro(const std::string& texto, const std::string& nome, int minimo, int maximo);

/**
 * @brief Interpreta os argumentos de linha de comando.
 * @param argc Quantidade de argumentos (como recebida pelo main).
 * @param argv Argumentos; argv[0] é o nome do programa e é ignorado.
 * @return ConfiguracaoJogo Largura e altura validadas; se aparecer --ajuda, a leitura
 *         termina imediatamente com mostrarAjuda verdadeiro.
 * @throw ErroValorInvalido Se faltar valor, o valor for inválido ou o argumento for desconhecido.
 */
ConfiguracaoJogo interpretarArgumentos(int argc, char* argv[]);

/**
 * @brief Monta o texto de ajuda exibido por --ajuda e após erros de argumento.
 * @param programa Nome do executável, usado na linha de uso.
 * @return std::string Texto de ajuda, terminado em quebra de linha.
 */
std::string textoDeAjuda(const std::string& programa);

#endif
