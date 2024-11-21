#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <cmath>
#include <vector>

class Planet {
public:
    sf::CircleShape shape;
    b2Body* body;
    sf::VertexArray trail;
    Planet(b2World& world, float x, float y, const b2Vec2& velocity, float radius, sf::Color color)
    void Planet::update(const b2Vec2& starPos);
    void Planet::draw(sf::RenderWindow& window);
};