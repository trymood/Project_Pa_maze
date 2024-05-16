#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cstdlib> // For rand()
#include <sstream>

const int WIDTH = 800;    // Width of the window
const int HEIGHT = 600;   // Height of the window
const int ROWS = 30;      // Number of rows in the maze
const int COLS = 40;      // Number of columns in the maze
const int BORDER_SIZE = 5; // Size of the border around the maze

struct Cell {
    int row, col;
    bool visited;
    bool walls[4]; // top, right, bottom, left
    bool checkpoint; // Indicates if the cell is a checkpoint
    std::string question; // Question associated with the checkpoint

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
    bool isCheckpoint(int row, int col); // Check if a cell is a checkpoint
    std::string getQuestion(int row, int col); // Get the question associated with a checkpoint

private:
    std::vector<Cell> cells;

    int getIndex(int row, int col);
    bool isValid(int row, int col);
    void connectNeighbors(Cell& current, Cell& neighbor);
};

class Player {
public:
    Player(int r, int c) : row(r), col(c) {}
    void move(int dx, int dy); // Move the player
    void draw(sf::RenderWindow& window); // Draw the player
    //private:
    int row, col;
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
    //friend std::string askQuestion(sf::RenderWindow& window, sf::Font& font, const std::string& question);
    sf::Font& getFont(); // New member function to access the font
private:
    sf::Text title;
    sf::Font font;
    Button startButton;
    Button exitButton;
};

sf::Font& Menu::getFont() {
    return font;
}

Maze::Maze() {
    // Initialize the cells
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cells.push_back(Cell(i, j));
        }
    }
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
    }

    generateExit(); // Generate exit after maze generation

    // Define the positions of the checkpoints and their questions
    std::vector<std::pair<std::pair<int, int>, std::pair<std::string, std::string>>> checkpointData = {
    {{1,3}, {"What are the EU Commission objectives regarding green / sustainable energy?\n",
        "A1.The EU Commission objectives regarding green / sustainable energy are to totally replace the fossil fuels with wind and solar energy by 2035.\nA2.The EU Commission objectives regarding green / sustainable energy comprise the total banning of fossil fuels and atomic energy and use of renewable sources of energy by 2040.\nA3.The EU Commission objectives regarding green / sustainable energy are to increase the Energy efficiency in any application, to develop all the renewable energy sources by large investment expenses and to diminish with a high speed the fossil fuels energy sources\nA4.The EU Commission objectives regarding green / sustainable energy are to increase the Energy efficiency in all the technological applications, and to replace the fossil fuels use to generate energy by green / sustainable sources, including any renewable sources"}},
    {{8,22}, {"What is the definition of green / sustainable energy in comparison with renewable or clean energy?", ""}},
    {{13,15}, {"Which are the most important characteristics of renewable energy?", ""}},
    {{7,0}, {"What are the limitations of wind energy technology?", ""}},
    {{15,7}, {"The hydropower (hydroelectric energy) can be considered as a Green / sustainable energy source?", ""}},
    {{29,11}, {"What are the main characteristics of a biofuel?", ""}},
    {{21,39}, {"What is the definition of green hydrogen?", ""}},
    {{22,27}, {"What is the capital of Australia?", ""}}
    // Add more checkpoint positions and questions as needed
    };



    // Set the checkpoints and questions at the specified positions
    for (const auto& data : checkpointData) {
        int row = data.first.first;
        int col = data.first.second;
        cells[getIndex(row, col)].checkpoint = true;
        cells[getIndex(row, col)].question = data.second.first;
    }
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

        // Draw checkpoints
        if (cells[i].checkpoint) {
            sf::CircleShape checkpoint(3);
            checkpoint.setFillColor(sf::Color::Yellow);
            checkpoint.setPosition(x + cellSizeX / 2, y + cellSizeY / 2);
            window.draw(checkpoint);
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

bool Maze::isCheckpoint(int row, int col) {
    if (row < 0 || col < 0 || row >= ROWS || col >= COLS)
        return false;
    return cells[getIndex(row, col)].checkpoint;
}

std::string Maze::getQuestion(int row, int col) {
    if (row < 0 || col < 0 || row >= ROWS || col >= COLS)
        return "";
    return cells[getIndex(row, col)].question;
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

// Define a function to ask a question using SFML
std::pair<std::string, std::string> askQuestion(sf::RenderWindow& window, sf::Font& font, const std::string& question) {
    // Create a text object to display the question
    sf::Text questionText(question, font, 18);
    questionText.setFillColor(sf::Color::White); // Set text color to white for visibility

    // Set maximum width for text wrapping
    float maxWidth = window.getSize().x - 200;
    questionText.setPosition(100, 100); // Set initial position

    // Check if text exceeds maximum width
    if (questionText.getLocalBounds().width > maxWidth) {
        // Create a text string with word wrapping
        std::string wrappedText;
        std::istringstream iss(question);
        std::ostringstream oss;
        float lineWidth = 0;
        std::string word;
        while (iss >> word) {
            sf::Text tempText(word, font, 18);
            float wordWidth = tempText.getLocalBounds().width;
            if (lineWidth + wordWidth > maxWidth) {
                wrappedText += "\n";
                lineWidth = 0;
            }
            wrappedText += word + " ";
            lineWidth += wordWidth;
        }
        questionText.setString(wrappedText);
    }

    // Adjust text position for centered alignment
    questionText.setPosition((window.getSize().x - questionText.getLocalBounds().width) / 2, 100);

    // Create a text input field for the player's answer
    std::string userAnswer;
    sf::Text answerText("", font, 18);
    answerText.setFillColor(sf::Color::White); // Set text color to white
    answerText.setPosition((window.getSize().x - questionText.getLocalBounds().width) / 2, 200); // Center horizontally

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    userAnswer += static_cast<char>(event.text.unicode);
                    answerText.setString(userAnswer);
                }
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                return { userAnswer, question };
            }
        }

        window.clear(sf::Color::Black);
        window.draw(questionText);
        window.draw(answerText);
        window.display();
    }

    return { "", "" };
}




int Menu::handleInput(sf::RenderWindow& window) {
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

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze");
    window.setFramerateLimit(60);

    Menu menu;
    int menuResult = 0; // 0: Menu not yet shown, 1: Start game, -1: Exit game

    while (window.isOpen()) {
        if (menuResult == 0) {
            window.clear(sf::Color::Black);
            menu.draw(window);
            window.display();

            menuResult = menu.handleInput(window);
        }
        else if (menuResult == 1) {
            Maze maze;
            maze.generate();

            Player player(0, 0);

            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed) {
                        int dx = 0, dy = 0;
                        if (event.key.code == sf::Keyboard::Up && !maze.isWall(player.row - 1, player.col, 2))
                            dy = -1;
                        else if (event.key.code == sf::Keyboard::Down && !maze.isWall(player.row + 1, player.col, 0))
                            dy = 1;
                        else if (event.key.code == sf::Keyboard::Left && !maze.isWall(player.row, player.col - 1, 1))
                            dx = -1;
                        else if (event.key.code == sf::Keyboard::Right && !maze.isWall(player.row, player.col + 1, 3))
                            dx = 1;

                        // Check for checkpoint
                        if (maze.isCheckpoint(player.row + dy, player.col + dx)) {
                            std::string question = maze.getQuestion(player.row + dy, player.col + dx);
                            std::cout << "Checkpoint reached! Question: " << question << std::endl;
                            std::pair<std::string, std::string> answerPair = askQuestion(window, menu.getFont(), question);
                            std::string userAnswer = answerPair.first;
                            // Check the user's answer here and proceed accordingly
                            // For demonstration purposes, just print the answer
                            std::cout << "User's Answer: " << userAnswer << std::endl;
                        }

                        player.move(dx, dy);
                    }
                }

                window.clear(sf::Color::Black);
                maze.draw(window);
                player.draw(window);
                window.display();
            }
            menuResult = 0; // Reset menu result after game ends
        }
        else if (menuResult == -1) {
            window.close();
        }
    }

    return 0;
}

