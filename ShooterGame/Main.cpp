#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>

int main()
{
    // Pencere oluþturma
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Shooter");
    window.setFramerateLimit(60);

    // Skor metni oluþturma
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
    bullet.setScale(0.1f, 0.1f); // Mermiyi küçült
    bool bulletFired = false;

    // Düþman gemileri
    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("resimler/enemy.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite enemy(enemyTexture);
    enemy.setPosition(400, 100);

    // Skor
    int score = 0;
    int speedIncreaseCount = 0; // Hýz artýþý sayacý
    float enemySpeed = 2.0f; // Baþlangýç düþman hýzý

    // Oyunun devam edip etmediðini kontrol etmek için bir bayrak
    bool gameOver = false;

    // Ana oyun döngüsü
    while (window.isOpen())
    {
        // Oyun bitti mi kontrol et
        if (gameOver) {
            // Oyun bitti metnini ekrana çiz
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(48);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setString("Oyun Bitti! Skor: " + std::to_string(score));
            gameOverText.setPosition(window.getSize().x / 2 - gameOverText.getGlobalBounds().width / 2, window.getSize().y / 2 - gameOverText.getGlobalBounds().height / 2 - 100);

            // Yeniden baþlatma düðmesini ekrana çiz
            sf::Text restartText;
            restartText.setFont(font);
            restartText.setCharacterSize(36);
            restartText.setFillColor(sf::Color::White);
            restartText.setString("Oyunu Yeniden Oyna");
            restartText.setPosition(window.getSize().x / 2 - restartText.getGlobalBounds().width / 2, window.getSize().y / 2 - restartText.getGlobalBounds().height / 2);

            // Ekrana çiz
            window.clear();
            window.draw(gameOverText);
            window.draw(restartText);
            window.display();

            // Yeniden baþlatma düðmesine týklanýrsa oyunu sýfýrla
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
                        scoreText.setString("Skor: " + std::to_string(score)); // Skoru sýfýrla
                        player.setPosition(400, 500);
                        enemy.setPosition(400, 100);
                        speedIncreaseCount = 0; // Hýz artýþý sayacýný sýfýrla
                        enemySpeed = 2.0f; // Düþman hýzýný baþlangýç deðerine geri döndür
                    }
                }
            }
        }
        else {
            // Oyun devam ediyor
            // Olaylarý iþle
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

            // Düþmanýn hareketi (sadece yukarý ve aþaðý)
            enemy.move(0, enemySpeed); // Hýzlandýrýlmýþ hareket

            if (enemy.getPosition().y > window.getSize().y) {
                enemy.setPosition(rand() % (int)(window.getSize().x - enemy.getGlobalBounds().width), 0);
            }

            // Mermiyi hareket ettir
            if (bulletFired) {
                bullet.move(0, -10); // Yukarý doðru hareket
                if (bullet.getPosition().y < 0) {
                    bulletFired = false;
                }
            }

            // Mermi-düþman çarpýþmasý kontrolü
            if (bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                bulletFired = false;
                enemy.setPosition(rand() % (int)(window.getSize().x - enemy.getGlobalBounds().width), 0);
                score++; // Skoru arttýr
                scoreText.setString("Skor: " + std::to_string(score)); // Skoru güncelle
                // Skor 60'a ulaþtýðýnda hýz artýþýný durdur
                if (score < 50 && score % 10 == 0) {
                    speedIncreaseCount++;
                    enemySpeed += 0.60f; // Düþman hýzýný arttýr
                }
            }

            // Düþman-gemi çarpýþmasý kontrolü
            if (player.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                gameOver = true;
            }

            // Ekraný temizle ve çizimleri yap
            window.clear();
            window.draw(player);
            window.draw(enemy);
            if (bulletFired) {
                window.draw(bullet);
            }
            window.draw(scoreText); // Skoru ekrana yazdýr
            window.display();
        }
    }

    return 0;
}



