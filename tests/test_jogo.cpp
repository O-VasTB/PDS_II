/**
 * @file test_jogo.cpp
 * @brief Testes de unidade (doctest) da classe Jogo.
 *
 * Seguem a abordagem TDD: escritos antes da implementação, portanto
 * ainda não devem passar.
 */

#include "doctest.h"

#include "../include/Jogo.hpp"

TEST_CASE("Jogo - Construtor e Estado Inicial") {
    Jogo jogo;
    CHECK(jogo.getEstado() == EstadoJogo::MENU);
}

TEST_CASE("Jogo - estaRodando e encerrar") {
    Jogo jogo;

    SUBCASE("recem criado esta rodando") {
        CHECK(jogo.estaRodando() == true);
    }

    SUBCASE("encerrar para o loop") {
        jogo.encerrar();
        CHECK(jogo.estaRodando() == false);
    }

    SUBCASE("encerrar duas vezes continua parado") {
        jogo.encerrar();
        jogo.encerrar();
        CHECK(jogo.estaRodando() == false);
    }
}

TEST_CASE("Jogo - setEstado e getEstado") {
    Jogo jogo;

    SUBCASE("MENU para JOGANDO") {
        jogo.setEstado(EstadoJogo::JOGANDO);
    }
}