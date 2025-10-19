#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

int main()
{
    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Enhanced Glyph Magic Game");
    window.setFramerateLimit(60);

    // Player properties
    sf::RectangleShape player(sf::Vector2f(40.0f, 60.0f));
    player.setFillColor(sf::Color::Green);
    player.setPosition(100.0f, 400.0f);

    // Player stats
    int playerHealth = 5;
    int playerMaxHealth = 5;
    int score = 0;
    bool isInvulnerable = false;
    int invulnerabilityTimer = 0;
    int invulnerabilityDuration = 60; // 1 second

    // Player physics
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float moveSpeed = 5.0f;
    bool isOnGround = false;

    // Power-ups
    bool hasShield = false;
    int shieldTimer = 0;
    int shieldDuration = 300; // 5 seconds
    bool hasSpeedBoost = false;
    int speedBoostTimer = 0;
    int speedBoostDuration = 180; // 3 seconds
    bool hasRapidFire = false;
    int rapidFireTimer = 0;
    int rapidFireDuration = 240; // 4 seconds

    // Ground platform
    sf::RectangleShape ground(sf::Vector2f(800.0f, 50.0f));
    ground.setFillColor(sf::Color(100, 100, 100));
    ground.setPosition(0.0f, 550.0f);

    // Additional platforms
    std::vector<sf::RectangleShape> platforms;

    sf::RectangleShape plat1(sf::Vector2f(150.0f, 20.0f));
    plat1.setFillColor(sf::Color(120, 120, 120));
    plat1.setPosition(200.0f, 450.0f);
    platforms.push_back(plat1);

    sf::RectangleShape plat2(sf::Vector2f(150.0f, 20.0f));
    plat2.setFillColor(sf::Color(120, 120, 120));
    plat2.setPosition(450.0f, 350.0f);
    platforms.push_back(plat2);

    sf::RectangleShape plat3(sf::Vector2f(100.0f, 20.0f));
    plat3.setFillColor(sf::Color(120, 120, 120));
    plat3.setPosition(650.0f, 450.0f);
    platforms.push_back(plat3);

    // Rotating Glyph
    sf::CircleShape glyph(20.0f, 6);
    glyph.setFillColor(sf::Color(150, 0, 255, 200));
    glyph.setOutlineThickness(2.0f);
    glyph.setOutlineColor(sf::Color(255, 255, 0));
    glyph.setOrigin(20.0f, 20.0f);
    glyph.setPosition(400.0f, 520.0f);
    float glyphRotation = 0.0f;
    bool glyphExists = true;
    bool playerHasGlyph = false;

    // Power-up pickup
    sf::CircleShape powerUp(15.0f);
    powerUp.setFillColor(sf::Color(0, 255, 255, 200));
    powerUp.setOutlineThickness(2.0f);
    powerUp.setOutlineColor(sf::Color::White);
    bool powerUpExists = false;
    int powerUpType = 0; // 0=shield, 1=speed, 2=rapidfire
    int powerUpSpawnTimer = 0;

    // Fireball properties
    sf::CircleShape fireball(15.0f);
    fireball.setFillColor(sf::Color(255, 100, 0));
    fireball.setOutlineThickness(3.0f);
    fireball.setOutlineColor(sf::Color(255, 255, 0));
    float fireballSpeed = 8.0f;
    bool fireballActive = false;
    fireball.setPosition(-100.0f, 50.0f);

    sf::CircleShape fireballTail(10.0f);
    fireballTail.setFillColor(sf::Color(255, 200, 0, 150));

    // Enemy types
    enum EnemyType { NORMAL, FAST, TANK, FLYING, BOSS };

    struct Enemy {
        sf::RectangleShape shape;
        EnemyType type;
        int health;
        float speed;
        bool active;
        float flyY;
        float flyDirection;
    };

    std::vector<Enemy> enemies;

    // Wave system
    int currentWave = 1;
    int enemiesSpawnedThisWave = 0;
    int enemiesPerWave = 3;
    int waveTimer = 0;

    // Animation system
    enum EffectType { NONE, LIGHTNING, FIRE, ICE };
    EffectType currentEffect = NONE;
    int effectTimer = 0;
    int effectDuration = 60;

    // Lightning effect
    sf::RectangleShape lightning(sf::Vector2f(10.0f, 600.0f));
    lightning.setFillColor(sf::Color(255, 255, 100));
    lightning.setPosition(400.0f, 0.0f);

    // Fire effect
    sf::CircleShape flames[5];
    float flameY[5];
    for (int i = 0; i < 5; i++) {
        flames[i].setRadius(30.0f);
        flames[i].setFillColor(sf::Color(255, 100 + i * 30, 0, 200));
        flames[i].setPosition(150.0f + i * 150.0f, 300.0f);
        flameY[i] = 0.0f;
    }

    // Ice effect
    sf::RectangleShape iceShards[8];
    float iceRotation[8];
    for (int i = 0; i < 8; i++) {
        iceShards[i].setSize(sf::Vector2f(40.0f, 10.0f));
        iceShards[i].setFillColor(sf::Color(100, 200, 255));
        iceShards[i].setOrigin(20.0f, 5.0f);
        iceShards[i].setPosition(400.0f, 300.0f);
        iceRotation[i] = i * 45.0f;
    }

    // Dagger swipe properties
    std::vector<sf::RectangleShape> daggers;
    std::vector<float> daggerRotations;
    std::vector<int> daggerLifetimes;
    int daggerCooldown = 0;
    int daggerMaxLifetime = 30;

    // Particle system
    struct Particle {
        sf::CircleShape shape;
        float vx, vy;
        int lifetime;
    };
    std::vector<Particle> particles;

    // Health hearts UI
    sf::CircleShape heart(10.0f, 3);
    heart.setFillColor(sf::Color::Red);
    heart.setOutlineThickness(1.0f);
    heart.setOutlineColor(sf::Color(150, 0, 0));
    heart.setRotation(180.0f);

    // Shield visual
    sf::CircleShape shieldCircle(35.0f);
    shieldCircle.setFillColor(sf::Color(0, 200, 255, 50));
    shieldCircle.setOutlineThickness(3.0f);
    shieldCircle.setOutlineColor(sf::Color(0, 255, 255));
    shieldCircle.setOrigin(35.0f, 35.0f);

    // Game loop
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                // Use glyph with E key (wrong usage)
                if (event.key.code == sf::Keyboard::E && playerHasGlyph)
                {
                    fireballActive = true;
                    fireball.setPosition(-50.0f, 50.0f + std::rand() % 150);
                    playerHasGlyph = false;
                }

                // Use glyph with F key (correct usage)
                if (event.key.code == sf::Keyboard::F && playerHasGlyph && daggerCooldown <= 0)
                {
                    sf::RectangleShape newDagger(sf::Vector2f(80.0f, 15.0f));
                    newDagger.setFillColor(sf::Color(200, 200, 200));
                    newDagger.setOutlineThickness(2.0f);
                    newDagger.setOutlineColor(sf::Color(255, 255, 255));
                    newDagger.setOrigin(40.0f, 7.5f);
                    newDagger.setPosition(player.getPosition().x + 20.0f,
                        player.getPosition().y + 30.0f);

                    daggers.push_back(newDagger);
                    daggerRotations.push_back(0.0f);
                    daggerLifetimes.push_back(daggerMaxLifetime);

                    if (!hasRapidFire) {
                        playerHasGlyph = false;
                        daggerCooldown = 15;
                    }
                    else {
                        daggerCooldown = 5; // Faster with rapid fire
                    }
                }
            }
        }

        // Input handling
        velocityX = 0.0f;
        float currentMoveSpeed = hasSpeedBoost ? moveSpeed * 1.5f : moveSpeed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            velocityX = -currentMoveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            velocityX = currentMoveSpeed;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && isOnGround)
        {
            velocityY = jumpStrength;
            isOnGround = false;
        }

        // Apply gravity
        velocityY += gravity;

        // Update player position
        player.move(velocityX, velocityY);

        // Collision with ground
        sf::FloatRect playerBounds = player.getGlobalBounds();
        sf::FloatRect groundBounds = ground.getGlobalBounds();

        if (playerBounds.intersects(groundBounds) && velocityY > 0)
        {
            player.setPosition(player.getPosition().x,
                ground.getPosition().y - playerBounds.height);
            velocityY = 0.0f;
            isOnGround = true;
        }

        // Collision with platforms
        for (auto& platform : platforms)
        {
            sf::FloatRect platBounds = platform.getGlobalBounds();
            if (playerBounds.intersects(platBounds) && velocityY > 0)
            {
                float playerBottom = player.getPosition().y + playerBounds.height;
                float platformTop = platform.getPosition().y;

                if (playerBottom - velocityY <= platformTop + 5)
                {
                    player.setPosition(player.getPosition().x, platformTop - playerBounds.height);
                    velocityY = 0.0f;
                    isOnGround = true;
                }
            }
        }

        // Keep player in bounds
        if (player.getPosition().x < 0)
            player.setPosition(0, player.getPosition().y);
        if (player.getPosition().x + playerBounds.width > 800)
            player.setPosition(800 - playerBounds.width, player.getPosition().y);

        // Update timers
        if (daggerCooldown > 0) daggerCooldown--;
        if (invulnerabilityTimer > 0) invulnerabilityTimer--;
        else isInvulnerable = false;

        if (shieldTimer > 0) {
            shieldTimer--;
            if (shieldTimer == 0) hasShield = false;
        }
        if (speedBoostTimer > 0) {
            speedBoostTimer--;
            if (speedBoostTimer == 0) hasSpeedBoost = false;
        }
        if (rapidFireTimer > 0) {
            rapidFireTimer--;
            if (rapidFireTimer == 0) hasRapidFire = false;
        }

        // Glyph rotation
        if (glyphExists)
        {
            glyphRotation += 2.0f;
            glyph.setRotation(glyphRotation);

            if (playerBounds.intersects(glyph.getGlobalBounds()))
            {
                glyphExists = false;
                playerHasGlyph = true;
            }
        }

        // Power-up spawning
        powerUpSpawnTimer++;
        if (!powerUpExists && powerUpSpawnTimer > 600) // Every 10 seconds
        {
            powerUpExists = true;
            powerUpType = std::rand() % 3;
            int platChoice = std::rand() % platforms.size();
            powerUp.setPosition(platforms[platChoice].getPosition().x + 50.0f,
                platforms[platChoice].getPosition().y - 30.0f);
            powerUpSpawnTimer = 0;

            // Set color based on type
            if (powerUpType == 0) powerUp.setFillColor(sf::Color(0, 200, 255, 200)); // Shield - cyan
            else if (powerUpType == 1) powerUp.setFillColor(sf::Color(255, 255, 0, 200)); // Speed - yellow
            else powerUp.setFillColor(sf::Color(255, 0, 255, 200)); // Rapid fire - magenta
        }

        // Power-up pickup
        if (powerUpExists && playerBounds.intersects(powerUp.getGlobalBounds()))
        {
            powerUpExists = false;
            if (powerUpType == 0) {
                hasShield = true;
                shieldTimer = shieldDuration;
            }
            else if (powerUpType == 1) {
                hasSpeedBoost = true;
                speedBoostTimer = speedBoostDuration;
            }
            else {
                hasRapidFire = true;
                rapidFireTimer = rapidFireDuration;
            }
        }

        // Update fireball
        if (fireballActive)
        {
            fireball.move(fireballSpeed, 0);
            fireballTail.setPosition(fireball.getPosition().x - 20.0f,
                fireball.getPosition().y + 5.0f);

            if (fireball.getPosition().x > 400 && fireball.getPosition().x < 420)
            {
                int choice = std::rand() % 100;

                if (choice < 50) // 50% spawn enemy
                {
                    Enemy newEnemy;
                    newEnemy.active = true;

                    int enemyTypeRoll = std::rand() % 100;
                    if (currentWave >= 5 && enemyTypeRoll < 10) {
                        // Boss (10% after wave 5)
                        newEnemy.type = BOSS;
                        newEnemy.shape = sf::RectangleShape(sf::Vector2f(80.0f, 80.0f));
                        newEnemy.shape.setFillColor(sf::Color(100, 0, 0));
                        newEnemy.health = 5;
                        newEnemy.speed = 1.0f;
                    }
                    else if (enemyTypeRoll < 40) {
                        // Fast enemy
                        newEnemy.type = FAST;
                        newEnemy.shape = sf::RectangleShape(sf::Vector2f(30.0f, 40.0f));
                        newEnemy.shape.setFillColor(sf::Color(255, 100, 0));
                        newEnemy.health = 1;
                        newEnemy.speed = 4.0f;
                    }
                    else if (enemyTypeRoll < 60) {
                        // Tank enemy
                        newEnemy.type = TANK;
                        newEnemy.shape = sf::RectangleShape(sf::Vector2f(60.0f, 60.0f));
                        newEnemy.shape.setFillColor(sf::Color(150, 0, 0));
                        newEnemy.health = 3;
                        newEnemy.speed = 1.5f;
                    }
                    else if (enemyTypeRoll < 80) {
                        // Flying enemy
                        newEnemy.type = FLYING;
                        newEnemy.shape = sf::RectangleShape(sf::Vector2f(40.0f, 30.0f));
                        newEnemy.shape.setFillColor(sf::Color(200, 0, 200));
                        newEnemy.health = 1;
                        newEnemy.speed = 2.5f;
                        newEnemy.flyY = 0.0f;
                        newEnemy.flyDirection = 1.0f;
                    }
                    else {
                        // Normal enemy
                        newEnemy.type = NORMAL;
                        newEnemy.shape = sf::RectangleShape(sf::Vector2f(50.0f, 50.0f));
                        newEnemy.shape.setFillColor(sf::Color::Red);
                        newEnemy.health = 1;
                        newEnemy.speed = 2.0f;
                    }

                    newEnemy.shape.setPosition(700.0f, 500.0f);
                    enemies.push_back(newEnemy);
                }
                else // 50% elemental effect
                {
                    int effectChoice = std::rand() % 3;
                    if (effectChoice == 0) currentEffect = LIGHTNING;
                    else if (effectChoice == 1) currentEffect = FIRE;
                    else currentEffect = ICE;

                    effectTimer = effectDuration;
                }
            }

            if (fireball.getPosition().x > 850)
            {
                fireballActive = false;
                glyphExists = true;
                glyph.setPosition(200.0f + std::rand() % 400, 520.0f);
            }
        }

        // Update enemies
        for (size_t i = 0; i < enemies.size(); i++)
        {
            if (!enemies[i].active) continue;

            if (enemies[i].type == FLYING)
            {
                enemies[i].flyY += enemies[i].flyDirection * 2.0f;
                if (enemies[i].flyY > 100 || enemies[i].flyY < -100)
                    enemies[i].flyDirection *= -1;

                enemies[i].shape.move(-enemies[i].speed, enemies[i].flyDirection * 2.0f);
            }
            else
            {
                enemies[i].shape.move(-enemies[i].speed, 0);
            }

            // Check collision with player
            if (!isInvulnerable && !hasShield &&
                playerBounds.intersects(enemies[i].shape.getGlobalBounds()))
            {
                playerHealth--;
                isInvulnerable = true;
                invulnerabilityTimer = invulnerabilityDuration;

                // Create damage particles
                for (int p = 0; p < 10; p++)
                {
                    Particle part;
                    part.shape = sf::CircleShape(3.0f);
                    part.shape.setFillColor(sf::Color::Red);
                    part.shape.setPosition(player.getPosition().x + 20.0f,
                        player.getPosition().y + 30.0f);
                    part.vx = (std::rand() % 200 - 100) / 20.0f;
                    part.vy = (std::rand() % 200 - 100) / 20.0f;
                    part.lifetime = 30;
                    particles.push_back(part);
                }

                if (playerHealth <= 0)
                {
                    // Game over - reset
                    playerHealth = playerMaxHealth;
                    score = 0;
                    currentWave = 1;
                    enemies.clear();
                    player.setPosition(100.0f, 400.0f);
                }
            }

            // Check collision with daggers
            for (size_t d = 0; d < daggers.size(); d++)
            {
                if (enemies[i].shape.getGlobalBounds().intersects(daggers[d].getGlobalBounds()))
                {
                    enemies[i].health--;

                    // Create hit particles
                    for (int p = 0; p < 5; p++)
                    {
                        Particle part;
                        part.shape = sf::CircleShape(2.0f);
                        part.shape.setFillColor(sf::Color::Yellow);
                        part.shape.setPosition(daggers[d].getPosition());
                        part.vx = (std::rand() % 200 - 100) / 20.0f;
                        part.vy = (std::rand() % 200 - 100) / 20.0f;
                        part.lifetime = 20;
                        particles.push_back(part);
                    }

                    if (enemies[i].health <= 0)
                    {
                        enemies[i].active = false;
                        score += (enemies[i].type == BOSS ? 50 :
                            enemies[i].type == TANK ? 20 :
                            enemies[i].type == FAST ? 15 : 10);

                        // Spawn glyph reward
                        if (std::rand() % 100 < 70) // 70% chance
                        {
                            glyphExists = true;
                            glyph.setPosition(enemies[i].shape.getPosition().x,
                                enemies[i].shape.getPosition().y);
                        }

                        // Death particles
                        for (int p = 0; p < 15; p++)
                        {
                            Particle part;
                            part.shape = sf::CircleShape(4.0f);
                            part.shape.setFillColor(enemies[i].shape.getFillColor());
                            part.shape.setPosition(enemies[i].shape.getPosition());
                            part.vx = (std::rand() % 400 - 200) / 20.0f;
                            part.vy = (std::rand() % 400 - 200) / 20.0f;
                            part.lifetime = 40;
                            particles.push_back(part);
                        }
                    }
                    break;
                }
            }

            // Remove if off screen
            if (enemies[i].shape.getPosition().x < -100)
            {
                enemies[i].active = false;
            }
        }

        // Clean up inactive enemies
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.active; }), enemies.end());

        // Update daggers
        for (size_t i = 0; i < daggers.size(); i++)
        {
            daggerLifetimes[i]--;
            daggerRotations[i] += 25.0f;
            daggers[i].setRotation(daggerRotations[i]);

            float radians = daggerRotations[i] * 3.14159f / 180.0f;
            daggers[i].move(15.0f * std::cos(radians),
                15.0f * std::sin(radians) * 0.3f);
        }

        // Remove expired daggers
        for (int i = daggerLifetimes.size() - 1; i >= 0; i--)
        {
            if (daggerLifetimes[i] <= 0)
            {
                daggers.erase(daggers.begin() + i);
                daggerRotations.erase(daggerRotations.begin() + i);
                daggerLifetimes.erase(daggerLifetimes.begin() + i);
            }
        }

        // Update particles
        for (auto& p : particles)
        {
            p.lifetime--;
            p.shape.move(p.vx, p.vy);
            p.vy += 0.2f; // Gravity on particles
        }
        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.lifetime <= 0; }), particles.end());

        // Update effects
        if (effectTimer > 0)
        {
            effectTimer--;

            if (currentEffect == FIRE)
            {
                for (int i = 0; i < 5; i++)
                {
                    flameY[i] -= 2.0f;
                    flames[i].setPosition(150.0f + i * 150.0f, 500.0f + flameY[i]);
                }
            }
            else if (currentEffect == ICE)
            {
                for (int i = 0; i < 8; i++)
                {
                    iceRotation[i] += 5.0f;
                    float angle = iceRotation[i] * 3.14159f / 180.0f;
                    float radius = 100.0f + (effectDuration - effectTimer) * 2.0f;
                    iceShards[i].setPosition(
                        400.0f + std::cos(angle) * radius,
                        300.0f + std::sin(angle) * radius
                    );
                    iceShards[i].setRotation(iceRotation[i]);
                }
            }
        }
        else
        {
            currentEffect = NONE;
            for (int i = 0; i < 5; i++)
                flameY[i] = 0.0f;
        }

        // Wave system
        waveTimer++;
        if (enemies.empty() && waveTimer > 120) // 2 second delay between waves
        {
            currentWave++;
            enemiesPerWave = 2 + currentWave;
            waveTimer = 0;
        }

        // Render
        window.clear(sf::Color(135, 206, 235));

        window.draw(ground);

        for (auto& plat : platforms)
            window.draw(plat);

        if (glyphExists)
            window.draw(glyph);

        if (powerUpExists)
            window.draw(powerUp);

        if (fireballActive)
        {
            window.draw(fireballTail);
            window.draw(fireball);
        }

        // Draw effects
        if (currentEffect == LIGHTNING && effectTimer > 0)
        {
            if ((effectTimer / 5) % 2 == 0)
            {
                lightning.setPosition(300.0f + std::rand() % 200, 0.0f);
                window.draw(lightning);
            }
        }
        else if (currentEffect == FIRE && effectTimer > 0)
        {
            for (int i = 0; i < 5; i++)
                window.draw(flames[i]);
        }
        else if (currentEffect == ICE && effectTimer > 0)
        {
            for (int i = 0; i < 8; i++)
                window.draw(iceShards[i]);
        }

        for (auto& enemy : enemies)
            window.draw(enemy.shape);

        for (auto& dagger : daggers)
            window.draw(dagger);

        for (auto& p : particles)
            window.draw(p.shape);

        // Draw player with invulnerability flash
        if (!isInvulnerable || (invulnerabilityTimer / 5) % 2 == 0)
            window.draw(player);

        // Draw shield
        if (hasShield)
        {
            shieldCircle.setPosition(player.getPosition().x + 20.0f,
                player.getPosition().y + 30.0f);
            window.draw(shieldCircle);
        }

        // Draw UI - Health hearts
        for (int i = 0; i < playerHealth; i++)
        {
            sf::CircleShape h = heart;
            h.setPosition(10.0f + i * 25.0f, 10.0f);
            window.draw(h);
        }

        window.display();
    }

    return 0;
}
