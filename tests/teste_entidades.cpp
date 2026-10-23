#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Inimigo.hpp"
#include "Item.hpp"

// --- Testes para a classe Inimigo ---

TEST_CASE("Inimigo - Construtor e Inicializacao basica") {
    // Testa Goomba
    Inimigo goomba(100.0f, 200.0f, TipoInimigo::GOOMBA, 50.0f, 150.0f);
    
    CHECK(goomba.getTipoInimigo() == TipoInimigo::GOOMBA);
    CHECK(goomba.getEstado() == EstadoInimigo::ANDANDO);
    CHECK(goomba.estaAndando() == true);
    // Como a direção inicial depende da sua lógica de implementação (ex: sempre direita primeiro), 
    // testamos se é válida (-1 ou 1)
    CHECK((goomba.getDirecao() == 1 || goomba.getDirecao() == -1));
    CHECK(goomba.getPontos() > 0); 
    CHECK(goomba.getDano() > 0);
    CHECK(goomba.getLargura() > 0.0f);
    CHECK(goomba.getAltura() > 0.0f);
    
    // Testa inversão automática de limites (construtor robusto)
    Inimigo koopa_limites_invertidos(100.0f, 200.0f, TipoInimigo::KOOPA, 150.0f, 50.0f);
    CHECK(koopa_limites_invertidos.getTipoInimigo() == TipoInimigo::KOOPA);
    // Esperamos que ele tenha lidado com os limites corretamente (isso será validado indiretamente pelo movimento depois)
}

TEST_CASE("Inimigo - Movimento e Inversão de Direção") {
    Inimigo inimigo(100.0f, 200.0f, TipoInimigo::GOOMBA, 50.0f, 150.0f);
    
    int direcaoInicial = inimigo.getDirecao();
    float velX = inimigo.getVelocidadeX();
    
    // Confirma que a velocidade acompanha a direção
    if (direcaoInicial == 1) {
        CHECK(velX > 0.0f);
    } else {
        CHECK(velX < 0.0f);
    }

    inimigo.inverterDirecao();
    CHECK(inimigo.getDirecao() == -direcaoInicial);
    
    // A velocidade X deve inverter o sinal
    float velXInvertida = inimigo.getVelocidadeX();
    CHECK(velXInvertida == -velX);
}

TEST_CASE("Inimigo - Interações e Estados (Esmagar e Matar)") {
    SUBCASE("Esmagar altera estado para ESMAGADO e remove dano/velocidade") {
        Inimigo goomba(100.0f, 200.0f, TipoInimigo::GOOMBA, 50.0f, 150.0f);
        
        CHECK(goomba.getEstado() == EstadoInimigo::ANDANDO);
        goomba.esmagar();
        
        CHECK(goomba.getEstado() == EstadoInimigo::ESMAGADO);
        CHECK(goomba.estaAndando() == false);
        CHECK(goomba.getVelocidadeX() == 0.0f);
        CHECK(goomba.getDano() == 0);
    }

    SUBCASE("Matar altera estado para MORTO diretamente") {
        Inimigo koopa(100.0f, 200.0f, TipoInimigo::KOOPA, 50.0f, 150.0f);
        
        koopa.matar();
        CHECK(koopa.getEstado() == EstadoInimigo::MORTO);
        CHECK(koopa.estaAndando() == false);
    }
}


// --- Testes para a classe Item ---

TEST_CASE("Item - Construtor e Inicializacao basica") {
    SUBCASE("Inicializacao de Moeda") {
        Item moeda(50.0f, 50.0f, TipoItem::MOEDA);
        
        CHECK(moeda.getTipoItem() == TipoItem::MOEDA);
        CHECK(moeda.foiColetado() == false);
        CHECK(moeda.getPontos() > 0);
        CHECK(moeda.getLargura() > 0.0f);
        CHECK(moeda.getAltura() > 0.0f);
        CHECK(moeda.getTempoAnimacao() == 0.0f);
    }

    SUBCASE("Inicializacao de Cogumelo") {
        Item cogumelo(100.0f, 100.0f, TipoItem::COGUMELO);
        CHECK(cogumelo.getTipoItem() == TipoItem::COGUMELO);
        CHECK(cogumelo.foiColetado() == false);
        CHECK(cogumelo.getPontos() > 0); 
    }
    
    SUBCASE("Inicializacao de Estrela") {
        Item estrela(150.0f, 150.0f, TipoItem::ESTRELA);
        CHECK(estrela.getTipoItem() == TipoItem::ESTRELA);
        CHECK(estrela.foiColetado() == false);
        CHECK(estrela.getPontos() > 0);
    }
}

TEST_CASE("Item - Logica de Coleta") {
    Item cogumelo(100.0f, 100.0f, TipoItem::COGUMELO);
    
    int pontosEsperados = cogumelo.getPontos();
    
    // Primeira coleta deve retornar os pontos e marcar como coletado
    int pontosObtidos = cogumelo.coletar();
    
    CHECK(pontosObtidos == pontosEsperados);
    CHECK(cogumelo.foiColetado() == true);
    
    // Coletas subsequentes devem retornar 0
    int pontosObtidosNovamente = cogumelo.coletar();
    CHECK(pontosObtidosNovamente == 0);
    CHECK(cogumelo.foiColetado() == true);
}

TEST_CASE("Item - Atualizacao de Animacao") {
    Item moeda(50.0f, 50.0f, TipoItem::MOEDA);
    
    CHECK(moeda.getTempoAnimacao() == 0.0f);
    
    float dt = 0.016f; // Simulando 1 frame a ~60FPS
    moeda.atualizar(dt);
    
    CHECK(moeda.getTempoAnimacao() == doctest::Approx(0.016f));
    
    moeda.atualizar(dt);
    CHECK(moeda.getTempoAnimacao() == doctest::Approx(0.032f));
}