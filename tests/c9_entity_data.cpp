// Testes de EntityData (entity_data.cpp) e da hierarquia de entidades.
#include <cmath>
#include <limits>
#include <type_traits>
#include "doctest.h"
#include "entity_data.hpp"
#include "excecoes.hpp"
#include "inimigo.hpp"
#include "itens.hpp"
#include "jogador.hpp"

TEST_CASE("EntityData: o construtor guarda tipo, posicao e estado inicial") {
    EntityData e(EntityType::Inimigo, 10.5f, -3.0f, 42.0f);
    CHECK(e.getType() == EntityType::Inimigo);
    CHECK(e.getPosition().x == doctest::Approx(10.5f));
    CHECK(e.getPosition().y == doctest::Approx(-3.0f));
    CHECK(e.confereVivo());
}

TEST_CASE("EntityData: setPosition e setAlive alteram o estado") {
    EntityData e(EntityType::Jogador, 0.0f, 0.0f, 1.0f);
    e.setPosition(5.0f, 7.5f);
    CHECK(e.getPosition().x == doctest::Approx(5.0f));
    CHECK(e.getPosition().y == doctest::Approx(7.5f));

    e.setAlive(false);
    CHECK_FALSE(e.confereVivo());
    e.setAlive(true);
    CHECK(e.confereVivo());
}

TEST_CASE("EntityData: valores invalidos sao rejeitados no construtor") {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();

    SUBCASE("posicao NaN ou infinita") {
        CHECK_THROWS_AS(EntityData(EntityType::Item, nan, 0.0f, 1.0f), ErroValorInvalido);
        CHECK_THROWS_AS(EntityData(EntityType::Item, 0.0f, nan, 1.0f), ErroValorInvalido);
        CHECK_THROWS_AS(EntityData(EntityType::Item, inf, 0.0f, 1.0f), ErroValorInvalido);
        CHECK_THROWS_AS(EntityData(EntityType::Item, 0.0f, -inf, 1.0f), ErroValorInvalido);
    }
    SUBCASE("velocidade negativa, NaN ou infinita") {
        CHECK_THROWS_AS(EntityData(EntityType::Item, 0.0f, 0.0f, -0.5f), ErroValorInvalido);
        CHECK_THROWS_AS(EntityData(EntityType::Item, 0.0f, 0.0f, nan), ErroValorInvalido);
        CHECK_THROWS_AS(EntityData(EntityType::Item, 0.0f, 0.0f, inf), ErroValorInvalido);
    }
    SUBCASE("a excecao tambem e um std::invalid_argument") {
        CHECK_THROWS_AS(EntityData(EntityType::Item, 0.0f, 0.0f, -1.0f), std::invalid_argument);
    }
    SUBCASE("valores limite validos sao aceitos") {
        CHECK_NOTHROW(EntityData(EntityType::Item, 0.0f, 0.0f, 0.0f));          // velocidade zero
        CHECK_NOTHROW(EntityData(EntityType::Item, -1000.0f, 1000.0f, 1.0f));   // posicoes negativas
    }
}

TEST_CASE("EntityData: as classes derivadas repassam a validacao") {
    CHECK_THROWS_AS(Jogador(0.0f, 0.0f, -1.0f), ErroValorInvalido);
    CHECK_THROWS_AS(Inimigo(0.0f, 0.0f, -1.0f, TipoInimigo::Andarilho), ErroValorInvalido);
    CHECK_THROWS_AS(Inimigo(std::numeric_limits<float>::quiet_NaN(), 0.0f, 1.0f, TipoInimigo::Escudo),
                    ErroValorInvalido);
    CHECK_THROWS_AS(Cogumelo(0.0f, 0.0f, -2.0f), ErroValorInvalido);
    CHECK_THROWS_AS(Estrela(0.0f, 0.0f, -2.0f), ErroValorInvalido);
    CHECK_NOTHROW(Moeda(1.0f, 2.0f));
}

TEST_CASE("EntityData: cada entidade informa o seu tipo (polimorfismo)") {
    Jogador jogador(1.0f, 2.0f, 100.0f);
    Inimigo inimigo(3.0f, 4.0f, 50.0f, TipoInimigo::Andarilho);
    Moeda moeda(5.0f, 6.0f);

    const EntityData* todas[] = {&jogador, &inimigo, &moeda};
    CHECK(todas[0]->getType() == EntityType::Jogador);
    CHECK(todas[1]->getType() == EntityType::Inimigo);
    CHECK(todas[2]->getType() == EntityType::Item);

    CHECK(todas[1]->getPosition().x == doctest::Approx(3.0f));
    CHECK(todas[2]->getPosition().y == doctest::Approx(6.0f));
}

TEST_CASE("EntityData: o destrutor e virtual (deletar pela base e seguro)") {
    // Se este teste falhar, falta `virtual ~EntityData() = default;` na parte publica da classe base.
    // (Sem ele, `delete` por um ponteiro EntityData* nao destroi a classe derivada corretamente.)
    CHECK(std::has_virtual_destructor<EntityData>::value);
    CHECK(std::has_virtual_destructor<Jogador>::value);
    CHECK(std::has_virtual_destructor<Item>::value);
}
