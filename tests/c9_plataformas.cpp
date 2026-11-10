// Testes das plataformas dinamicas do Mapa: moveis ('h', 'v') e frageis ('~').
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include "doctest.h"
#include "gerenciadores.hpp"
#include "jogador.hpp"
#include "mapa.hpp"

namespace {
/** Grava um arquivo de texto temporario e devolve o caminho. */
std::string gravar(const std::string& nome, const std::string& conteudo) {
    std::ofstream arquivo(nome.c_str(), std::ios::binary);
    arquivo << conteudo;
    return nome;
}

const float kQuadro = 1.0f / 60.0f;

// 8 colunas x 4 linhas (tile = 32 px):
//   'hhh' em (64, 0) com 96 px de largura; 'v' em (192, 0); '~' em (0, 64) e (32, 64).
const char* const kMapaDinamico = "@.hhh.vF\n........\n~~......\n########\n";

// Plataforma 'hhh' em (96, 128); sem chao: quem cair some pela borda de baixo.
const char* const kMapaMovel = "@..........F\n............\n............\n............\n...hhh......\n............\n";
const char* const kMapaVertical = "@..........F\n............\n............\n............\n...vvv......\n............\n";
// Tres tiles frageis em (96, 128), sem chao embaixo.
const char* const kMapaFragil = "@..........F\n............\n............\n............\n...~~~......\n............\n";

/** Posiciona o jogador com os pes exatamente sobre o topo de uma plataforma. */
void pousar(Jogador& j, float x, float topo) {
    j.setPosition(x, topo - j.getHitbox().height);
    j.setVelY(0.0f);
}

/** Executa um quadro igual ao de Jogo::atualizarPartida (so a parte do cenario). */
void quadro(Mapa& mapa, Jogador& j, const GerenciadorColisao& colisao) {
    const Pos carga = mapa.atualizar(kQuadro, j.getHitbox());
    j.setPosition(j.getPosition().x + carga.x, j.getPosition().y + carga.y);
    j.atualizar(kQuadro, 1500.0f);
    colisao.resolverJogadorMapa(j, mapa);
}
}  // namespace

TEST_CASE("Plataformas: o parser cria plataformas dinamicas fora da grade") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaDinamico));

    const std::vector<PlataformaMovel>& moveis = mapa.getPlataformasMoveis();
    REQUIRE(moveis.size() == 2);
    // 'hhh' vira UMA plataforma de 3 tiles.
    CHECK(moveis[0].horizontal);
    CHECK(moveis[0].area.x == doctest::Approx(64.0f));
    CHECK(moveis[0].area.width == doctest::Approx(96.0f));
    CHECK(moveis[0].area.height == doctest::Approx(32.0f));
    // 'v' isolado vira uma plataforma de 1 tile.
    CHECK_FALSE(moveis[1].horizontal);
    CHECK(moveis[1].area.x == doctest::Approx(192.0f));
    CHECK(moveis[1].area.width == doctest::Approx(32.0f));

    const std::vector<PlataformaFragil>& frageis = mapa.getPlataformasFrageis();
    REQUIRE(frageis.size() == 2);
    CHECK(frageis[0].area.x == doctest::Approx(0.0f));
    CHECK(frageis[0].area.y == doctest::Approx(64.0f));
    CHECK(frageis[1].area.x == doctest::Approx(32.0f));
    CHECK(frageis[0].estado == EstadoFragil::Estavel);

    // Nao entram na grade nem nos spawns.
    CHECK(mapa.getTile(2, 0) == TipoTile::Vazio);
    CHECK(mapa.getTile(0, 2) == TipoTile::Vazio);
    CHECK_FALSE(mapa.ehSolido(0, 2));
    CHECK(mapa.getSpawns().empty());
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas: mapa sem simbolos dinamicos nao tem plataformas") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", "@.F\n###\n"));
    CHECK(mapa.getPlataformasMoveis().empty());
    CHECK(mapa.getPlataformasFrageis().empty());
    const Pos carga = mapa.atualizar(0.1f, Rectangle{0, 0, 10, 10});
    CHECK(carga.x == doctest::Approx(0.0f));
    CHECK(carga.y == doctest::Approx(0.0f));
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas: mapa nao carregado ignora atualizar") {
    Mapa mapa;
    const Pos carga = mapa.atualizar(0.1f, Rectangle{0, 0, 10, 10});
    CHECK(carga.x == doctest::Approx(0.0f));
    CHECK(carga.y == doctest::Approx(0.0f));
}

TEST_CASE("Plataformas moveis: vao e voltam pelo alcance") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaMovel));
    const Rectangle longe = {1000, 1000, 10, 10};  // jogador fora de alcance

    mapa.atualizar(0.5f, longe);  // 80 px/s * 0.5 s = 40 px
    CHECK(mapa.getPlataformasMoveis()[0].area.x == doctest::Approx(96.0f + 40.0f));
    CHECK(mapa.getPlataformasMoveis()[0].area.y == doctest::Approx(128.0f));

    for (int i = 0; i < 20; ++i) {
        mapa.atualizar(0.1f, longe);  // passa do alcance (128 px) e volta
    }
    const float x = mapa.getPlataformasMoveis()[0].area.x;
    CHECK(x >= 96.0f - 0.01f);
    CHECK(x <= 96.0f + 128.0f + 0.01f);

    // Depois de 3,2 s (ida e volta completas) esta de novo na origem.
    mapa.reiniciarDinamicas();
    for (int i = 0; i < 32; ++i) {
        mapa.atualizar(0.1f, longe);
    }
    CHECK(mapa.getPlataformasMoveis()[0].area.x == doctest::Approx(96.0f).epsilon(0.001));
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas moveis: a vertical sobe e desce") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaVertical));
    const Rectangle longe = {1000, 1000, 10, 10};

    mapa.atualizar(0.5f, longe);  // 60 px/s * 0.5 s = 30 px para cima
    CHECK(mapa.getPlataformasMoveis()[0].area.y == doctest::Approx(128.0f - 30.0f));
    CHECK(mapa.getPlataformasMoveis()[0].area.x == doctest::Approx(96.0f));
    for (int i = 0; i < 100; ++i) {
        mapa.atualizar(0.1f, longe);
        const float y = mapa.getPlataformasMoveis()[0].area.y;
        CHECK(y <= 128.0f + 0.01f);
        CHECK(y >= 128.0f - 96.0f - 0.01f);
    }
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas moveis: entram nas consultas de colisao") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaMovel));

    CHECK(mapa.colideComSolido(Rectangle{100, 130, 10, 10}));
    CHECK_FALSE(mapa.colideComSolido(Rectangle{100, 90, 10, 10}));
    CHECK(mapa.getTilesSolidosProximos(Rectangle{100, 130, 10, 10}).size() == 1);

    // A colisao acompanha a plataforma, nao a posicao inicial.
    for (int i = 0; i < 10; ++i) {
        mapa.atualizar(0.1f, Rectangle{1000, 1000, 10, 10});  // anda 80 px
    }
    CHECK_FALSE(mapa.colideComSolido(Rectangle{100, 130, 10, 10}));
    CHECK(mapa.colideComSolido(Rectangle{180, 130, 10, 10}));
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas moveis: so carregam quem esta em cima") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaMovel));
    // Plataforma em x 96..192, topo y=128.
    const Rectangle emCima = {110, 98, 26, 30};   // pes em y=128
    const Rectangle ao_lado = {200, 98, 26, 30};  // fora da largura
    const Rectangle noAr = {110, 40, 26, 30};     // pes bem acima

    Pos carga = mapa.atualizar(0.1f, emCima);
    CHECK(carga.x == doctest::Approx(8.0f));
    CHECK(carga.y == doctest::Approx(0.0f));

    mapa.reiniciarDinamicas();
    carga = mapa.atualizar(0.1f, ao_lado);
    CHECK(carga.x == doctest::Approx(0.0f));
    mapa.reiniciarDinamicas();
    carga = mapa.atualizar(0.1f, noAr);
    CHECK(carga.x == doctest::Approx(0.0f));
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas frageis: pisar inicia a contagem e o tile some e volta") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaDinamico));
    const Rectangle noTile = {2, 34, 26, 30};     // pes em y=64, sobre o '~' de (0, 64)
    const Rectangle longe = {1000, 1000, 10, 10};
    const Rectangle regiao = {0, 64, 32, 32};

    // Sem pisar, nada acontece.
    mapa.atualizar(1.0f, longe);
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Estavel);
    CHECK(mapa.colideComSolido(regiao));

    // Pisa: comeca a tremer, mas ainda e solido.
    mapa.atualizar(0.1f, noTile);
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Tremendo);
    CHECK(mapa.getPlataformasFrageis()[1].estado == EstadoFragil::Estavel);  // o vizinho nao foi pisado
    CHECK(mapa.colideComSolido(regiao));

    // A contagem de 0,5 s comeca no quadro da pisada: aos 0,45 s ainda segura.
    mapa.atualizar(0.45f, longe);
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Tremendo);
    CHECK(mapa.colideComSolido(regiao));

    // Passado o tempo de aviso, some e nao tem mais colisao.
    mapa.atualizar(0.1f, longe);
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Ausente);
    CHECK_FALSE(mapa.colideComSolido(regiao));

    // Reaparece depois de alguns segundos.
    mapa.atualizar(2.0f, longe);
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Ausente);
    mapa.atualizar(1.5f, longe);
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Estavel);
    CHECK(mapa.colideComSolido(regiao));
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas frageis: nao reaparecem em cima do jogador") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaDinamico));
    const Rectangle noTile = {2, 34, 26, 30};
    const Rectangle dentro = {2, 60, 26, 30};  // ocupando o espaco do tile

    mapa.atualizar(0.1f, noTile);
    mapa.atualizar(0.6f, noTile);
    REQUIRE(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Ausente);

    mapa.atualizar(5.0f, dentro);  // o tempo acabou, mas o jogador esta no lugar
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Ausente);
    mapa.atualizar(0.1f, Rectangle{1000, 1000, 10, 10});  // jogador saiu
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Estavel);
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas: reiniciarDinamicas restaura o estado inicial") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaDinamico));
    mapa.atualizar(0.1f, Rectangle{2, 34, 26, 30});
    mapa.atualizar(0.7f, Rectangle{1000, 1000, 10, 10});
    REQUIRE(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Ausente);
    REQUIRE(mapa.getPlataformasMoveis()[0].area.x > 64.0f);

    mapa.reiniciarDinamicas();
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Estavel);
    CHECK(mapa.getPlataformasMoveis()[0].area.x == doctest::Approx(64.0f));
    CHECK(mapa.getPlataformasMoveis()[1].area.y == doctest::Approx(0.0f));
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Plataformas: recarregar descarta as plataformas do mapa anterior") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaDinamico));
    mapa.carregarDeArquivo(gravar("tmp_c9_din2.txt", "@.F\n###\n"));
    CHECK(mapa.getPlataformasMoveis().empty());
    CHECK(mapa.getPlataformasFrageis().empty());
    std::remove("tmp_c9_din.txt");
    std::remove("tmp_c9_din2.txt");
}

TEST_CASE("Integracao: o jogador fica em cima de uma plataforma horizontal em movimento") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaMovel));
    const GerenciadorColisao colisao;
    Jogador j(0.0f, 0.0f, 220.0f);
    pousar(j, 110.0f, 128.0f);

    int quadrosNoAr = 0;
    for (int i = 0; i < 120; ++i) {  // 2 s: vai ate o fim do trajeto e volta
        quadro(mapa, j, colisao);
        if (!j.estaNoChao()) {
            ++quadrosNoAr;
        }
        const Rectangle plataforma = mapa.getPlataformasMoveis()[0].area;
        CHECK(j.getHitbox().y + j.getHitbox().height == doctest::Approx(128.0f).epsilon(0.001));
        CHECK(j.getPosition().x - plataforma.x == doctest::Approx(110.0f - 96.0f).epsilon(0.01));
    }
    CHECK(quadrosNoAr == 0);
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Integracao: o jogador acompanha uma plataforma vertical sem cair nem tremer") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaVertical));
    const GerenciadorColisao colisao;
    Jogador j(0.0f, 0.0f, 220.0f);
    pousar(j, 110.0f, 128.0f);

    int quadrosNoAr = 0;
    for (int i = 0; i < 240; ++i) {  // 4 s: sobe 96 px (1,6 s) e desce
        quadro(mapa, j, colisao);
        if (!j.estaNoChao()) {
            ++quadrosNoAr;
        }
        const Rectangle plataforma = mapa.getPlataformasMoveis()[0].area;
        CHECK(j.getHitbox().y + j.getHitbox().height == doctest::Approx(plataforma.y).epsilon(0.01));
    }
    CHECK(quadrosNoAr == 0);
    std::remove("tmp_c9_din.txt");
}

TEST_CASE("Integracao: o jogador cai quando a plataforma fragil some") {
    Mapa mapa;
    mapa.carregarDeArquivo(gravar("tmp_c9_din.txt", kMapaFragil));
    const GerenciadorColisao colisao;
    Jogador j(0.0f, 0.0f, 220.0f);
    pousar(j, 100.0f, 128.0f);

    // Nos primeiros 0,4 s o tile ainda segura o jogador.
    for (int i = 0; i < 24; ++i) {
        quadro(mapa, j, colisao);
    }
    CHECK(j.estaNoChao());
    CHECK(j.getHitbox().y + j.getHitbox().height == doctest::Approx(128.0f).epsilon(0.001));
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Tremendo);

    // Passado o tempo de aviso, o jogador despenca.
    for (int i = 0; i < 40; ++i) {
        quadro(mapa, j, colisao);
    }
    CHECK(mapa.getPlataformasFrageis()[0].estado == EstadoFragil::Ausente);
    CHECK(j.getHitbox().y > 128.0f);
    CHECK_FALSE(j.estaNoChao());
    std::remove("tmp_c9_din.txt");
}
