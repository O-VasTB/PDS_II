#include "doctest.h"
#include "mapa.hpp"

TEST_CASE("Teste do construtor e dimensões iniciais do Mapa") {
    Mapa mapa;
    
    // Verifica se a largura e a altura inicializam com valores coerentes (não negativos)
    CHECK(mapa.getLargura() >= 0);
    CHECK(mapa.getAltura() >= 0);
}

TEST_CASE("Teste de carregamento de mapa com caminhos inválidos") {
    Mapa mapa;
    
    // Tentar carregar um arquivo de dados ou imagem que não existe deve retornar falso
    bool carregou = mapa.carregarMapa("caminho/invalido/dados.txt", "caminho/invalido/tileset.png");
    CHECK_FALSE(carregou);
}

TEST_CASE("Teste de alteração de estado de bloco") {
    Mapa mapa;
    
    // Garante que a chamada da função de alterar estado não cause falhas ou exceções inesperadas
    CHECK_NOTHROW(mapa.alterarEstadoBloco(0, 0));
}

TEST_CASE("Teste das propriedades básicas da estrutura Bloco") {
    Bloco blocoExemplo;
    blocoExemplo.solido = true;
    blocoExemplo.destrutivel = false;
    blocoExemplo.transponivel = false;
    blocoExemplo.tipo = 1;

    CHECK(blocoExemplo.solido == true);
    CHECK_FALSE(blocoExemplo.destrutivel);
    CHECK_FALSE(blocoExemplo.transponivel);
    CHECK_EQ(blocoExemplo.tipo, 1);
}