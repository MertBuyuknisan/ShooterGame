#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>

int main()
{
    // Pencere olu�turma
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Shooter");
    window.setFramerateLimit(60);

    // Skor metni olu�turma
    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 10);

    // Oyuncu gemisi
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("resimler/player.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite player(playerTexture);
    player.setPosition(400, 500);

    // Mermi
    sf::Texture bulletTexture;
    if (!bulletTexture.loadFromFile("resimler/bullet.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite bullet(bulletTexture);
    bullet.setScale(0.1f, 0.1f); // Mermiyi k���lt
    bool bulletFired = false;

    // D��man gemileri
    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("resimler/enemy.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite enemy(enemyTexture);
    enemy.setPosition(400, 100);

    // Skor
    int score = 0;
    int speedIncreaseCount = 0; // H�z art��� sayac�
    float enemySpeed = 2.0f; // Ba�lang�� d��man h�z�

    // Oyunun devam edip etmedi�ini kontrol etmek i�in bir bayrak
    bool gameOver = false;

    // Ana oyun d�ng�s�
    while (window.isOpen())
    {
        // Oyun bitti mi kontrol et
        if (gameOver) {
            // Oyun bitti metnini ekrana �iz
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(48);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setString("Oyun Bitti! Skor: " + std::to_string(score));
            gameOverText.setPosition(window.getSize().x / 2 - gameOverText.getGlobalBounds().width / 2, window.getSize().y / 2 - gameOverText.getGlobalBounds().height / 2 - 100);

            // Yeniden ba�latma d��mesini ekrana �iz
            sf::Text restartText;
            restartText.setFont(font);
            restartText.setCharacterSize(36);
            restartText.setFillColor(sf::Color::White);
            restartText.setString("Oyunu Yeniden Oyna");
            restartText.setPosition(window.getSize().x / 2 - restartText.getGlobalBounds().width / 2, window.getSize().y / 2 - restartText.getGlobalBounds().height / 2);

            // Ekrana �iz
            window.clear();
            window.draw(gameOverText);
            window.draw(restartText);
            window.display();

            // Yeniden ba�latma d��mesine t�klan�rsa oyunu s�f�rla
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect bounds = restartText.getGlobalBounds();
                    if (bounds.contains(mousePos.x, mousePos.y)) {
                        gameOver = false;
                        score = 0;
                        scoreText.setString("Skor: " + std::to_string(score)); // Skoru s�f�rla
                        player.setPosition(400, 500);
                        enemy.setPosition(400, 100);
                        speedIncreaseCount = 0; // H�z art��� sayac�n� s�f�rla
                        enemySpeed = 2.0f; // D��man h�z�n� ba�lang�� de�erine geri d�nd�r
                    }
                }
            }
        }
        else {
            // Oyun devam ediyor
            // Olaylar� i�le
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                    bulletFired = true;
                    bullet.setPosition(player.getPosition().x + player.getGlobalBounds().width / 2, player.getPosition().y);
                }
            }

            // Oyuncunun hareketi
            float playerSpeed = 5.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player.getPosition().x > 0) {
                player.move(-playerSpeed, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player.getPosition().x < window.getSize().x - player.getGlobalBounds().width) {
                player.move(playerSpeed, 0);
            }

            // D��man�n hareketi (sadece yukar� ve a�a��)
            enemy.move(0, enemySpeed); // H�zland�r�lm�� hareket

            if (enemy.getPosition().y > window.getSize().y) {
                enemy.setPosition(rand() % (int)(window.getSize().x - enemy.getGlobalBounds().width), 0);
            }

            // Mermiyi hareket ettir
            if (bulletFired) {
                bullet.move(0, -10); // Yukar� do�ru hareket
                if (bullet.getPosition().y < 0) {
                    bulletFired = false;
                }
            }

            // Mermi-d��man �arp��mas� kontrol�
            if (bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                bulletFired = false;
                enemy.setPosition(rand() % (int)(window.getSize().x - enemy.getGlobalBounds().width), 0);
                score++; // Skoru artt�r
                scoreText.setString("Skor: " + std::to_string(score)); // Skoru g�ncelle
                // Skor 60'a ula�t���nda h�z art���n� durdur
                if (score < 50 && score % 10 == 0) {
                    speedIncreaseCount++;
                    enemySpeed += 0.60f; // D��man h�z�n� artt�r
                }
            }

            // D��man-gemi �arp��mas� kontrol�
            if (player.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                gameOver = true;
            }

            // Ekran� temizle ve �izimleri yap
            window.clear();
            window.draw(player);
            window.draw(enemy);
            if (bulletFired) {
                window.draw(bullet);
            }
            window.draw(scoreText); // Skoru ekrana yazd�r
            window.display();
        }
    }

    return 0;
}



