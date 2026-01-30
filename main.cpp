#include <iostream>
#include <string>

using namespace std;

string GREEN_BG = "\033[42;30m";  
string YELLOW_BG = "\033[43;30m"; 
string GRAY_BG = "\033[47;30m";   
string RESET = "\033[0m";         

int main() {
    string target = "APPLE"; 
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

            if (guess[i] == target[i]) {
                bg = GREEN_BG;
            } else {
                bool found = false;
                for (int j = 0; j < 5; j++) {
                    if (guess[i] == target[j]) found = true;
                }
                bg = found ? YELLOW_BG : GRAY_BG;
            }
            
            cout << bg << " " << c << " " << RESET << "  ";
        }
        cout << endl;

        if (guess == target) {
            cout << "\nCONGRATULATIONS! You guessed it!" << endl;
            break;
        }
        tries++;
    }

    if (tries == 6 && guess != target) {
        cout << "\nGAME OVER! The word was: " << target << endl;
    }

    return 0;
}