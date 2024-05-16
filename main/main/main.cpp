#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cstdlib> // For rand()
#include <ctime>   // For time()

const int WIDTH = 800;
const int HEIGHT = 600;
const int ROWS = 30;
const int COLS = 40;
const int BORDER_SIZE = 5;

// Struct to represent a cell in the maze
struct Cell {
    int row, col;
    bool visited;
    bool walls[4];
    bool checkpoint;

    Cell(int r, int c) : row(r), col(c), visited(false), checkpoint(false) {
        for (int i = 0; i < 4; ++i) walls[i] = true;
    }
};

// Class to represent the maze
class Maze {
public:
    Maze();
    void generate();
    void generateExit();
    void draw(sf::RenderWindow& window);
    bool isWall(int row, int col, int dir);
    bool isCheckpoint(int row, int col);
    void removeCheckpoint(int row, int col);

private:
    std::vector<Cell> cells;

    int getIndex(int row, int col);
    bool isValid(int row, int col);
    void connectNeighbors(Cell& current, Cell& neighbor);
};

// Class to represent the player
class Player {
public:
    Player(int r, int c) : row(r), col(c) {}
    void move(int dx, int dy);
    void draw(sf::RenderWindow& window);
    int row, col;
};

// Class to represent a button
class Button {
public:
    Button(float x, float y, float width, float height, const std::string& text, sf::Font& font)
        : m_text(text, font, 30), m_width(width), m_height(height) {
        m_rect.setPosition(x, y);
        m_rect.setSize(sf::Vector2f(width, height));
        m_rect.setFillColor(sf::Color::Green);

        m_text.setFillColor(sf::Color::White);
        sf::FloatRect textRect = m_text.getLocalBounds();
        m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        m_text.setPosition(x + width / 8.5f, y + height / 1.5f);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_rect);
        window.draw(m_text);
    }

    bool isClicked(const sf::Vector2f& mousePos) {
        return m_rect.getGlobalBounds().contains(mousePos);
    }

private:
    sf::RectangleShape m_rect;
    sf::Text m_text;
    float m_width;
    float m_height;
};

// Class to represent the menu
class Menu {
public:
    Menu();
    void draw(sf::RenderWindow& window);
    int handleInput(sf::RenderWindow& window);
    sf::Font& getFont();
private:
    sf::Text title;
    sf::Font font;
    Button startButton;
    Button exitButton;
};

// Class to represent questions at checkpoints
class Question {
public:
    Question(const std::string& q, const std::string& a) : question(q), answer(a) {}
    std::string getQuestion() const { return question; }
    bool checkAnswer(const std::string& ans) const { return ans == answer; }

private:
    std::string question;
    std::string answer;
};

// Global list of questions
std::vector<Question> questions = {
    Question("What is 2 + 2?", "4"),
    Question("What is the capital of France?", "Paris"),
    Question("What is the color of the sky?", "Blue")
};

sf::Font& Menu::getFont() {
    return font;
}

Maze::Maze() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cells.push_back(Cell(i, j));
        }
    }
}

void Maze::generateExit() {
    int side = rand() % 4;
    int cellIndex;
    switch (side) {
    case 0: cellIndex = rand() % COLS; connectNeighbors(cells[cellIndex], cells[cellIndex + COLS]); break;
    case 1: cellIndex = COLS * (ROWS - 1) + (rand() % COLS); connectNeighbors(cells[cellIndex], cells[cellIndex + 1]); break;
    case 2: cellIndex = COLS * (ROWS - 1) + (rand() % COLS); connectNeighbors(cells[cellIndex], cells[cellIndex - COLS]); break;
    case 3: cellIndex = rand() % COLS; connectNeighbors(cells[cellIndex], cells[cellIndex - 1]); break;
    }
}

void Maze::generate() {
    std::stack<Cell*> stack;
    Cell* current = &cells[0];
    current->visited = true;

    while (true) {
        std::vector<Cell*> unvisitedNeighbors;
        for (int i = 0; i < 4; ++i) {
            int next_row = current->row + (i == 0 ? -1 : (i == 2 ? 1 : 0));
            int next_col = current->col + (i == 1 ? 1 : (i == 3 ? -1 : 0));
            if (isValid(next_row, next_col) && !cells[getIndex(next_row, next_col)].visited) {
                unvisitedNeighbors.push_back(&cells[getIndex(next_row, next_col)]);
            }
        }

        if (!unvisitedNeighbors.empty()) {
            Cell* next = unvisitedNeighbors[rand() % unvisitedNeighbors.size()];
            connectNeighbors(*current, *next);
            stack.push(current);
            current = next;
            current->visited = true;
        }
        else if (!stack.empty()) {
            current = stack.top();
            stack.pop();
        }
        else {
            break;
        }
    }

    generateExit();

    std::vector<std::pair<int, int>> checkpointPositions = { {1,3}, {8,22}, {13,15}, {7,0}, {15,7}, {29,11}, {21,39}, {22,27} };
    for (const auto& pos : checkpointPositions) {
        int row = pos.first;
        int col = pos.second;
        cells[getIndex(row, col)].checkpoint = true;
    }
}

void Maze::draw(sf::RenderWindow& window) {
    float cellSizeX = static_cast<float>(WIDTH - 2 * BORDER_SIZE) / COLS;
    float cellSizeY = static_cast<float>(HEIGHT - 2 * BORDER_SIZE) / ROWS;

    for (int i = 0; i < cells.size(); ++i) {
        int x = cells[i].col * cellSizeX + BORDER_SIZE;
        int y = cells[i].row * cellSizeY + BORDER_SIZE;

        for (int j = 0; j < 4; ++j) {
            if (cells[i].walls[j]) {
                sf::RectangleShape wall;
                switch (j) {
                case 0: wall.setSize(sf::Vector2f(cellSizeX, 1)); wall.setPosition(x, y); break;
                case 1: wall.setSize(sf::Vector2f(1, cellSizeY)); wall.setPosition(x + cellSizeX, y); break;
                case 2: wall.setSize(sf::Vector2f(cellSizeX, 1)); wall.setPosition(x, y + cellSizeY); break;
                case 3: wall.setSize(sf::Vector2f(1, cellSizeY)); wall.setPosition(x, y); break;
                }
                wall.setFillColor(sf::Color::White);
                window.draw(wall);
            }
        }

        if (cells[i].checkpoint) {
            sf::RectangleShape checkpoint(sf::Vector2f(cellSizeX, cellSizeY));
            checkpoint.setPosition(x, y);
            checkpoint.setFillColor(sf::Color::Yellow);
            window.draw(checkpoint);
        }
    }
}

bool Maze::isWall(int row, int col, int dir) {
    if (!isValid(row, col)) return true;
    return cells[getIndex(row, col)].walls[dir];
}

bool Maze::isCheckpoint(int row, int col) {
    return cells[getIndex(row, col)].checkpoint;
}

void Maze::removeCheckpoint(int row, int col) {
    cells[getIndex(row, col)].checkpoint = false;
}

int Maze::getIndex(int row, int col) {
    return row * COLS + col;
}

bool Maze::isValid(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

void Maze::connectNeighbors(Cell& current, Cell& neighbor) {
    int dx = neighbor.col - current.col;
    int dy = neighbor.row - current.row;

    if (dx == 1) {
        current.walls[1] = false;
        neighbor.walls[3] = false;
    }
    else if (dx == -1) {
        current.walls[3] = false;
        neighbor.walls[1] = false;
    }
    else if (dy == 1) {
        current.walls[2] = false;
        neighbor.walls[0] = false;
    }
    else if (dy == -1) {
        current.walls[0] = false;
        neighbor.walls[2] = false;
    }
}

void Player::move(int dx, int dy) {
    row += dy;
    col += dx;
}

void Player::draw(sf::RenderWindow& window) {
    float cellSizeX = static_cast<float>(WIDTH - 2 * BORDER_SIZE) / COLS;
    float cellSizeY = static_cast<float>(HEIGHT - 2 * BORDER_SIZE) / ROWS;
    float radius = std::min(cellSizeX, cellSizeY) / 3;

    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(col * cellSizeX + BORDER_SIZE + cellSizeX / 2 - radius, row * cellSizeY + BORDER_SIZE + cellSizeY / 2 - radius);

    window.draw(circle);
}

Menu::Menu() : startButton(300, 200, 200, 100, "Start Game", font), exitButton(300, 400, 200, 100, "Exit", font) {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
    }

    title.setFont(font);
    title.setString("Maze Game");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::White);
    title.setPosition(250, 50);
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(title);
    startButton.draw(window);
    exitButton.draw(window);
}

int Menu::handleInput(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (startButton.isClicked(mousePosF)) {
        return 1;
    }
    if (exitButton.isClicked(mousePosF)) {
        return -1;
    }
    return 0;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze Game");
    window.setFramerateLimit(60);

    Menu menu;
    Maze maze;
    Player player(0, 0);

    bool gameStarted = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && !gameStarted) {
                int menuResult = menu.handleInput(window);
                if (menuResult == 1) {
                    gameStarted = true;
                    maze.generate();
                }
                else if (menuResult == -1) {
                    window.close();
                }
            }

            if (event.type == sf::Event::KeyPressed && gameStarted) {
                if (event.key.code == sf::Keyboard::Up && !maze.isWall(player.row, player.col, 0)) {
                    player.move(0, -1);
                }
                if (event.key.code == sf::Keyboard::Right && !maze.isWall(player.row, player.col, 1)) {
                    player.move(1, 0);
                }
                if (event.key.code == sf::Keyboard::Down && !maze.isWall(player.row, player.col, 2)) {
                    player.move(0, 1);
                }
                if (event.key.code == sf::Keyboard::Left && !maze.isWall(player.row, player.col, 3)) {
                    player.move(-1, 0);
                }

                if (maze.isCheckpoint(player.row, player.col)) {
                    int questionIndex = rand() % questions.size();
                    std::string question = questions[questionIndex].getQuestion();
                    std::string answer;
                    std::cout << question << "\n";
                    std::cin >> answer;
                    if (questions[questionIndex].checkAnswer(answer)) {
                        maze.removeCheckpoint(player.row, player.col);
                        std::cout << "Correct!\n";
                    }
                    else {
                        std::cout << "Wrong! Try again later.\n";
                    }
                }
            }
        }

        window.clear();
        if (!gameStarted) {
            menu.draw(window);
        }
        else {
            maze.draw(window);
            player.draw(window);
        }
        window.display();
    }

    return 0;
}

