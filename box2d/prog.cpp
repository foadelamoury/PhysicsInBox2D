#include "Planet.h" 
#include "Star.h"

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

            if (
                
                (categoryA == GROUP_STAR && categoryB == GROUP_PLANET) 
                
                ||

                (categoryA == GROUP_PLANET && categoryB == GROUP_STAR)
                ) {
                if (categoryA == GROUP_PLANET)

                    fixtureA->GetBody()->SetAwake(false);
                else
                    fixtureB->GetBody()->SetAwake(false);
            }
        }
    }
}


b2Vec2 calculateGravitationalForce(const b2Vec2& planetPos, const b2Vec2& BiggerMassPos, float planetMass, float otherMass) {
    b2Vec2 direction = BiggerMassPos - planetPos;
    float distance = direction.Length();

    if (distance < 1.0f) distance = 1.0f;

    float forceMagnitude = G * ((otherMass * planetMass) / (distance * distance));

    direction.Normalize(); 
    return forceMagnitude * direction; 
}


void initializeGame(sf::RenderWindow& window, b2World& world, Star& star, std::vector<Planet>& planets) {
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Simple Solar System Simulation");
    window.setFramerateLimit(60);

    world.SetGravity(b2Vec2(0.0f, 0.0f));

    star = Star(world, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 100);

    planets.clear();
}

void handleEvents(sf::RenderWindow& window, b2World& world, Star& star, std::vector<Planet>& planets) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            float mouseX = event.mouseButton.x;
            float mouseY = event.mouseButton.y;

            b2Vec2 starPos(star.position.x / PIXELS_PER_METER, (WINDOW_HEIGHT - star.position.y) / PIXELS_PER_METER);
            b2Vec2 planetPos(mouseX / PIXELS_PER_METER, (WINDOW_HEIGHT - mouseY) / PIXELS_PER_METER);
            b2Vec2 direction = planetPos - starPos;
            float distance = direction.Length();

            direction.Normalize();
            float orbitalSpeed = std::sqrt(G * STAR_MASS / distance);
            b2Vec2 tangentialVelocity;
            if (distance < 6.7f)
            {
                tangentialVelocity= b2Vec2(-direction.y , direction.x );


            }
            else
                tangentialVelocity = b2Vec2(-direction.y * orbitalSpeed, direction.x * orbitalSpeed);
            planets.emplace_back(world, mouseX, mouseY, tangentialVelocity, randomFloat(1.0f, 50.0f), sf::Color(rand() % 256, rand() % 256, rand() % 256));

        }
    }
}

void updatePositions(b2World& world, Star& star, std::vector<Planet>& planets, float deltaTime) {
    for (Planet& planet : planets) {
        b2Vec2 planetPos = planet.body->GetPosition();
        b2Vec2 starPos(star.position.x / PIXELS_PER_METER, (WINDOW_HEIGHT - star.position.y) / PIXELS_PER_METER);

        b2Vec2 totalForce = calculateGravitationalForce(planetPos, starPos, planet.body->GetMass(), STAR_MASS);

        for (Planet& otherPlanet : planets) {
            if (&planet != &otherPlanet && otherPlanet.body->GetMass() > planet.body->GetMass()) {
                b2Vec2 otherPlanetPos = otherPlanet.body->GetPosition();
                b2Vec2 gravitationalForce = calculateGravitationalForce(planetPos, otherPlanetPos, planet.body->GetMass(), otherPlanet.body->GetMass());
       
                totalForce += gravitationalForce;
            }
        }

        planet.body->ApplyForceToCenter(totalForce, true);
    }

    world.Step(deltaTime, 8, 3);

    for (Planet& planet : planets)
        planet.update();

    checkCollisions(world);

    planets.erase(
        std::remove_if(planets.begin(), planets.end(), [&](Planet& planet) {
        if (!planet.body->IsAwake()) {
            world.DestroyBody(planet.body);
            return true;
        }
        return false;
    }),
        planets.end()
    );
}

void render(sf::RenderWindow& window, Star& star, std::vector<Planet>& planets) {
    window.clear();
    window.draw(star.shape);
    for (Planet& planet : planets)
    {
        planet.draw(window);
      

    }
    window.display();
}

int main() {
    sf::RenderWindow window;
    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);

    Star star(world, 0, 0, 0);
    std::vector<Planet> planets;

    initializeGame(window, world, star, planets);

    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        handleEvents(window, world, star, planets);
        updatePositions(world, star, planets, deltaTime);
        render(window, star, planets);
    }

    return 0;
}
