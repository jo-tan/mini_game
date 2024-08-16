#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Score {
    std::string name;
    int score;
    std::string difficulty;
};

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class Difficulty { EASY, MEDIUM, HARD };

const std::string HIGH_SCORE_FILE = "snake_high_scores.txt";
std::vector<Score> highScores;

class Snake {
private:
    WINDOW* win;
    int height, width;
    std::vector<Point> body;
    Direction dir;
    Point food;
    int score;
    bool game_over;
    Difficulty difficulty;
    int speed;

public:
    Snake(WINDOW* w, Difficulty diff) : win(w), dir(Direction::RIGHT), score(0), game_over(false), difficulty(diff) {
        getmaxyx(win, height, width);
        body.push_back(Point(width / 2, height / 2));
        generateFood();
        switch (difficulty) {
            case Difficulty::EASY: speed = 150; break;
            case Difficulty::MEDIUM: speed = 100; break;
            case Difficulty::HARD: speed = 50; break;
        }
    }

    void generateFood() {
        do {
            food.x = rand() % (width - 2) + 1;
            food.y = rand() % (height - 2) + 1;
        } while (std::find(body.begin(), body.end(), food) != body.end());
    }

    void move() {
        Point newHead = body.front();
        switch (dir) {
            case Direction::UP: newHead.y--; break;
            case Direction::DOWN: newHead.y++; break;
            case Direction::LEFT: newHead.x--; break;
            case Direction::RIGHT: newHead.x++; break;
        }

        if (newHead.x <= 0 || newHead.x >= width - 1 || newHead.y <= 0 || newHead.y >= height - 1 ||
            std::find(body.begin(), body.end(), newHead) != body.end()) {
            game_over = true;
            return;
        }

        body.insert(body.begin(), newHead);

        if (newHead == food) {
            score += 10;
            generateFood();
        } else {
            body.pop_back();
        }
    }

    void changeDir(Direction newDir) {
        if ((dir == Direction::UP && newDir != Direction::DOWN) ||
            (dir == Direction::DOWN && newDir != Direction::UP) ||
            (dir == Direction::LEFT && newDir != Direction::RIGHT) ||
            (dir == Direction::RIGHT && newDir != Direction::LEFT)) {
            dir = newDir;
        }
    }

    void draw() {
        wclear(win);
        box(win, 0, 0);

        for (const auto& p : body) {
            mvwaddch(win, p.y, p.x, '*');
        }

        mvwaddch(win, food.y, food.x, 'O');

        mvwprintw(win, 0, 2, "Score: %d", score);
        wrefresh(win);
    }

    bool isGameOver() const { return game_over; }
    int getScore() const { return score; }
    int getSpeed() const { return speed; }
};

void loadHighScores() {
    std::ifstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        Score score;
        while (file >> score.name >> score.score >> score.difficulty) {
            highScores.push_back(score);
        }
        file.close();
    }
}

void saveHighScores() {
    std::ofstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        for (const auto& score : highScores) {
            file << score.name << " " << score.score << " " << score.difficulty << "\n";
        }
        file.close();
    }
}

void displayHighScores(WINDOW* win) {
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "High Scores:");
    for (size_t i = 0; i < highScores.size() && i < 10; ++i) {
        mvwprintw(win, i + 3, 2, "%d. %s - %d (%s)", i + 1, highScores[i].name.c_str(), highScores[i].score, highScores[i].difficulty.c_str());
    }
    wrefresh(win);
    wgetch(win);
}

void updateHighScores(const std::string& name, int score, const std::string& difficulty) {
    highScores.push_back({name, score, difficulty});
    std::sort(highScores.begin(), highScores.end(), 
              [](const Score& a, const Score& b) { return a.score > b.score; });
    if (highScores.size() > 10) {
        highScores.resize(10);
    }
    saveHighScores();
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    srand(time(0));

    loadHighScores();

    int ch;
    do {
        clear();
        mvprintw(0, 0, "Snake Game");
        mvprintw(2, 0, "1. Play Game");
        mvprintw(3, 0, "2. View High Scores");
        mvprintw(4, 0, "3. Exit");
        mvprintw(6, 0, "Enter your choice: ");
        refresh();

        ch = getch();

        switch (ch) {
            case '1': {
                clear();
                mvprintw(0, 0, "Select Difficulty:");
                mvprintw(2, 0, "1. Easy");
                mvprintw(3, 0, "2. Medium");
                mvprintw(4, 0, "3. Hard");
                refresh();

                int diff_ch = getch();
                Difficulty diff;
                std::string diff_str;
                switch (diff_ch) {
                    case '1': diff = Difficulty::EASY; diff_str = "Easy"; break;
                    case '2': diff = Difficulty::MEDIUM; diff_str = "Medium"; break;
                    case '3': diff = Difficulty::HARD; diff_str = "Hard"; break;
                    default: diff = Difficulty::MEDIUM; diff_str = "Medium";
                }

                WINDOW* game_win = newwin(0, 0, 0, 0);
                keypad(game_win, TRUE);
                nodelay(game_win, TRUE);

                Snake snake(game_win, diff);

                while (!snake.isGameOver()) {
                    snake.draw();
                    int key = wgetch(game_win);
                    switch (key) {
                        case KEY_UP: snake.changeDir(Direction::UP); break;
                        case KEY_DOWN: snake.changeDir(Direction::DOWN); break;
                        case KEY_LEFT: snake.changeDir(Direction::LEFT); break;
                        case KEY_RIGHT: snake.changeDir(Direction::RIGHT); break;
                    }
                    snake.move();
                    std::this_thread::sleep_for(std::chrono::milliseconds(snake.getSpeed()));
                }

                nodelay(game_win, FALSE);
                mvwprintw(game_win, 10, 10, "Game Over! Your score: %d", snake.getScore());
                mvwprintw(game_win, 12, 10, "Enter your name: ");
                char name[50];
                echo();
                wgetstr(game_win, name);
                noecho();
                updateHighScores(name, snake.getScore(), diff_str);
                wrefresh(game_win);
                wgetch(game_win);
                delwin(game_win);
                break;
            }
            case '2':
                displayHighScores(stdscr);
                break;
        }
    } while (ch != '3');

    endwin();
    return 0;
}