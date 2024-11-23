#include "Star.h"

Star::Star(b2World& world, float x, float y, float radius)
    : position(x, y) {
    // SFML Shape
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::Yellow);

    // Box2D Body
    b2BodyDef bodyDef2;
    bodyDef2.type = b2_kinematicBody;
    bodyDef2.position.Set(x / PIXELS_PER_METER, (WINDOW_HEIGHT - y) / PIXELS_PER_METER);
    body2 = world.CreateBody(&bodyDef2);

    // Box2D Fixture
    b2CircleShape circleShape;
    circleShape.m_radius = radius / PIXELS_PER_METER;

    b2FixtureDef fixtureDef2;
    fixtureDef2.shape = &circleShape;
    fixtureDef2.filter.categoryBits = GROUP_STAR;
    //fixtureDef2.filter.maskBits = GROUP_PLANET;
    body2->CreateFixture(&fixtureDef2);
}
