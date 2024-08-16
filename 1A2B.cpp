#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <set>

struct Score {
    std::string name;
    int attempts;
    std::string difficulty;
};

std::vector<Score> highScores;
const std::string HIGH_SCORE_FILE = "1a2b_high_scores.txt";

std::string generateSecretNumber(int length) {
    std::vector<int> digits(10);
    std::iota(digits.begin(), digits.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(digits.begin(), digits.end(), g);
    std::string secret;
    for (int i = 0; i < length; ++i) {
        secret += std::to_string(digits[i]);
    }
    return secret;
}

std::pair<int, int> checkGuess(const std::string& secret, const std::string& guess) {
    int a = 0, b = 0;
    for (size_t i = 0; i < secret.length(); ++i) {
        if (guess[i] == secret[i]) {
            ++a;
        } else if (secret.find(guess[i]) != std::string::npos) {
            ++b;
        }
    }
    return {a, b};
}

bool isValidGuess(const std::string& guess, int length) {
    if (guess.length() != length) return false;
    for (char c : guess) {
        if (!std::isdigit(c)) return false;
    }
    return std::set<char>(guess.begin(), guess.end()).size() == length;
}

void loadHighScores() {
    std::ifstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        Score score;
        while (file >> score.name >> score.attempts >> score.difficulty) {
            highScores.push_back(score);
        }
        file.close();
    }
}

void saveHighScores() {
    std::ofstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        for (const auto& score : highScores) {
            file << score.name << " " << score.attempts << " " << score.difficulty << "\n";
        }
        file.close();
    }
}

void displayHighScores() {
    std::cout << "\nHigh Scores:\n";
    std::cout << std::setw(15) << "Name" << std::setw(10) << "Attempts" << std::setw(15) << "Difficulty\n";
    std::cout << std::string(40, '-') << "\n";
    for (const auto& score : highScores) {
        std::cout << std::setw(15) << score.name << std::setw(10) << score.attempts 
                  << std::setw(15) << score.difficulty << "\n";
    }
}

void updateHighScores(const std::string& name, int attempts, const std::string& difficulty) {
    highScores.push_back({name, attempts, difficulty});
    std::sort(highScores.begin(), highScores.end(), 
              [](const Score& a, const Score& b) { return a.attempts < b.attempts; });
    if (highScores.size() > 10) {
        highScores.resize(10);
    }
    saveHighScores();
}

void playGame() {
    std::cout << "Choose difficulty:\n";
    std::cout << "1. Easy (4 digits)\n";
    std::cout << "2. Medium (5 digits)\n";
    std::cout << "3. Hard (6 digits)\n";
    int difficultyChoice;
    std::cin >> difficultyChoice;

    int length;
    std::string difficultyName;
    switch (difficultyChoice) {
        case 1: length = 4; difficultyName = "Easy"; break;
        case 2: length = 5; difficultyName = "Medium"; break;
        case 3: length = 6; difficultyName = "Hard"; break;
        default: length = 4; difficultyName = "Easy";
    }

    std::string secret = generateSecretNumber(length);
    int attempts = 0;

    std::cout << "Welcome to 1A2B Game! (" << difficultyName << " mode)\n";
    std::cout << "Try to guess the " << length << "-digit number. Each digit is different.\n";

    while (true) {
        std::string guess;
        std::cout << "Enter your guess: ";
        std::cin >> guess;

        if (!isValidGuess(guess, length)) {
            std::cout << "Invalid guess. Please enter a " << length << "-digit number with unique digits.\n";
            continue;
        }

        ++attempts;

        auto [a, b] = checkGuess(secret, guess);

        if (a == length) {
            std::cout << "Congratulations! You've guessed the number in " << attempts << " attempts.\n";
            
            std::string name;
            std::cout << "Enter your name for the high score: ";
            std::cin >> name;
            updateHighScores(name, attempts, difficultyName);
            
            break;
        } else {
            std::cout << a << "A" << b << "B\n";
        }
    }
}

int main() {
    loadHighScores();
    char playAgain;
    do {
        playGame();
        displayHighScores();
        std::cout << "Do you want to play again? (y/n): ";
        std::cin >> playAgain;
    } while (playAgain == 'y' || playAgain == 'Y');
    return 0;
}