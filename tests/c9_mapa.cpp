// Testes de Mapa (mapa.cpp): carga, consultas de tiles, colisao e erros.
#include <cstdio>
#include <fstream>
#include <string>
#include "doctest.h"
#include "excecoes.hpp"
#include "mapa.hpp"

namespace {
/** Grava um arquivo de texto temporario e devolve o caminho. */
std::string gravar(const std::string& nome, const std::string& conteudo) {
    std::ofstream arquivo(nome.c_str(), std::ios::binary);
    arquivo << conteudo;
    return nome;
}

// 6 colunas x 3 linhas (tile = 32 px):
//   linha 0: "...o.."   moeda em (96, 0)
//   linha 1: "@.g.mF"   jogador em (0, 32), andarilho (64, 32), cogumelo (128, 32), bandeira em (160, 32)
//   linha 2: "######"   chao de y=64 a y=96
const char* const kMapa = "...o..\n@.g.mF\n######\n";
}  // namespace

TEST_CASE("Mapa: recem-criado esta vazio e as consultas sao seguras") {
    Mapa mapa;
    CHECK_FALSE(mapa.estaCarregado());
    CHECK(mapa.getColunas() == 0);
    CHECK(mapa.getLinhas() == 0);
    CHECK(mapa.getTile(0, 0) == TipoTile::Vazio);
    CHECK_FALSE(mapa.ehSolido(0, 0));
    CHECK_FALSE(mapa.colideComSolido(Rectangle{0, 0, 10, 10}));
    CHECK_FALSE(mapa.tocaBandeira(Rectangle{0, 0, 10, 10}));
    CHECK(mapa.getSpawns().empty());
}

TEST_CASE("Mapa: carrega um arquivo valido") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_mapa.txt", kMapa));
    REQUIRE(mapa.estaCarregado());

    SUBCASE("dimensoes") {
        CHECK(mapa.getColunas() == 6);
        CHECK(mapa.getLinhas() == 3);
        CHECK(mapa.getTamanhoTile() == doctest::Approx(32.0f));
        CHECK(mapa.getLarguraPx() == doctest::Approx(192.0f));
        CHECK(mapa.getAlturaPx() == doctest::Approx(96.0f));
    }
    SUBCASE("tipos de tile") {
        CHECK(mapa.getTile(0, 2) == TipoTile::Chao);
        CHECK(mapa.getTile(5, 1) == TipoTile::Bandeira);
        CHECK(mapa.getTile(0, 1) == TipoTile::Vazio);   // '@' nao vira tile
        CHECK(mapa.getTile(2, 1) == TipoTile::Vazio);   // 'g' nao vira tile
    }
    SUBCASE("solidez") {
        CHECK(mapa.ehSolido(3, 2));
        CHECK_FALSE(mapa.ehSolido(5, 1));   // bandeira nao e solida
        CHECK_FALSE(mapa.ehSolido(0, 1));
    }
    SUBCASE("limites: fora do mapa e sempre vazio") {
        CHECK(mapa.getTile(-1, 0) == TipoTile::Vazio);
        CHECK(mapa.getTile(0, -1) == TipoTile::Vazio);
        CHECK(mapa.getTile(6, 0) == TipoTile::Vazio);
        CHECK(mapa.getTile(0, 3) == TipoTile::Vazio);
        CHECK_FALSE(mapa.ehSolido(100, 100));
    }
    SUBCASE("pontos de surgimento") {
        CHECK(mapa.getSpawnJogador().x == doctest::Approx(0.0f));
        CHECK(mapa.getSpawnJogador().y == doctest::Approx(32.0f));

        const std::vector<SpawnEntidade>& spawns = mapa.getSpawns();
        REQUIRE(spawns.size() == 3);  // 'o', 'g' e 'm', na ordem de leitura
        CHECK(spawns[0].simbolo == 'o');
        CHECK(spawns[0].posicao.x == doctest::Approx(96.0f));
        CHECK(spawns[0].posicao.y == doctest::Approx(0.0f));
        CHECK(spawns[1].simbolo == 'g');
        CHECK(spawns[1].posicao.x == doctest::Approx(64.0f));
        CHECK(spawns[2].simbolo == 'm');
        CHECK(spawns[2].posicao.x == doctest::Approx(128.0f));
    }
    std::remove("tmp_c9_mapa.txt");
}

TEST_CASE("Mapa: colisao com tiles solidos") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_mapa.txt", kMapa));

    CHECK(mapa.colideComSolido(Rectangle{0, 60, 10, 10}));        // entra 6 px no chao
    CHECK_FALSE(mapa.colideComSolido(Rectangle{0, 50, 10, 10}));  // acima do chao
    CHECK_FALSE(mapa.colideComSolido(Rectangle{0, 54, 10, 10}));  // encostando (y=64): nao colide
    CHECK_FALSE(mapa.colideComSolido(Rectangle{500, 60, 10, 10})); // fora do mapa

    // Um retangulo sobre a divisa de dois tiles toca os dois.
    const std::vector<Rectangle> tiles = mapa.getTilesSolidosProximos(Rectangle{20, 60, 20, 10});
    CHECK(tiles.size() == 2);
    for (std::size_t i = 0; i < tiles.size(); ++i) {
        CHECK(tiles[i].width == doctest::Approx(32.0f));
        CHECK(tiles[i].y == doctest::Approx(64.0f));
    }
    CHECK(mapa.getTilesSolidosProximos(Rectangle{0, 0, 10, 10}).empty());
    std::remove("tmp_c9_mapa.txt");
}

TEST_CASE("Mapa: bandeira de fim de fase") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_mapa.txt", kMapa));
    CHECK(mapa.tocaBandeira(Rectangle{165, 40, 5, 5}));
    CHECK_FALSE(mapa.tocaBandeira(Rectangle{100, 40, 5, 5}));
    std::remove("tmp_c9_mapa.txt");
}

TEST_CASE("Mapa: tolera fim de linha do Windows e linhas em branco no final") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_crlf.txt", "@.F\r\n###\r\n\r\n\r\n"));
    CHECK(mapa.getColunas() == 3);
    CHECK(mapa.getLinhas() == 2);
    std::remove("tmp_c9_crlf.txt");
}

TEST_CASE("Mapa: recarregar substitui o conteudo anterior") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_a.txt", kMapa));
    mapa.carregarDeArquivo(gravar("tmp_c9_b.txt", "@F\n##\n"));
    CHECK(mapa.getColunas() == 2);
    CHECK(mapa.getLinhas() == 2);
    CHECK(mapa.getSpawns().empty());
    std::remove("tmp_c9_a.txt");
    std::remove("tmp_c9_b.txt");
}

TEST_CASE("Mapa: arquivos invalidos lancam excecoes e nao alteram o mapa carregado") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_mapa.txt", kMapa));

    CHECK_THROWS_AS(mapa.carregarDeArquivo("nao_existe.txt"), ErroArquivo);
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_c9_x.txt", "")), ErroArquivo);
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_c9_x.txt", "@.F\n..\n")), ErroMapa);
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_c9_x.txt", "@.XF\n####\n")), ErroMapa);
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_c9_x.txt", "...F\n####\n")), ErroMapa);   // sem '@'
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_c9_x.txt", "@...\n####\n")), ErroMapa);   // sem 'F'
    CHECK_THROWS_AS(mapa.carregarDeArquivo(gravar("tmp_c9_x.txt", "@.@F\n####\n")), ErroMapa);   // dois '@'

    CHECK(mapa.estaCarregado());       // o mapa anterior segue valido
    CHECK(mapa.getColunas() == 6);
    CHECK(mapa.getSpawns().size() == 3);
    std::remove("tmp_c9_mapa.txt");
    std::remove("tmp_c9_x.txt");
}
