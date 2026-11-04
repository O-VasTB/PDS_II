/**
 * @file argumentos.cpp
 * @brief Implementação da leitura e validação dos argumentos de linha de comando.
 */

#include <cstddef>
#include <stdexcept>
#include "argumentos.hpp"
#include "excecoes.hpp"

int lerInteiro(const std::string& texto, const std::string& nome, int minimo, int maximo) {
    std::size_t usados = 0;
    long valor = 0;
    try {
        valor = std::stol(texto, &usados);
    } catch (const std::invalid_argument&) {
        throw ErroValorInvalido("Valor nao numerico para " + nome + ": '" + texto + "'");
    } catch (const std::out_of_range&) {
        throw ErroValorInvalido("Valor grande demais para " + nome + ": '" + texto + "'");
    }
    if (usados != texto.size()) {
        throw ErroValorInvalido("Valor invalido para " + nome + ": '" + texto + "'");
    }
    if (valor < minimo || valor > maximo) {
        throw ErroValorInvalido(nome + " deve estar entre " + std::to_string(minimo) + " e " +
                                std::to_string(maximo));
    }
    return static_cast<int>(valor);
}

ConfiguracaoJogo interpretarArgumentos(int argc, char* argv[]) {
    ConfiguracaoJogo cfg;
    cfg.largura = limites_janela::kLarguraPadrao;
    cfg.altura = limites_janela::kAlturaPadrao;
    cfg.mostrarAjuda = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = (argv[i] != NULL) ? argv[i] : "";
        if (arg == "--ajuda" || arg == "-h") {
            cfg.mostrarAjuda = true;
            return cfg;
        }
        if (arg == "--largura" || arg == "--altura") {
            if (i + 1 >= argc || argv[i + 1] == NULL) {
                throw ErroValorInvalido("Falta o valor depois de " + arg);
            }
            const std::string valor = argv[++i];
            if (arg == "--largura") {
                cfg.largura = lerInteiro(valor, "largura", limites_janela::kLarguraMin,
                                         limites_janela::kLarguraMax);
            } else {
                cfg.altura = lerInteiro(valor, "altura", limites_janela::kAlturaMin,
                                        limites_janela::kAlturaMax);
            }
        } else {
            throw ErroValorInvalido("Argumento desconhecido: " + arg);
        }
    }
    return cfg;
}

std::string textoDeAjuda(const std::string& programa) {
    return "Uso: " + programa + " [--largura N] [--altura N] [--ajuda]\n" +
           "  --largura N   largura da janela em pixels (" + std::to_string(limites_janela::kLarguraMin) +
           " a " + std::to_string(limites_janela::kLarguraMax) + ", padrao " +
           std::to_string(limites_janela::kLarguraPadrao) + ")\n" +
           "  --altura N    altura da janela em pixels (" + std::to_string(limites_janela::kAlturaMin) +
           " a " + std::to_string(limites_janela::kAlturaMax) + ", padrao " +
           std::to_string(limites_janela::kAlturaPadrao) + ")\n" +
           "  --ajuda       mostra esta mensagem\n";
}
