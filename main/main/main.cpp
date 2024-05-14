#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cstdlib> // For rand()
#include <string>
#include <map>


const int WIDTH = 800;    // Width of the window
const int HEIGHT = 600;   // Height of the window
const int ROWS = 30;      // Number of rows in the maze
const int COLS = 40;      // Number of columns in the maze
const int BORDER_SIZE = 5; // Size of the border around the maze

struct Question {
    std::string question;
    bool correctAnswer;
};

class Cell {
public:
    int row, col;
    bool visited;
    bool walls[4]; // top, right, bottom, left
    bool checkpoint; // Indicates if the cell is a checkpoint
    std::vector<Cell*> neighbors;

    Cell(int r, int c) : row(r), col(c), visited(false), checkpoint(false) {
        for (int i = 0; i < 4; ++i) {
            walls[i] = true;
        }
    }
};


class Maze {
public:
    Maze();
    void generate();
    void generateExit();
    void draw(sf::RenderWindow& window);
    bool isWall(int row, int col, int dir); // Check if there's a wall in a specific direction
    bool answerQuestion(int row, int col, bool answer);


    void setCheckpoints(const std::map<int, Question>& checkpointQuestions); // Set checkpoint questions
    bool isCheckpoint(int row, int col);
    Question getQuestion(int row, int col);


private:
    std::vector<Cell> cells;

    int getIndex(int row, int col);
    bool isValid(int row, int col);
    void connectNeighbors(Cell& current, Cell& neighbor);

    std::unordered_set<int> checkpoints;
    std::map<int, Question> checkpointQuestions;




};




bool ansQuestion(int row, int col, bool ans) {
    Question question = getQuestion(row, col);
    if (question.correctAnswer == ans) {
        return true; // Correct answer
    }
    else {
        return false; // Incorrect answer
    }
}


class Player {
public:
    Player(int r, int c, int lives = 3) : row(r), col(c), lives(lives) {}
    void move(int dx, int dy); // Move the player
    void draw(sf::RenderWindow& window); // Draw the player
    void loseLife(); // Decrement player's life
    int getLives() const; // Get player's remaining lives
public:
    int row, col;
    int lives;
};

void Player::loseLife() {
    lives--;
}

int Player::getLives() const {
    return lives;
}

void Player::draw(sf::RenderWindow& window) {
    // Draw the player

}

class GamePanel {
public:
    GamePanel(float x, float y, sf::Font& font) : font(font) {
        livesText.setFont(font);
        livesText.setCharacterSize(20);
        livesText.setFillColor(sf::Color::White);
        livesText.setPosition(x, y);
        updateLives(3); // Initial lives
    }

    void updateLives(int lives) {
        livesText.setString("Lives: " + std::to_string(lives));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(livesText);
    }

private:
    sf::Text livesText;
    sf::Font& font;
};

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

class Menu {
public:
    Menu();
    void draw(sf::RenderWindow& window);
    int handleInput(sf::RenderWindow& window);
private:
    sf::Text title;
    sf::Font font;
    Button startButton;
    Button exitButton;
};

Maze::Maze() {
    // Initialize the cells
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cells.push_back(Cell(i, j));
        }
    }
}




void Maze::setCheckpoints(const std::map<int, Question>& checkpointQuestions) {
    this->checkpointQuestions = checkpointQuestions;
    for (const auto& pair : checkpointQuestions) {
        checkpoints.insert(pair.first);
        cells[pair.first].checkpoint = true;
    }
}

Question Maze::getQuestion(int row, int col) {
    int index = getIndex(row, col);
    auto it = checkpointQuestions.find(index);
    if (it != checkpointQuestions.end()) {
        return it->second;
    }
    return { "", false }; // Default empty question
}

bool Maze::isCheckpoint(int row, int col) {
    int index = getIndex(row, col);
    return checkpoints.find(index) != checkpoints.end();
}


bool Maze::answerQuestion(int row, int col, bool answer) {
    int index = getIndex(row, col);
    auto it = checkpointQuestions.find(index);
    if (it != checkpointQuestions.end()) {
        return it->second.correctAnswer == answer;
    }
    return false;
}


void Maze::generateExit() {
    // Randomly select a border side (0: top, 1: right, 2: bottom, 3: left)
    int side = rand() % 4;

    // Randomly select a cell on the chosen side
    int cellIndex;
    switch (side) {
    case 0: // top side
        cellIndex = rand() % COLS;
        connectNeighbors(cells[cellIndex], cells[cellIndex + COLS]);
        break;
    case 1: // right side
        cellIndex = COLS * (ROWS - 1) + (rand() % COLS);
        connectNeighbors(cells[cellIndex], cells[cellIndex + 1]);
        break;
    case 2: // bottom side
        cellIndex = COLS * (ROWS - 1) + (rand() % COLS);
        connectNeighbors(cells[cellIndex], cells[cellIndex - COLS]);
        break;
    case 3: // left side
        cellIndex = rand() % COLS;
        connectNeighbors(cells[cellIndex], cells[cellIndex - 1]);
        break;
    }
}

void Maze::generate() {
    std::stack<Cell*> stack;
    Cell* current = &cells[0];
    current->visited = true;

    while (true) {
        // Find unvisited neighbors
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

        for (int i = 0; i < 3; ++i) { // Change 3 to the desired number of checkpoints
            int randRow = rand() % ROWS;
            int randCol = rand() % COLS;
            int index = getIndex(randRow, randCol);
            checkpointQuestions[index] = { "Question " + std::to_string(i + 1), true }; // Modify the question accordingly
            checkpoints.insert(index);
            cells[index].checkpoint = true;
        }

    }

    generateExit(); // Generate exit after maze generation
}

void Maze::draw(sf::RenderWindow& window) {
    // Calculate cell size based on window dimensions
    float cellSizeX = static_cast<float>(WIDTH - 2 * BORDER_SIZE) / COLS;
    float cellSizeY = static_cast<float>(HEIGHT - 2 * BORDER_SIZE) / ROWS;

    for (int i = 0; i < cells.size(); ++i) {
        int x = cells[i].col * cellSizeX + BORDER_SIZE;
        int y = cells[i].row * cellSizeY + BORDER_SIZE;

        // Draw walls
        for (int j = 0; j < 4; ++j) {
            if (cells[i].walls[j]) {
                sf::RectangleShape wall;
                switch (j) {
                case 0: // top
                    wall.setSize(sf::Vector2f(cellSizeX, 1));
                    wall.setPosition(x, y);
                    break;
                case 1: // right
                    wall.setSize(sf::Vector2f(1, cellSizeY));
                    wall.setPosition(x + cellSizeX, y);
                    break;
                case 2: // bottom
                    wall.setSize(sf::Vector2f(cellSizeX, 1));
                    wall.setPosition(x, y + cellSizeY);
                    break;
                case 3: // left
                    wall.setSize(sf::Vector2f(1, cellSizeY));
                    wall.setPosition(x, y);
                    break;
                }
                wall.setFillColor(sf::Color::White); // Set wall color to white
                window.draw(wall);
            }
        }

        // Draw exit cell
        if (cells[i].row == ROWS - 1 && cells[i].col == COLS - 1) {
            sf::RectangleShape exit;
            exit.setSize(sf::Vector2f(cellSizeX, cellSizeY));
            exit.setPosition(x, y);
            exit.setFillColor(sf::Color::Red); // Set exit cell color to red
            window.draw(exit);
        }
    }
}

bool Maze::isWall(int row, int col, int dir) {
    if (row < 0 || col < 0 || row >= ROWS || col >= COLS)
        return true; // Treat border as a wall
    return cells[getIndex(row, col)].walls[dir];
}

int Maze::getIndex(int row, int col) {
    return row * COLS + col;
}

bool Maze::isValid(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

void Maze::connectNeighbors(Cell& current, Cell& neighbor) {
    if (current.row == neighbor.row) {
        if (current.col < neighbor.col) {
            current.walls[1] = false; // current's right wall
            neighbor.walls[3] = false; // neighbor's left wall
        }
        else {
            current.walls[3] = false; // current's left wall
            neighbor.walls[1] = false; // neighbor's right wall
        }
    }
    else {
        if (current.row < neighbor.row) {
            current.walls[2] = false; // current's bottom wall
            neighbor.walls[0] = false; // neighbor's top wall
        }
        else {
            current.walls[0] = false; // current's top wall
            neighbor.walls[2] = false; // neighbor's bottom wall
        }
    }
}

void Player::move(int dx, int dy) {
    row += dy;
    col += dx;
}

void Player::draw(sf::RenderWindow& window) {
    // Calculate cell size based on window dimensions
    float cellSizeX = static_cast<float>(WIDTH - 2 * BORDER_SIZE) / COLS;
    float cellSizeY = static_cast<float>(HEIGHT - 2 * BORDER_SIZE) / ROWS;

    sf::CircleShape playerShape(std::min(cellSizeX, cellSizeY) / 2);
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition(col * cellSizeX + cellSizeX / 4 + BORDER_SIZE, row * cellSizeY + cellSizeY / 4 + BORDER_SIZE);
    window.draw(playerShape);
}

Menu::Menu() : startButton(WIDTH / 2.0f - 100.0f, 250.0f, 200.0f, 50.0f, "Start Game", font),
exitButton(WIDTH / 2.0f - 100.0f, 350.0f, 200.0f, 50.0f, "      Exit", font) {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }

    title.setFont(font);
    title.setString("Maze Game");
    title.setCharacterSize(60);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleRect = title.getLocalBounds();
    title.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
    title.setPosition(WIDTH / 2.0f, 100.0f);
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(title);
    startButton.draw(window);
    exitButton.draw(window);
}

int int Menu::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (startButton.isClicked(mousePos)) {
                return 1; // Start Game button clicked
            }
            if (exitButton.isClicked(mousePos)) {
                return -1; // Exit button clicked
            }
        }
    }
    return 0; // No button clicked
}
