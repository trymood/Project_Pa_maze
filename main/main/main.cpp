#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cstdlib> // For rand()

const int WIDTH = 800;    // Width of the window
const int HEIGHT = 600;   // Height of the window
const int ROWS = 30;      // Number of rows in the maze
const int COLS = 40;      // Number of columns in the maze
const int BORDER_SIZE = 5; // Size of the border around the maze

class Cell {
public:
    int row, col;
    bool visited;
    bool walls[4]; // top, right, bottom, left
    std::vector<Cell*> neighbors;

    Cell(int r, int c) : row(r), col(c), visited(false) {
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
public:
    int row, col;
};

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

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze");
    window.setFramerateLimit(60);

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
                player.move(dx, dy);
            }
        }

        window.clear(sf::Color::Black);
        maze.draw(window);
        player.draw(window);
        window.display();
    }

    return 0;
}





