#ifndef ENTITY_H
#define ENTITY_H

enum class EntityType {
    Jogador, Tiro, Inimigo, Moeda
};


struct Pos {
    double x;
    double y;
};


class EntityData {
public:
    EntityData(EntityType type, float x, float y, float speed); // Construtor de cada entidade definindo seu tipo para interações de colisão

    EntityType getType() const; //Confere qual tipo da entidade para gerenciar mecanicas
    Pos getPosition() const;
    void setPosition(float x, float y);
    
    bool confereVivo() const;
    void setAlive(bool alive);

protected:
    EntityType _type;
    Pos _posicao;
    float _vel;
    bool vivo;
};

#endif