#ifndef BOSS_H
#define BOSS_H

#include <memory>
#include <vector>
#include <raylib.h>
#include "entity_data.hpp"

/** @brief Projétil do boss: velocidade própria, machuca o jogador. */
class TiroBoss : public EntityData {
public:
    TiroBoss(float x, float y, float speed, int direcao);
    virtual ~TiroBoss() = default;

    void atualizar(float dt);
    void desenhar() const;
    Rectangle getHitbox() const;
    int getDano() const;

private:
    int _direcao;
    int _dano;
    float _largura;
    float _altura;
};

/** @brief Boss estacionário: atira alto/baixo alternadamente e acelera ao perder vida. */
class Boss : public EntityData {
public:
    Boss(float x, float y);
    virtual ~Boss() = default;

    void atualizar(float dt, float alvoX, std::vector<std::unique_ptr<TiroBoss> >& tiros);
    void receberDano(int dano);
    void desenhar() const;
    Rectangle getHitbox() const;

    int getVida() const;
    int getVidaMax() const;
    int getPontos() const;

private:
    float fator() const;  /**< Multiplicador de dificuldade (1 = início, sobe conforme perde vida). */

    int _vida;
    int _vidaMax;
    int _pontos;
    float _largura;
    float _altura;
    float _cooldown;      /**< Tempo até o próximo tiro. */
    float _invulneravel;  /**< Tempo restante de invulnerabilidade após levar dano. */
    bool _proximoAlto;    /**< true: próximo tiro é alto; false: baixo. */
    int _direcao;         /**< Lado para onde o boss está virado. */
};

#endif