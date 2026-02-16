#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <random> 
#include <ctime>
#include <algorithm>

using namespace std;

vector<string> loadDictionary(string filename) {
    vector<string> words;
    ifstream file(filename);
    if (file.is_open()) {
        string word;
        while (file >> word) {
            if (word.length() == 5) {
                for (auto &c : word) c = toupper(c);
                words.push_back(word);
            }
        }
        file.close();
    }
    return words;
}

string getRandomWord(const vector<string>& words) {
    if (words.empty()) return "SUPER";
    random_device rd; 
    mt19937 gen(rd()); 
    uniform_int_distribution<> dis(0, (int)words.size() - 1);
    return words[dis(gen)];
}

int main() {
    sf::RenderWindow window(sf::VideoMode(450, 750), "C++ Wordle Game");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("ariblk.ttf")) return -1;

    vector<string> dictionary = loadDictionary("word.txt");
    string targetWord = getRandomWord(dictionary);

    string currentGuess = "";
    vector<string> guesses;
    int currentRow = 0;
    bool gameOver = false;
    bool invalidWord = false;
    
    int menuState = 0; 
    bool challengeMode = false;
    float timeLimit = 0;
    sf::Clock gameClock;

    sf::RectangleShape btnPlay(sf::Vector2f(200, 60)), btnChallenge(sf::Vector2f(200, 60));
    btnPlay.setPosition(125, 200); btnPlay.setFillColor(sf::Color(46, 204, 113));
    btnChallenge.setPosition(125, 300); btnChallenge.setFillColor(sf::Color(231, 76, 60));

    sf::RectangleShape btn60(sf::Vector2f(100, 50)), btn90(sf::Vector2f(100, 50)), btn120(sf::Vector2f(100, 50));
    btn60.setPosition(175, 250); btn60.setFillColor(sf::Color(52, 152, 219));
    btn90.setPosition(175, 330); btn90.setFillColor(sf::Color(52, 152, 219));
    btn120.setPosition(175, 410); btn120.setFillColor(sf::Color(52, 152, 219));

    vector<sf::RectangleShape> boxes;
    for (int i = 0; i < 30; i++) {
        sf::RectangleShape box(sf::Vector2f(60.f, 60.f));
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(200, 200, 200));
        // ขยับจุดเริ่มตารางขึ้นไปที่พิกัด y = 90
        box.setPosition(50.f + (i % 5) * 70, 90.f + (i / 5) * 70);
        boxes.push_back(box);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                if (menuState == 0) {
                    if (btnPlay.getGlobalBounds().contains(mousePos)) {
                        challengeMode = false; menuState = 2; gameClock.restart();
                    }
                    if (btnChallenge.getGlobalBounds().contains(mousePos)) {
                        menuState = 1;
                    }
                } else if (menuState == 1) {
                    if (btn60.getGlobalBounds().contains(mousePos)) { timeLimit = 60; challengeMode = true; menuState = 2; gameClock.restart(); }
                    if (btn90.getGlobalBounds().contains(mousePos)) { timeLimit = 90; challengeMode = true; menuState = 2; gameClock.restart(); }
                    if (btn120.getGlobalBounds().contains(mousePos)) { timeLimit = 120; challengeMode = true; menuState = 2; gameClock.restart(); }
                }
            }

            if (menuState == 2 && !gameOver && event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char entered = static_cast<char>(event.text.unicode);
                    if (entered == 8 && !currentGuess.empty()) {
                        currentGuess.pop_back(); invalidWord = false;
                    }
                    else if (isalpha(entered) && currentGuess.length() < 5) {
                        currentGuess += toupper(entered);
                    }
                    else if (entered == 13 && currentGuess.length() == 5) {
                        if (find(dictionary.begin(), dictionary.end(), currentGuess) != dictionary.end()) {
                            for (int i = 0; i < 5; i++) {
                                int boxIndex = currentRow * 5 + i;
                                if (currentGuess[i] == targetWord[i]) boxes[boxIndex].setFillColor(sf::Color(46, 204, 113));
                                else if (targetWord.find(currentGuess[i]) != string::npos) boxes[boxIndex].setFillColor(sf::Color(241, 196, 15));
                                else boxes[boxIndex].setFillColor(sf::Color(149, 165, 166));
                            }
                            guesses.push_back(currentGuess);
                            if (currentGuess == targetWord || currentRow == 5) gameOver = true;
                            currentGuess = ""; currentRow++; invalidWord = false;
                        } else { invalidWord = true; }
                    }
                }
            } else if (gameOver && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    targetWord = getRandomWord(dictionary);
                    guesses.clear(); currentGuess = ""; currentRow = 0;
                    gameOver = false; invalidWord = false; menuState = 0; challengeMode = false;
                    for (auto& b : boxes) b.setFillColor(sf::Color::White);
                }
            }
        }

        window.clear(sf::Color(240, 240, 240));

        if (menuState == 0) {
            sf::Text title("WORDLE", font, 50);
            title.setFillColor(sf::Color::Black); title.setPosition(110, 80);
            window.draw(title);
            window.draw(btnPlay); window.draw(btnChallenge);
            sf::Text tP("PLAY", font, 25); tP.setPosition(190, 212); window.draw(tP);
            sf::Text tC("CHALLENGE", font, 25); tC.setPosition(145, 312); window.draw(tC);
        } else if (menuState == 1) {
            sf::Text sub("Select Time Limit", font, 30);
            sub.setFillColor(sf::Color::Black); sub.setPosition(90, 150);
            window.draw(sub);
            window.draw(btn60); window.draw(btn90); window.draw(btn120);
            sf::Text t60("60s", font, 20); t60.setPosition(205, 260); window.draw(t60);
            sf::Text t90("90s", font, 20); t90.setPosition(205, 340); window.draw(t90);
            sf::Text t120("120s", font, 20); t120.setPosition(200, 420); window.draw(t120);
        } else {
            float timeLeft = timeLimit - gameClock.getElapsedTime().asSeconds();
            if (challengeMode && !gameOver) {
                if (timeLeft <= 0) { gameOver = true; timeLeft = 0; }
                sf::Text timer("Time: " + to_string((int)timeLeft) + "s", font, 30);
                timer.setFillColor(timeLeft < 10 ? sf::Color::Red : sf::Color::Black);
                timer.setPosition(160, 20); window.draw(timer);
            }

            for (auto& b : boxes) window.draw(b);
            for (int r = 0; r < guesses.size(); r++) {
                for (int c = 0; c < 5; c++) {
                    sf::Text t(string(1, guesses[r][c]), font, 35);
                    t.setFillColor(sf::Color::White);
                    t.setPosition(50.f + (c * 70) + 15, 90.f + (r * 70) + 5);
                    window.draw(t);
                }
            }
            if (!gameOver) {
                for (int i = 0; i < currentGuess.length(); i++) {
                    sf::Text t(string(1, currentGuess[i]), font, 35);
                    t.setFillColor(sf::Color::Black);
                    t.setPosition(50.f + (i * 70) + 15, 90.f + (currentRow * 70) + 5);
                    window.draw(t);
                }
                if (invalidWord) {
                    sf::Text warn("Not in word list!", font, 18);
                    warn.setFillColor(sf::Color::Red); warn.setPosition(150, 60); window.draw(warn);
                }
            } else {
                string result = "";
                if (challengeMode && timeLeft <= 0 && (guesses.empty() || guesses.back() != targetWord)) result = "TIME OUT!";
                else if (!guesses.empty() && guesses.back() == targetWord) result = "YOU WIN!";
                else result = "GAME OVER";

                sf::Text res(result, font, 45);
                res.setFillColor(result == "YOU WIN!" ? sf::Color(46, 204, 113) : sf::Color::Red);
                sf::FloatRect resBounds = res.getLocalBounds();
                res.setOrigin(resBounds.left + resBounds.width / 2.0f, resBounds.top + resBounds.height / 2.0f);
                res.setPosition(window.getSize().x / 2.0f, 550); // เลื่อนลงมาที่ 550
                window.draw(res);

                sf::Text ans("Answer: " + targetWord, font, 22);
                ans.setFillColor(sf::Color::Blue);
                sf::FloatRect ansBounds = ans.getLocalBounds();
                ans.setOrigin(ansBounds.left + ansBounds.width / 2.0f, ansBounds.top + ansBounds.height / 2.0f);
                ans.setPosition(window.getSize().x / 2.0f, 610);
                window.draw(ans);

                sf::Text msg("Press SPACE to Menu", font, 18);
                msg.setFillColor(sf::Color::Black);
                sf::FloatRect msgBounds = msg.getLocalBounds();
                msg.setOrigin(msgBounds.left + msgBounds.width / 2.0f, msgBounds.top + msgBounds.height / 2.0f);
                msg.setPosition(window.getSize().x / 2.0f, 660);
                window.draw(msg);
            }
        }
        window.display();
    }
    return 0;
}