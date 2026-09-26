#ifndef ENGINE_H
#define ENGINE_H

#include "entity_head.hpp"

class ColisionAssist {
public:
    void calcColisao(EntityData& receptor, EntityData& agressor);
};


#endif