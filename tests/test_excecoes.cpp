
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>
#include "doctest.h"
#include "excecoes.hpp"
#include "gerenciadores.hpp"
#include "jogador.hpp"
#include "mapa.hpp"

namespace {
/** Grava um arquivo de texto temporário e devolve o caminho. */
std::string gravar(const std::string& nome, const std::string& conteudo) {
    std::ofstream arquivo(nome.c_str());
    arquivo << conteudo;
    return nome;
}
const char* const kMapaValido = "....F\n@...F\n#####\n";
}  // namespace

TEST_CASE("Mapa: arquivos invalidos lancam a excecao correta") {
    Mapa mapa;

    SUBCASE("arquivo inexistente") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo("nao_existe.txt"), ErroArquivo);
    }
    SUBCASE("arquivo vazio") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_vazio.txt", "")), ErroArquivo);
    }
    SUBCASE("linhas de tamanhos diferentes") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_torto.txt", "@..F\n..\n")), ErroMapa);
    }
    SUBCASE("simbolo desconhecido") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_simbolo.txt", "@.XF\n####\n")), ErroMapa);
    }
    SUBCASE("sem ponto de partida") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_sem_inicio.txt", "...F\n####\n")), ErroMapa);
    }
    SUBCASE("sem bandeira") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_sem_bandeira.txt", "@...\n####\n")), ErroMapa);
    }
    SUBCASE("dois pontos de partida") {
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_dois_inicios.txt", "@.@F\n####\n")), ErroMapa);
    }
    SUBCASE("mapa alto demais") {
        std::string muitas;
        for (int i = 0; i < 150; ++i) muitas += "@F\n";
        CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_alto.txt", muitas)), ErroMapa);
    }
    CHECK_FALSE(mapa.estaCarregado());
    std::remove("tmp_vazio.txt"); std::remove("tmp_torto.txt"); std::remove("tmp_simbolo.txt");
    std::remove("tmp_sem_inicio.txt"); std::remove("tmp_sem_bandeira.txt");
    std::remove("tmp_dois_inicios.txt"); std::remove("tmp_alto.txt");
}

TEST_CASE("Mapa: as excecoes tambem sao capturaveis como tipos da biblioteca padrao") {
    Mapa mapa;
    CHECK_THROWS_AS(mapa.carregarDeArquivo("nao_existe.txt"), std::runtime_error);
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_t.txt", "@..\n..\n")), std::invalid_argument);
    std::remove("tmp_t.txt");
}

TEST_CASE("Mapa: carga bem-sucedida e falha preserva o mapa anterior") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_ok.txt", kMapaValido));
    REQUIRE(mapa.estaCarregado());
    CHECK(mapa.getColunas() == 5);
    CHECK(mapa.getLinhas() == 3);

    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_ruim.txt", "@..\n..\n")), ErroMapa);
    CHECK(mapa.estaCarregado());       // o mapa anterior continua intacto
    CHECK(mapa.getColunas() == 5);
    std::remove("tmp_ok.txt"); std::remove("tmp_ruim.txt");
}

TEST_CASE("Mapa: consultas fora dos limites sao seguras") {
    Mapa mapa;  // sem nada carregado
    CHECK(mapa.getTile(0, 0) == TipoTile::Vazio);
    CHECK_FALSE(mapa.ehSolido(-1, -1));
    CHECK(mapa.getTilesSolidosProximos(Rectangle{0, 0, 10, 10}).empty());
    CHECK_FALSE(mapa.tocaBandeira(Rectangle{0, 0, 10, 10}));
}

TEST_CASE("Entidades: parametros invalidos sao rejeitados no construtor") {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    CHECK_THROWS_AS(Jogador(nan, 0.0f, 100.0f), ErroValorInvalido);
    CHECK_THROWS_AS(Jogador(0.0f, inf, 100.0f), ErroValorInvalido);
    CHECK_THROWS_AS(Jogador(0.0f, 0.0f, -5.0f), ErroValorInvalido);
    CHECK_THROWS_AS(Jogador(0.0f, 0.0f, nan), ErroValorInvalido);
    CHECK_NOTHROW(Jogador(0.0f, 0.0f, 0.0f));
}

TEST_CASE("Jogador: pontuacao ignora negativos e nao estoura") {
    Jogador j(0.0f, 0.0f, 100.0f);
    j.adicionarPontos(-50);
    CHECK(j.getPontuacao() == 0);
    j.adicionarPontos(100);
    CHECK(j.getPontuacao() == 100);
    j.adicionarPontos(std::numeric_limits<int>::max());
    j.adicionarPontos(std::numeric_limits<int>::max());
    CHECK(j.getPontuacao() == 999999999);
}

TEST_CASE("Jogador: acoes em estado invalido sao ignoradas") {
    Jogador j(0.0f, 0.0f, 100.0f);
    j.pular();                       // no ar: nao pula
    CHECK(j.getVelY() == doctest::Approx(0.0f));
    j.moverDireita(-1.0f);           // dt negativo: ignorado
    CHECK(j.getPosition().x == doctest::Approx(0.0f));
    j.levarDano();
    CHECK_FALSE(j.confereVivo());
    const int vidas = j.getVidas();
    j.levarDano();                   // ja morto: nao perde outra vida
    CHECK(j.getVidas() == vidas);
}

TEST_CASE("GerenciadorInput: tecla invalida e rejeitada") {
    GerenciadorInput input;
    CHECK_THROWS_AS(input.setTecla(Acao::Pular, 0), ErroValorInvalido);
    CHECK_THROWS_AS(input.setTecla(Acao::Pular, -3), std::invalid_argument);
    CHECK(input.getTecla(Acao::Pular) == 32);  // KEY_SPACE: valor anterior preservado
}
