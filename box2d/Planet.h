#include "Constants.h" 


#pragma once
class Planet {
public:
    sf::CircleShape shape;
    b2Body* body;
    sf::VertexArray trail;
    Planet(b2World& world, float x, float y, const b2Vec2& velocity, float radius, sf::Color color);

    void update();

    void draw(sf::RenderWindow& window);

    //float calculateMassBasedOnRadius(float radius);
};

