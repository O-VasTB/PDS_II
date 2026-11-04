// Testes de Jogo (jogo.cpp). O laco principal (executar) precisa de janela e teclado e e
// testado manualmente; aqui validamos construcao, estados e as protecoes da classe.
#include <memory>
#include <type_traits>
#include <raylib.h>
#include "doctest.h"
#include "excecoes.hpp"
#include "jogo.hpp"

namespace {
/**
 * Cria um Jogo com janela oculta. Devolve nullptr se o ambiente nao tiver video
 * (por exemplo, um servidor de integracao continua), para o teste ser ignorado em vez de falhar.
 */
std::unique_ptr<Jogo> criarJogoOculto() {
    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    try {
        return std::unique_ptr<Jogo>(new Jogo(640, 360, "teste"));
    } catch (const std::exception&) {
        return std::unique_ptr<Jogo>();
    }
}
}  // namespace

TEST_CASE("Jogo: dimensoes invalidas sao rejeitadas antes de abrir a janela") {
    CHECK_THROWS_AS(Jogo(0, 540, "teste"), ErroValorInvalido);
    CHECK_THROWS_AS(Jogo(960, 0, "teste"), ErroValorInvalido);
    CHECK_THROWS_AS(Jogo(-10, -10, "teste"), ErroValorInvalido);
    CHECK_THROWS_AS(Jogo(0, 0, "teste"), std::invalid_argument);
}

TEST_CASE("Jogo: nao pode ser copiado (a janela e um recurso unico)") {
    CHECK_FALSE(std::is_copy_constructible<Jogo>::value);
    CHECK_FALSE(std::is_copy_assignable<Jogo>::value);
}

TEST_CASE("Jogo: comeca no menu principal") {
    std::unique_ptr<Jogo> jogo = criarJogoOculto();
    if (!jogo) {
        MESSAGE("Sem janela disponivel neste ambiente: teste ignorado");
        return;
    }
    CHECK(jogo->getEstado() == EstadoJogo::MenuPrincipal);
}

TEST_CASE("Jogo: pode ser criado e destruido varias vezes") {
    for (int i = 0; i < 3; ++i) {
        std::unique_ptr<Jogo> jogo = criarJogoOculto();
        if (!jogo) {
            MESSAGE("Sem janela disponivel neste ambiente: teste ignorado");
            return;
        }
        CHECK(jogo->getEstado() == EstadoJogo::MenuPrincipal);
    }
}
