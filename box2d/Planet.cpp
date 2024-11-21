class Planet {
public:
    sf::CircleShape shape;
    b2Body* body;
    sf::VertexArray trail;

    Planet(b2World& world, float x, float y, const b2Vec2& velocity, float radius, sf::Color color) {
        // SFML Shape
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(x, y);
        shape.setFillColor(color);

        // Box2D Body
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(x / 30.f, (WINDOW_HEIGHT - y) / 30.f); // Convert to Box2D world
        bodyDef.linearVelocity = velocity;
        body = world.CreateBody(&bodyDef);

        // Box2D Shape
        b2CircleShape circleShape;
        circleShape.m_radius = radius / 30.f;

        // Box2D Fixture
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.0f;
        body->CreateFixture(&fixtureDef);

        // Trail
        trail = sf::VertexArray(sf::LineStrip);
    }

    void update(const b2Vec2& starPos) {
        // Update SFML position from Box2D body
        b2Vec2 position = body->GetPosition();
        shape.setPosition(position.x * 30.f, WINDOW_HEIGHT - position.y * 30.f);

        // Add trail point
        trail.append(sf::Vertex(shape.getPosition(), shape.getFillColor()));

        // Calculate distance from the star
        float distance = b2Distance(position, starPos);

        // Rotate planet if far from the star
        if (distance > 5.0f) { // Adjust the threshold as needed
            body->SetAngularVelocity(2.0f); // Add angular velocity
        } else {
            body->SetAngularVelocity(0.0f); // Stop rotation
        }

        // Apply rotation to SFML shape
        shape.setRotation(body->GetAngle() * 180 / b2_pi); // Convert radians to degrees
    }

    void draw(sf::RenderWindow& window) {
        if (trail.getVertexCount() > 1)
            window.draw(trail);
        window.draw(shape);
    }
};
