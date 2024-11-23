
#pragma once

#include "Constants.h"

class Star {
public:
    sf::CircleShape shape;
    b2Body* body2;
    b2Vec2 position;

    Star(b2World& world, float x, float y, float radius);

};
