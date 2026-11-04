/**
 * @file main.cpp
 * @brief Ponto de entrada do jogo: valida a linha de comando, cria o Jogo e o executa.
 */

#include <exception>
#include <iostream>
#include <string>
#include "argumentos.hpp"
#include "excecoes.hpp"
#include "jogo.hpp"

/**
 * @brief Função principal.
 * @param argc Quantidade de argumentos.
 * @param argv Argumentos da linha de comando (veja interpretarArgumentos()).
 * @return int 0 em sucesso; 2 para argumentos inválidos; 1 para qualquer outro erro.
 */
int main(int argc, char* argv[]) {
    const std::string programa = (argc > 0 && argv[0] != NULL) ? argv[0] : "jogo";
    try {
        const ConfiguracaoJogo cfg = interpretarArgumentos(argc, argv);
        if (cfg.mostrarAjuda) {
            std::cout << textoDeAjuda(programa);
            return 0;
        }
        Jogo jogo(cfg.largura, cfg.altura, "Jogo de Plataforma");
        jogo.executar();
    } catch (const ErroValorInvalido& e) {
        std::cerr << "Argumento invalido: " << e.what() << "\n\n" << textoDeAjuda(programa);
        return 2;
    } catch (const ErroArquivo& e) {
        std::cerr << "Erro de arquivo: " << e.what() << std::endl;
        return 1;
    } catch (const ErroMapa& e) {
        std::cerr << "Erro no mapa: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Erro desconhecido." << std::endl;
        return 1;
    }
    return 0;
}
