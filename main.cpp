#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

struct Game {
    std::string name;
    std::string command;
    std::string description;
};

std::vector<Game> games = {
    {"1A2B Game", "1a2b_game", "Guess a number with hints"},
    //{"Snake Game", "snake_game", "Classic snake game"},
    // Add more games here as you create them
};

void displayCatalog() {
    std::cout << "Welcome to the Mini-Games Collection!\n\n";
    std::cout << "Available games:\n";
    for (size_t i = 0; i < games.size(); ++i) {
        std::cout << i + 1 << ". " << games[i].name << " - " << games[i].description << "\n";
    }
    std::cout << "0. Exit\n";
}

void launchGame(const std::string& command) {
    std::string fullCommand = command;
    system(fullCommand.c_str());
}

int main() {
    while (true) {
        displayCatalog();
        
        int choice;
        std::cout << "\nEnter the number of the game you want to play (0 to exit): ";
        std::cin >> choice;
        
        if (choice == 0) {
            break;
        } else if (choice > 0 && static_cast<size_t>(choice) <= games.size()) {
            launchGame(games[choice - 1].command);
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
        
        std::cout << "\n";
    }
    
    std::cout << "Thank you for playing! Goodbye.\n";
    return 0;
}