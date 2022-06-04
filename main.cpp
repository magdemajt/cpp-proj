#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <random>
#include <iostream>

#define BUSINESSES 6
#define HARDMODE false
#define MAX_PRICE 10000000000
#define INITIAL_MONEY 100

// Prices are in cents

struct Business {
    std::string name;
    long long basePrice;
    long long baseIncome;
    long long price;

    Business(std::string name, long long basePrice, long long baseIncome) {
        this->name = std::move(name);
        this->basePrice = basePrice;
        this->baseIncome = baseIncome;
        this->price = basePrice;
    }

    long long min(long long  i, long long i1) {
        return i < i1 ? i : i1;
    }

    void updatePrice() {
        if (HARDMODE) {
            price = min(price * (110) / 100, MAX_PRICE);
            return;
        }
        price += (baseIncome / 10);
    }

};

bool randomBankruptcy(std::uniform_int_distribution<> &dist, std::mt19937 &gen, std::map<std::string, int> &ownedBusinesses) {
    if (dist(gen) == 5) {
        std::vector<std::string> keys;
        for (auto &kv : ownedBusinesses) {
            kv.second = 0;
        }
        return true;
    }
    return false;
}

void buyBusiness(Business *business, long long *money, std::map<std::string, int> *ownedBusinesses) {
    if (*money >= business->price) {
        *money -= business->price;
        (*ownedBusinesses).find(business->name)->second++;
        business->updatePrice();
    }
}

bool isGameOver(std::map<std::string, int> &ownedBusinesses, long long money, std::vector<Business> &businesses) {
    // game over is when you have  no businesses and cannot afford to buy any

    for (auto &kv : ownedBusinesses) {
        if (kv.second > 0) {
            return false;
        }
    }

    for (auto &business : businesses) {
        if (money >= business.price) {
            return false;
        }
    }
    return true;
}

void restartGame(std::map<std::string, int> &ownedBusinesses, long long *money, std::vector<Business> &businesses) {
    for (auto &kv : ownedBusinesses) {
        kv.second = 0;
    }
    *money = INITIAL_MONEY;
    for (auto &business : businesses) {
        business.price = business.basePrice;
    }
}

int main() {
    constexpr int frameRate = 60;
    constexpr int paymentAfterFrames = 60 * 2;
    float paymentProgress = 0;
    long long personalBalance = INITIAL_MONEY;
    int selectedBusiness = 0;
    std::vector<Business> businesses;
    constexpr int displayBankruptFor = 60 * 5;
    int bankruptDisplayedFor = 0;

    businesses.emplace_back( "Farm", 100, 10);
    businesses.emplace_back("Mine", 1000, 100);
    businesses.emplace_back("Factory", 10000, 1000);
    businesses.emplace_back("Bank", 100000, 10000);
    businesses.emplace_back("Casino", 1000000, 100000);
    businesses.emplace_back("Port", 10000000, 1000000);

    std::map <std::string, int> ownedBusinesses;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 12000); // Every 2 seconds there is 1/100 chances of a random bankruptcy

    for (auto & business : businesses) {
        ownedBusinesses.insert(std::make_pair(business.name, 0));
    }



    sf::RenderWindow window(sf::VideoMode(800, 600), "Greedy Snake");
    sf::Clock clock;

    bool gameOver = false;

    // load image
//    sf::Texture lemonadeStand;
//    if (!lemonadeStand.loadFromFile("./images/lemonade-stand.jpg")) {
//        return EXIT_FAILURE;
//    }

    // display image
//    sf::Sprite sprite(lemonadeStand);

    // display names of businesses as text on the screen
    // selected business should be framed in blue frame

    sf::Font font;
    if (!font.loadFromFile("./fonts/Lato-Regular.ttf")) {
        return EXIT_FAILURE;
    }

    window.setFramerateLimit(frameRate);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyReleased) {
                // which key
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::Left) {
                    selectedBusiness = (selectedBusiness + businesses.size() - 1) % businesses.size();

                } else if (event.key.code == sf::Keyboard::Right) {
                    selectedBusiness = (selectedBusiness + 1) % businesses.size();
                } else if (event.key.code == sf::Keyboard::Space) {
                    // buy business
                    buyBusiness(&businesses[selectedBusiness], &personalBalance, &ownedBusinesses);
                } else if (event.key.code == sf::Keyboard::R && gameOver) {
                    gameOver = false;
                    restartGame(ownedBusinesses, &personalBalance, businesses);
                    bankruptDisplayedFor = 0;
                }
            }
        }

        window.clear();

        if (!gameOver) {
            for (int i = 0; i < businesses.size(); i++) {
                auto & business = businesses[i];
                sf::Text text(business.name, font, 20);
                text.setPosition(sf::Vector2f(10, 10 + i * 30));
                // display price on the right side
                sf::Text priceText(std::to_string(business.price) + "$", font, 20);
                // if price is too high, display it in red else in green
                if (business.price > personalBalance) {
                    priceText.setColor(sf::Color::Red);
                } else {
                    priceText.setColor(sf::Color::Green);
                }
                priceText.setPosition(sf::Vector2f(text.getPosition().x + text.getGlobalBounds().width + 20, text.getPosition().y));

                // display number of owned businesses on the right side of business
                sf::Text ownedText("x" + std::to_string(ownedBusinesses.find(business.name)->second), font, 20);
                ownedText.setPosition(sf::Vector2f(priceText.getPosition().x + priceText.getGlobalBounds().width + 20, text.getPosition().y));

                if (i == selectedBusiness) {
                    sf::RectangleShape frame(sf::Vector2f(text.getLocalBounds().width + 10, text.getLocalBounds().height + 10));
                    frame.setPosition(text.getPosition() - sf::Vector2f(5, 5));
                    frame.setFillColor(sf::Color::Blue);
                    window.draw(frame);
                }
                window.draw(text);
                window.draw(priceText);
                window.draw(ownedText);
            }

            sf::Text text("Balance: " + std::to_string(personalBalance) + '$', font, 20);
            text.setPosition(sf::Vector2f(10, 10 + businesses.size() * 30));
            window.draw(text);

            bool isBankrupt = randomBankruptcy(distrib, gen, ownedBusinesses);

            if (isBankrupt) {
                bankruptDisplayedFor = displayBankruptFor;
            }

            if (bankruptDisplayedFor > 0) {
                bankruptDisplayedFor--;
                sf::Text bankruptText("Your companies are bankrupt!", font, 32);
                bankruptText.setPosition(sf::Vector2f(window.getSize().x / 2 - bankruptText.getGlobalBounds().width / 2,
                                                      window.getSize().y / 2 - bankruptText.getGlobalBounds().height / 2));
                bankruptText.setFillColor(sf::Color::Red);
                window.draw(bankruptText);
            }




            float frameTime = 1.0f / clock.getElapsedTime().asSeconds();

            paymentProgress += frameTime / 60.0f;

            clock.restart();

            // draw progress bar

            sf::RectangleShape progressBar(sf::Vector2f(200, 20));
            progressBar.setPosition(sf::Vector2f(10, 10 + businesses.size() * 30 + 30));
            progressBar.setFillColor(sf::Color::White);
            window.draw(progressBar);

            sf::RectangleShape progressBar2(sf::Vector2f(((paymentProgress / paymentAfterFrames) * 200), 20));

            progressBar2.setPosition(sf::Vector2f(10, 10 + businesses.size() * 30 + 30));
            progressBar2.setFillColor(sf::Color::Green);
            window.draw(progressBar2);

            window.display();


            if (paymentProgress > paymentAfterFrames) {
                paymentProgress = 0;
                for (auto & business : businesses) {
                    // get the number of businesses owned
                    int owned = ownedBusinesses[business.name];
                    personalBalance += owned * business.baseIncome;
                }
            }

            gameOver = isGameOver(ownedBusinesses, personalBalance, businesses);
        } else {
            sf::Text text("You are bankrupt!", font, 32);
            text.setPosition(sf::Vector2f(window.getSize().x / 2 - text.getGlobalBounds().width / 2,
                                          window.getSize().y / 2 - text.getGlobalBounds().height / 2));
            text.setFillColor(sf::Color::Red);
            window.draw(text);
            window.display();
            sf::sleep(sf::seconds(5));
        }
    }


    return 0;
}
