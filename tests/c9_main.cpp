// Testes do main.cpp: toda a validacao da linha de comando fica em argumentos.cpp,
// que o main usa; assim ela pode ser testada sem abrir o jogo.
#include <string>
#include <vector>
#include "argumentos.hpp"
#include "doctest.h"
#include "excecoes.hpp"

namespace {
/** Monta argc/argv a partir de uma lista de textos (o primeiro e o nome do programa). */
ConfiguracaoJogo interpretar(const std::vector<std::string>& args) {
    std::vector<std::string> copia = args;
    std::vector<char*> ponteiros;
    for (std::size_t i = 0; i < copia.size(); ++i) {
        ponteiros.push_back(&copia[i][0]);
    }
    ponteiros.push_back(NULL);
    return interpretarArgumentos(static_cast<int>(args.size()), ponteiros.data());
}
}  // namespace

TEST_CASE("main: sem argumentos usa os valores padrao") {
    const ConfiguracaoJogo cfg = interpretar({"jogo"});
    CHECK(cfg.largura == limites_janela::kLarguraPadrao);
    CHECK(cfg.altura == limites_janela::kAlturaPadrao);
    CHECK_FALSE(cfg.mostrarAjuda);
}

TEST_CASE("main: aceita largura e altura validas") {
    const ConfiguracaoJogo cfg = interpretar({"jogo", "--largura", "1280", "--altura", "720"});
    CHECK(cfg.largura == 1280);
    CHECK(cfg.altura == 720);

    const ConfiguracaoJogo so_largura = interpretar({"jogo", "--largura", "800"});
    CHECK(so_largura.largura == 800);
    CHECK(so_largura.altura == limites_janela::kAlturaPadrao);

    const ConfiguracaoJogo invertido = interpretar({"jogo", "--altura", "600", "--largura", "900"});
    CHECK(invertido.largura == 900);
    CHECK(invertido.altura == 600);
}

TEST_CASE("main: os limites do intervalo sao inclusivos") {
    CHECK(interpretar({"jogo", "--largura", "320"}).largura == 320);
    CHECK(interpretar({"jogo", "--largura", "3840"}).largura == 3840);
    CHECK(interpretar({"jogo", "--altura", "240"}).altura == 240);
    CHECK(interpretar({"jogo", "--altura", "2160"}).altura == 2160);

    CHECK_THROWS_AS(interpretar({"jogo", "--largura", "319"}), ErroValorInvalido);
    CHECK_THROWS_AS(interpretar({"jogo", "--largura", "3841"}), ErroValorInvalido);
    CHECK_THROWS_AS(interpretar({"jogo", "--altura", "239"}), ErroValorInvalido);
    CHECK_THROWS_AS(interpretar({"jogo", "--altura", "2161"}), ErroValorInvalido);
}

TEST_CASE("main: --ajuda pede o texto de ajuda e interrompe a leitura") {
    CHECK(interpretar({"jogo", "--ajuda"}).mostrarAjuda);
    CHECK(interpretar({"jogo", "-h"}).mostrarAjuda);
    // --ajuda vem primeiro: o argumento invalido depois dele nao e examinado
    CHECK(interpretar({"jogo", "--ajuda", "--foo"}).mostrarAjuda);
}

TEST_CASE("main: argumentos invalidos lancam ErroValorInvalido") {
    SUBCASE("valor que nao e numero") {
        CHECK_THROWS_AS(interpretar({"jogo", "--largura", "abc"}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "--largura", ""}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "--altura", "12.5x"}), ErroValorInvalido);
    }
    SUBCASE("numero seguido de lixo") {
        CHECK_THROWS_AS(interpretar({"jogo", "--largura", "12x"}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "--largura", "1280 "}), ErroValorInvalido);
    }
    SUBCASE("numero grande demais ou negativo") {
        CHECK_THROWS_AS(interpretar({"jogo", "--largura", "99999999999999999999"}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "--largura", "-800"}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "--altura", "0"}), ErroValorInvalido);
    }
    SUBCASE("falta o valor") {
        CHECK_THROWS_AS(interpretar({"jogo", "--largura"}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "--altura"}), ErroValorInvalido);
    }
    SUBCASE("argumento desconhecido") {
        CHECK_THROWS_AS(interpretar({"jogo", "--foo"}), ErroValorInvalido);
        CHECK_THROWS_AS(interpretar({"jogo", "800"}), ErroValorInvalido);
    }
    SUBCASE("a excecao e um std::invalid_argument") {
        CHECK_THROWS_AS(interpretar({"jogo", "--foo"}), std::invalid_argument);
    }
}

TEST_CASE("main: lerInteiro valida formato e intervalo") {
    CHECK(lerInteiro("42", "x", 0, 100) == 42);
    CHECK(lerInteiro("-5", "x", -10, 10) == -5);
    CHECK_THROWS_AS(lerInteiro("101", "x", 0, 100), ErroValorInvalido);
    CHECK_THROWS_AS(lerInteiro("abc", "x", 0, 100), ErroValorInvalido);

    try {
        lerInteiro("abc", "largura", 0, 100);
        FAIL("deveria ter lancado");
    } catch (const ErroValorInvalido& e) {
        CHECK(std::string(e.what()).find("largura") != std::string::npos);  // mensagem cita o parametro
    }
}

TEST_CASE("main: texto de ajuda cita o programa e os limites") {
    const std::string ajuda = textoDeAjuda("meujogo");
    CHECK(ajuda.find("meujogo") != std::string::npos);
    CHECK(ajuda.find("--largura") != std::string::npos);
    CHECK(ajuda.find("--altura") != std::string::npos);
    CHECK(ajuda.find("3840") != std::string::npos);
}
