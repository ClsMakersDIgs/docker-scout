#include <SFML/Graphics.hpp>

int main()
{
    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Side Scroller");
    window.setFramerateLimit(60);

    //Player properties
    sf::RectangleShape player(sf::Vector2f(40.0f, 60.0f));
    player.setFillColor(sf::Color::Green);
    player.setPosition(100.0f, 400.0f);

    //Player physics
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float moceSpeed = 5.0f;
    bool isOnGround = false;

    //Ground platform
    sf::RectangleShape ground(sf::Vector2f(800.0f, 50.0f));
    ground.setFillColor(sf::Color(100, 100, 100));
    ground.setPosition(0.0f, 550.0f);

    //Game loop
    while (window.isOpen()
    {
        //Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            window.close();
        }

        //Input handling
        velocityX = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            VelocityX = -moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            velocityX = moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && isOnGround)
        {
            velocityY = jumpStrength;
            isOnGround  = false;
        }

        //Apply gravity
        velocityY += gravity;

        //Update player position
        player.move(velocityX, velocityY);

        //Simple collision with ground
        sf::FloatReact playerBounds = player.getGlobalBounds();
        sf::FloatReact groundBounds = ground.getGlobalBounds();

        if (playerBounds.intersects(groundBounds) && velocityY > 0)
        {
            // Place player on top of ground
            player.setPosition(player.getPosition().x,
                            ground.getPosition().y - playerBounds.height);
            velocityY = 0.0f;
            isOnGround = true;                
        }   

        //  Keep player in screen bounds(left/right)
        if (player.getPosition().x < 0)
            player.setPosition(0, player.getPosition().y);
        if (player.getPosition().x + playerBounds.width > 800)
            player.setPosition(800 - playerBounds.width, player.getPosition().y);

        // Render
        window.clear(sf::Color(135, 206, 235)); // Sky blue background
        window.draw(ground);
        window.draw(player);
        window.display();        
    }
    //  Your setup code
    while (windows.isOpen()) {
        // 1. Handle events (close window, keyboard input)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // 2. Update game logic (apply physics, move player)
        // Apply gravity to vertical velocity
        velcoityY += gravity;
        // Handle left/right movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocityX = -moceSpeed;   // Move right
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocityX = moceSpeed;
            
        else {
            velocityX = 0.0f; // Stop horizon movement
        }

        // Handle jumping (only if on ground)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            velocityY = jumpStrength;
        }

        // Update player position based on velocity
        sf::Vector2f currentPos = player.getPosition();
        player.setPosition(currentPos.x + velocityX, currentPos.y + velocityY);

        // Simple ground collsion 9keep player above y = 500)
        if (player.getPosition().y > 500.0f) {
            player.setPosition(player.getPosition().x, 500.0f);
            velocityY = 0.0f;
        }

        // 3. Draw everything
        window.clear();
        window.draw(player);
        window.display();
    }

    return 0;
}