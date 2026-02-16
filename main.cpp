#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>

using namespace std;

string getRandomWord(string filename) {
    vector<string> words;
    ifstream file(filename);
    string word;
    if (file.is_open()) {
        while (file >> word) {
            words.push_back(word);
        }
        file.close();
    }
    if (words.empty()) return "SUPER";
    
    srand(time(0));
    int randomIndex = rand() % words.size();
    string selected = words[randomIndex];
    for (auto &c : selected) c = toupper(c);
    return selected;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(450, 600), "C++ Wordle GUI");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("ARIBLK.TTF")) return -1;

    string targetWord = getRandomWord("word.txt");
    string currentGuess = "";
    vector<string> guesses;
    int currentRow = 0;
    bool gameOver = false;

    vector<sf::RectangleShape> boxes;
    for (int i = 0; i < 30; i++) {
        sf::RectangleShape box(sf::Vector2f(60.f, 60.f));
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(200, 200, 200));
        box.setPosition(50.f + (i % 5) * 70, 50.f + (i / 5) * 70);
        boxes.push_back(box);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameOver && event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char entered = static_cast<char>(event.text.unicode);
                    if (entered == 8 && !currentGuess.empty()) {
                        currentGuess.pop_back();
                    }
                    else if (isalpha(entered) && currentGuess.length() < 5) {
                        currentGuess += toupper(entered);
                    }
                    else if (entered == 13 && currentGuess.length() == 5) {
                        for (int i = 0; i < 5; i++) {
                            int boxIndex = currentRow * 5 + i;
                            if (currentGuess[i] == targetWord[i]) {
                                boxes[boxIndex].setFillColor(sf::Color(46, 204, 113));
                            } else if (targetWord.find(currentGuess[i]) != string::n