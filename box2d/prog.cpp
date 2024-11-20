#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <sstream>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PIPE_SPEED = -200.0f;
const float GRAVITY = 9.8f;
const float JUMP_FORCE = 5.0f;

// Utility function to convert Box2D positions to SFML
sf::Vector2f box2dToSFML(const b2Vec2& position) {
    return { position.x * 30.f, WINDOW_HEIGHT - position.y * 30.f };
}

// Game Object Classes
class Bird {
public:
    sf::CircleShape shape;
    b2Body* body;

    Bird(b2World& world) {
        // SFML Shape
        shape.setRadius(15);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(15, 15);
        shape.setPosition(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2);

        // Box2D Body
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((WINDOW_WIDTH / 4) / 30.f, (WINDOW_HEIGHT / 2) / 30.f);
        body = world.CreateBody(&bodyDef);

        // Box2D Shape
        b2CircleShape circle;
        circle.m_radius = shape.getRadius() / 30.f;

        // Box2D Fixture
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circle;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.0f;
        fixtureDef.restitution = 0.0f;
        body->CreateFixture(&fixtureDef);
    }

    void update() {
        b2Vec2 position = body->GetPosition();
        shape.setPosition(box2dToSFML(position));
    }

    void jump() {
        b2Vec2 velocity = body->GetLinearVelocity();
        body->SetLinearVelocity(b2Vec2(velocity.x, JUMP_FORCE));
    }
};

class Pipe {
public:
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;

    Pipe(float x, float gapY) {
        topPipe.setSize({ 50, gapY });
        topPipe.setFillColor(sf::Color::Green);
        topPipe.setPosition(x, 0);

        bottomPipe.setSize({ 50, WINDOW_HEIGHT - gapY - 150 });
        bottomPipe.setFillColor(sf::Color::Green);
        bottomPipe.setPosition(x, gapY + 150);
    }

    void move(float deltaTime) {
        float dx = PIPE_SPEED * deltaTime;
        topPipe.move(dx, 0);
        bottomPipe.move(dx, 0);
    }

    bool isOffScreen() const {
        return topPipe.getPosition().x + topPipe.getSize().x < 0;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(topPipe);
        window.draw(bottomPipe);
    }
};

int main() {
    // SFML Setup
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird");
    window.setFramerateLimit(60);

    // Box2D Setup
    b2Vec2 gravity(0.0f, -GRAVITY);
    b2World world(gravity);

    // Create Bird
    Bird bird(world);

    // Pipes
    std::vector<Pipe> pipes;
    float pipeSpawnTimer = 0.0f;

    // Scoring
    int score = 0;
    sf::Font font;
    font.loadFromFile("C:/Game Development/C++/box2dProj/box2d/PhysicsInBox2D/Fonts/PixelifySans.ttf");
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(WINDOW_WIDTH / 2 - 20, 10);

    // Game Loop
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Event Handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                bird.jump();
            }
        }

        // Update Physics
        world.Step(deltaTime, 8, 3);

        // Update Bird
        bird.update();

        // Spawn Pipes
        pipeSpawnTimer += deltaTime;
        if (pipeSpawnTimer >= 2.0f) {
            float gapY = rand() % (WINDOW_HEIGHT - 200) + 50;
            pipes.emplace_back(WINDOW_WIDTH, gapY);
            pipeSpawnTimer = 0.0f;
        }

        // Update Pipes
        for (auto& pipe : pipes)
            pipe.move(deltaTime);

        // Remove Off-Screen Pipes
        pipes.erase(std::remove_if(pipes.begin(), pipes.end(), [](const Pipe& pipe) { return pipe.isOffScreen(); }),
            pipes.end());

        // Collision Detection (simple AABB checks)
        for (const auto& pipe : pipes) {
            if (bird.shape.getGlobalBounds().intersects(pipe.topPipe.getGlobalBounds()) ||
                bird.shape.getGlobalBounds().intersects(pipe.bottomPipe.getGlobalBounds())) {
                window.close(); // Game Over
            }
        }

        // Scoring
        for (auto& pipe : pipes) {
            if (pipe.topPipe.getPosition().x + 50 < bird.shape.getPosition().x && !pipe.isOffScreen()) {
                score++;
            }
        }

        // Update Score Text
        std::ostringstream ss;
        ss << "Score: " << score;
        scoreText.setString(ss.str());

        // Render
        window.clear();
        window.draw(bird.shape);
        for (auto& pipe : pipes)
            pipe.draw(window);
        window.draw(scoreText);
        window.display();
    }

    return 0;
}
