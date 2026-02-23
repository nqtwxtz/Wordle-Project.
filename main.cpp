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

    vector<string> dictionary;
    string targetWord = "";
    string currentGuess = "";
    vector<string> guesses;
    int currentRow = 0;
    bool gameOver = false;
    bool invalidWord = false;
    
    int menuState = 0; // 0: Mode, 1: Category, 2: Time, 3: Game
    bool challengeMode = false;
    float timeLimit = 0;
    sf::Clock gameClock;

    // UI Buttons
    sf::RectangleShape btnPlay(sf::Vector2f(200, 60)), btnChallenge(sf::Vector2f(200, 60));
    btnPlay.setPosition(125, 250); btnPlay.setFillColor(sf::Color(46, 204, 113));
    btnChallenge.setPosition(125, 350); btnChallenge.setFillColor(sf::Color(231, 76, 60));

    sf::RectangleShape btnCat[4];
    string catNames[] = {"ANIMALS", "FOOD", "ITEMS", "ALL"};
    sf::Color catColors[] = {sf::Color(243, 156, 18), sf::Color(230, 126, 34), sf::Color(155, 89, 182), sf::Color(52, 73, 94)};
    for(int i=0; i<4; i++) {
        btnCat[i].setSize(sf::Vector2f(250, 55));
        btnCat[i].setPosition(100, 200 + (i * 75));
        btnCat[i].setFillColor(catColors[i]);
    }

    sf::RectangleShape btnTime[3];
    string timeLabels[] = {"60s", "90s", "120s"};
    float times[] = {60, 90, 120};
    for(int i=0; i<3; i++) {
        btnTime[i].setSize(sf::Vector2f(120, 50));
        btnTime[i].setPosition(165, 250 + (i * 80));
        btnTime[i].setFillColor(sf::Color(52, 152, 219));
    }

    vector<sf::RectangleShape> boxes;
    for (int i = 0; i < 30; i++) {
        sf::RectangleShape box(sf::Vector2f(60.f, 60.f));
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(200, 200, 200));
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
                    if (btnPlay.getGlobalBounds().contains(mousePos)) { challengeMode = false; menuState = 1; }
                    if (btnChallenge.getGlobalBounds().contains(mousePos)) { challengeMode = true; menuState = 1; }
                } 
                else if (menuState == 1) {
                    string files[] = {"animals.txt", "food.txt", "items.txt", "word.txt"};
                    for(int i=0; i<4; i++) {
                        if (btnCat[i].getGlobalBounds().contains(mousePos)) {
                            dictionary = loadDictionary(files[i]);
                            targetWord = getRandomWord(dictionary);
                            if (challengeMode) menuState = 2; else { menuState = 3; gameClock.restart(); }
                        }
                    }
                }
                else if (menuState == 2) {
                    for(int i=0; i<3; i++) {
                        if (btnTime[i].getGlobalBounds().contains(mousePos)) {
                            timeLimit = times[i]; menuState = 3; gameClock.restart();
                        }
                    }
                }
            }

            if (menuState == 3 && !gameOver && event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char entered = static_cast<char>(event.text.unicode);
                    if (entered == 8 && !currentGuess.empty()) { currentGuess.pop_back(); invalidWord = false; }
                    else if (isalpha(entered) && currentGuess.length() < 5) { currentGuess += toupper(entered); }
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
            } else if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                guesses.clear(); currentGuess = ""; currentRow = 0;
                gameOver = false; invalidWord = false; menuState = 0; 
                for (auto& b : boxes) b.setFillColor(sf::Color::White);
            }
        }

        window.clear(sf::Color(240, 240, 240));

        if (menuState == 0) {
            sf::Text title("WORDLE", font, 60);
            title.setFillColor(sf::Color::Black); 
            sf::FloatRect tb = title.getLocalBounds(); title.setOrigin(tb.width/2, tb.height/2);
            title.setPosition(225, 120); window.draw(title);
            window.draw(btnPlay); window.draw(btnChallenge);
            sf::Text tP("PLAY", font, 25); tP.setPosition(190, 262); window.draw(tP);
            sf::Text tC("CHALLENGE", font, 25); tC.setPosition(145, 362); window.draw(tC);
        } 
        else if (menuState == 1) {
            sf::Text sub("Select Category", font, 35);
            sub.setFillColor(sf::Color::Black); 
            sf::FloatRect sb = sub.getLocalBounds(); sub.setOrigin(sb.width/2, sb.height/2);
            sub.setPosition(225, 120); window.draw(sub);
            for(int i=0; i<4; i++) {
                window.draw(btnCat[i]);
                sf::Text t(catNames[i], font, 22);
                sf::FloatRect tb = t.getLocalBounds(); t.setOrigin(tb.width/2, tb.height/2);
                t.setPosition(225, 225 + (i * 75)); window.draw(t);
            }
        }
        else if (menuState == 2) {
            sf::Text sub("Select Time", font, 35);
            sub.setFillColor(sf::Color::Black); 
            sf::FloatRect sb = sub.getLocalBounds(); sub.setOrigin(sb.width/2, sb.height/2);
            sub.setPosition(225, 150); window.draw(sub);
            for(int i=0; i<3; i++) {
                window.draw(btnTime[i]);
                sf::Text t(timeLabels[i], font, 22);
                sf::FloatRect tb = t.getLocalBounds(); t.setOrigin(tb.width/2, tb.height/2);
                t.setPosition(225, 272 + (i * 80)); window.draw(t);
            }
        }
        else {
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
                sf::FloatRect rb = res.getLocalBounds(); res.setOrigin(rb.left + rb.width/2.0f, rb.top + rb.height/2.0f);
                res.setPosition(225, 550); window.draw(res);

                sf::Text ans("Answer: " + targetWord, font, 22);
                ans.setFillColor(sf::Color::Blue);
                sf::FloatRect ab = ans.getLocalBounds(); ans.setOrigin(ab.left + ab.width/2.0f, ab.top + ab.height/2.0f);
                ans.setPosition(225, 610); window.draw(ans);

                sf::Text msg("Press SPACE to Menu", font, 18);
                msg.setFillColor(sf::Color::Black);
                sf::FloatRect mb = msg.getLocalBounds(); msg.setOrigin(mb.left + mb.width/2.0f, mb.top + mb.height/2.0f);
                msg.setPosition(225, 660); window.draw(msg);
            }
        }
        window.display();
    }
    return 0;
}