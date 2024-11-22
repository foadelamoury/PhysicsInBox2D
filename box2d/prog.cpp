#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <cmath>
#include <vector>
#include <iostream>


// Constants
const int WINDOW_WIDTH = 1082;
const int WINDOW_HEIGHT = 920;
const float STAR_MASS = 10000000.0f;   // Mass of the star
const float G = 1e-3;                // Scaled gravitational constant
const float PIXELS_PER_METER = 30.f; // Scaling factor for Box2D to SFML

enum CollisionGroups {
	GROUP_STAR = 0x00000001,
	GROUP_PLANET = 0x00000002
};


float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}


void checkCollisions(b2World& world) {
    

    for (b2Contact* contact = world.GetContactList(); contact; contact = contact->GetNext()) {
        if (contact->IsTouching()) {
            b2Fixture* fixtureA = contact->GetFixtureA();
            b2Fixture* fixtureB = contact->GetFixtureB();

            uint16 categoryA = fixtureA->GetFilterData().categoryBits;
            uint16 categoryB = fixtureB->GetFilterData().categoryBits;

            // Check for collision between star and planet
            if ((categoryA == GROUP_STAR && categoryB == GROUP_PLANET) ||
                (categoryA == GROUP_PLANET && categoryB == GROUP_STAR))
            {
                
                if (categoryA == GROUP_PLANET)
                    fixtureA->GetBody()->SetAwake(false);
                else
                    fixtureB->GetBody()->SetAwake(false);
            }
        }
    }

  
}

// Utility function to calculate gravitational force
b2Vec2 calculateGravitationalForce(const b2Vec2& planetPos, const b2Vec2& starPos, float planetMass) {
    b2Vec2 direction = starPos - planetPos;
    float distance = direction.Length();
    if (distance < 1.0f) distance = 1.0f; // Prevent extreme values
    float forceMagnitude = G * ((STAR_MASS * planetMass) / (distance * distance));
    direction.Normalize();
    return forceMagnitude * direction;
}



// Star Class
class Star {
public:
    sf::CircleShape shape;
    b2Body* body2;
    b2Vec2 position;
  
    Star(b2World& world, float x, float y, float radius) : position(x, y) {
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

};

// Planet Class
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
        bodyDef.position.Set(x / PIXELS_PER_METER, (WINDOW_HEIGHT - y) / PIXELS_PER_METER);
        bodyDef.linearVelocity = velocity;
        body = world.CreateBody(&bodyDef);

        // Box2D Shape
        b2CircleShape circleShape;
        circleShape.m_radius = radius / PIXELS_PER_METER;

        // Box2D Fixture
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.0f;
        fixtureDef.filter.categoryBits = GROUP_PLANET;
		fixtureDef.filter.maskBits = GROUP_STAR;
        body->CreateFixture(&fixtureDef);

        // Trail
        trail = sf::VertexArray(sf::LineStrip);
    }

    void update() {
        // Update SFML position from Box2D body
        b2Vec2 position = body->GetPosition();
        shape.setPosition(position.x * PIXELS_PER_METER, WINDOW_HEIGHT - position.y * PIXELS_PER_METER);

        // Add trail point
        trail.append(sf::Vertex(shape.getPosition(), shape.getFillColor()));
    }

    void draw(sf::RenderWindow& window) {
        if (trail.getVertexCount() > 1)
            window.draw(trail);
        window.draw(shape);
    }
};

int main() {
    // SFML Window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Simple Solar System Simulation");
    window.setFramerateLimit(60);

    // Box2D World
    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);

    // Create Star
    Star star(world,WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 50);

    // List of planets
    std::vector<Planet> planets;

    // Game Loop
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Handle Events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Spawn planet on mouse click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                float mouseX = event.mouseButton.x;
                float mouseY = event.mouseButton.y;

                // Calculate position and direction relative to the star
                b2Vec2 starPos(star.position.x / PIXELS_PER_METER, (WINDOW_HEIGHT - star.position.y) / PIXELS_PER_METER);
                b2Vec2 planetPos(mouseX / PIXELS_PER_METER, (WINDOW_HEIGHT - mouseY) / PIXELS_PER_METER);
                b2Vec2 direction = planetPos - starPos;
                float distance = direction.Length();

                // Normalize the direction
                direction.Normalize();

                // Calculate tangential velocity for orbital motion
                float orbitalSpeed = std::sqrt(G * STAR_MASS / distance); // Circular orbital speed
                b2Vec2 tangentialVelocity(-direction.y * orbitalSpeed, direction.x * orbitalSpeed);

                // Create the planet with the tangential velocity
                planets.emplace_back(world, mouseX, mouseY, tangentialVelocity, randomFloat(10.0f, 40.0f), sf::Color(rand() % 256, rand() % 256, rand() % 256));
            }
        }

        // Update Physics
        for (Planet& planet : planets) {
            b2Vec2 planetPos = planet.body->GetPosition();
            b2Vec2 starPos(star.position.x / PIXELS_PER_METER, (WINDOW_HEIGHT - star.position.y) / PIXELS_PER_METER);
            b2Vec2 gravitationalForce = calculateGravitationalForce(planetPos, starPos, planet.body->GetMass());
            planet.body->ApplyForceToCenter(gravitationalForce, true);
        }
        world.Step(deltaTime, 8, 3);

        // Update Planets
        for (Planet& planet : planets)
            planet.update();

        checkCollisions(world);
        planets.erase(std::remove_if(planets.begin(), planets.end(),
            [&](Planet& planet) {
            if (!(planet.body->IsAwake()))
            {
                world.DestroyBody(planet.body);

                return true; // Mark for removal
            }
            return false;
        }), planets.end());

        // Render
        window.clear();
        window.draw(star.shape);
        for (Planet& planet : planets)
            planet.draw(window);
        window.display();
    }

    return 0;
}
