#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

string GREEN_BG = "\033[42;30m";  
string YELLOW_BG = "\033[43;30m"; 
string GRAY_BG = "\033[47;30m";   
string RESET = "\033[0m";         

int main() {
    vector<string> vocabulary;
    string word_from_file;
    ifstream file("word.txt");

    if (file.is_open()) {
        while (file >> word_from_file) {
            vocabulary.push_back(word_from_file);
        }
        file.close();
    } else {
        cout << "Error: Could not open word.txt" << endl;
        return 1;
    }

    srand(time(0));
    string target = vocabulary[rand() % vocabulary.size()];

    string guess;
    int tries = 0;

    cout << "--- Welcome to Wordle ---" << endl;

    while (tries < 6) {
        cout << "\n[" << tries + 1 << "/6] Enter your guess: ";
        cin >> guess;

        if (guess.length() != 5) {
            cout << "(!) Please enter exactly 5 letters." << endl;
            continue;
        }

        cout << "\n    "; 
        for (int i = 0; i < 5; i++) {
            string bg;
            char c = toupper(guess[i]);

            if (guess[i] == (char)tolower(target[i]) || guess[i] == (char)toupper(target[i])) {
                if (toupper(guess[i]) == toupper(target[i])) {
                    bg = (i < 5 && toupper(guess[i]) == toupper(target[i])) ? GREEN_BG : GRAY_BG;
                }
            }
            
            if (toupper(guess[i]) == toupper(target[i])) {
                bg = GREEN_BG;
            } else {
                bool found = false;
                for (int j = 0; j < 5; j++) {
                    if (toupper(guess[i]) == toupper(target[j])) found = true;
                }
                bg = found ? YELLOW_BG : GRAY_BG;
            }
            cout << bg << " " << c << " " << RESET << "  ";
        }
        cout << endl;

        if (guess == target) {
           cout << "\n==============================" << endl;
        cout << "* CONGRATULATIONS! YOU WON! *" << endl;
        cout << "==============================" << endl;
            break;
        }
        tries++;
    }

    if (tries == 6 && guess != target) {
       cout << "\n[!] GAME OVER [!]" << endl;
        cout << "The secret word was: " << target << endl;
    }

    return 0;
}