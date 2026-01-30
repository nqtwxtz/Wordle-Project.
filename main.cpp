#include <iostream>
#include <string>

using namespace std;

int main() {
    string target = "APPLE";
    string guess;
    int tries = 0;

    cout << "--- Welcome to Wordle ---" << endl;

    while (tries < 6) {
        cout << "Enter your guess (5 letters): ";
        cin >> guess;

        if (guess.length() != 5) {
            cout << "Please enter 5 letters!" << endl;
            continue;
        }

        // เช็คตัวอักษรทีละตัว
        for (int i = 0; i < 5; i++) {
            if (guess[i] == target[i]) {
                cout << "[CORRECT] ";
            } else {
                cout << "[WRONG]   ";
            }
        }
        cout << endl;

        if (guess == target) {
            cout << "You Won!" << endl;
            break;
        }
        tries++;
    }
    return 0;
}