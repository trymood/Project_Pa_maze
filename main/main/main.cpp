#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <stack>
#include <unordered_set>

const int CELL_SIZE = 20; // Size of each cell in pixels
const int WIDTH = 800;    // Width of the window
const int HEIGHT = 600;   // Height of the window
const int ROWS = HEIGHT / CELL_SIZE;
const int COLS = WIDTH / CELL_SIZE;

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
}

void Maze::draw(sf::RenderWindow& window) {
    for (int i = 0; i < cells.size(); ++i) {
        int x = cells[i].col * CELL_SIZE;
        int y = cells[i].row * CELL_SIZE;

        if (cells[i].walls[0]) { // top
            sf::RectangleShape topWall(sf::Vector2f(CELL_SIZE, 1));
            topWall.setPosition(x, y);
            window.draw(topWall);
        }

        if (cells[i].walls[1]) { // right
            sf::RectangleShape rightWall(sf::Vector2f(1, CELL_SIZE));
            rightWall.setPosition(x + CELL_SIZE, y);
            window.draw(rightWall);
        }

        if (cells[i].walls[2]) { // bottom
            sf::RectangleShape bottomWall(sf::Vector2f(CELL_SIZE, 1));
            bottomWall.setPosition(x, y + CELL_SIZE);
            window.draw(bottomWall);
        }

        if (cells[i].walls[3]) { // left
            sf::RectangleShape leftWall(sf::Vector2f(1, CELL_SIZE));
            leftWall.setPosition(x, y);
            window.draw(leftWall);
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
    sf::CircleShape playerShape(CELL_SIZE / 2);
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition(col * CELL_SIZE + CELL_SIZE / 4, row * CELL_SIZE + CELL_SIZE / 4);
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
