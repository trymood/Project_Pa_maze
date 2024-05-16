
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
    const std::vector<Cell>& getCells() const;
    int getIndex(int row, int col) const;

private:
    std::vector<Cell> cells;

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

    // Default constructor
    Button() {}

    void draw(sf::RenderWindow& window) {
        window.draw(m_rect);
        window.draw(m_text);
    }

    bool isClicked(const sf::Vector2f& mousePos) {
        return m_rect.getGlobalBounds().contains(mousePos);
    }

public:
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
    Question("What is the definition of green / sustainable energy in comparison with renewable or clean energy ?\n\n 1. Sustainable energy includes any energy sourcewith little or no GHG emissions, that cannot be depleted and can remain viable forever; in comparison renewable energy,is exhaustible it uses resources from the earth that can naturally be replenished, but determine less GHG emissions, and clean energy exhaustible, but without GHG emissions\n 2. All these energy sources are without or with less GHG emissions, but Green / sustainable energy is exhaustible; by comparison Renewable energy sources and Clean energy sources are notexhaustible.\n 3. All these energy sources harm the environment by GHG emissions, but Green/ sustainable energy represents any energy so urce that cannot be depleted; in comparison Renewable energy can be depleted with natural system of regeneration, and Clean energy are exhaustible.\n 4. All these energy sources produce less or no GHG emissions, but Green / sustainable energy can be depleted, by comparison with the other two energy sources that can remain for ever.\n\n Choose the correct answer/s by leaving an empty space in between:  ", "1"),
    Question("What are the EU Commission objectives regarding green / sustainable energy?\n\n 1. The EU Commission objectives regarding green / sustainable energy are to totally replace thefossil fuels with wind and solarenergy by 2035.\n 2. The EU Commission objectives regarding green / sustainable energy comprise the totalbanning of fossil fuels and atomic energy and use of renewable sources of energy by 2040.\n 3. The EU Commission objectives regarding green / sustainable energy are toincrease the Energyefficiency in any application, todevelopalltherenewable energy sources bylargeinvestmentexpenses and to diminish with a high speed the fossil fuels energy sources\n 4. The EU Commission objectives regarding green / sustainable energyare to increase the Energyefficiency in all the technological applications, and to replace the fossil fuels use to generate energyby green/ sustainable sources, including any renewable sources\n\n Choose the correct answer/s by leaving an empty space in between:", "3 4"),
    Question("What are the most important characteristics of renewable energy ?\n\n 1. The most important characteristics of renewable energy comprise : limited power source, not providing from natural resources, no or less GHG emissions, important waste generation, low investment costs.\n 2. The most important characteristics of renewable energy are the high carbon Footprint, high GHG emissions, important impact on pollution.\n 3. The most important characteristics of renewable energy can be considered : decrease of carbon Footprint, decrease of GHG emissions, energy security, improving of public health, positive impacton pollution, but high investment costs, long-term affordable.\n 4. The most important characteristcs of renewable energy are : the energy sources are exhaustible, without natural systems to be replenished, but with less GHG emissions, so with positive impact on pollution.\n\n Choose the correct answer/s by leaving an empty space in between:", "3"),
    Question("What are the limitations of wind energy technology ?\n\n 1. The wind energy technology has no limitations from the economical, technological or environmental points of view.\n 2. The wind energy technology has high costs of converting wind energy into electric energy, it is not technological ready for large applications, and it is not affordable in many places.\n 3. The wind technology has several important limitations : the reliability and lifetime of wind turbines, noise pollution, birds’ collision,ice accumulation.\n 4. The wind energy technology limited due to raining or snowing periods, and not accepted by communities with strong traditional behavior.\n\n Choose the correct answer/s by leaving an empty space in between:", "3"),
    Question("Can hydropower (hydroelectric energy) be considered as a Green / sustainable energy source ?\n\n 1. The hydropower is one of the main sources to replace the fossil fuel energy , so it can be considered as Green / sustainable energy source.\n 2. The hydropower is not a Sustainable energy source, but it is the second largest Renewable energy source.\n 3. Hydropower is fueled by water, making it a clean source of energy, without GHG emissions, but it is not sustainable, nor renewable.\n 4. Hydroelectric energy is a form of renewable energy that uses the power of moving water to generate electricity , but it is not sustainable or green.\n\n Choose the correct answer/s by leaving an empty space in between:", "2 4"),
    Question("What are the main characteristics of a biofuel ?\n\n 1.Biofuel is considered any fuel derived from biomass, being a source of renewable energy, environmentally benign, but with economic and environmental costs associated with the refining process.\n 2.Biofuel is considered any fuel derived from natural sources, so it can be considered as green / sustainable energy.\n 3.Biofuel is any fuel withless greenhouse gases(GHG) emissions, derived from practically not replenished sources.\n 4.Biofuel is considered as sustainable energy source, representing any fuel providing from cost-effective biomass transformation, with less GHG emissions.\n\n Choose the correct answer/s by leaving an empty space in between:", "1"),
    Question("What is the definition of green hydrogen?\n\n 1. Green hydrogen is the hydrogen produced by the electrolysis of water, using electricity generated from any source of energy.\n 2. Green hydrogen is the hydrogen accumulated in natural reservoirs under the Earth mantel.\n 3. Green hydrogen is hydrogen produced by the electrolysis of water , using electricity generated only from renewable sources.\n 4. Green hydrogen is the hydrogen produced by the electrolysis of water, and theCO2 emissions due to the use of electricity generated with fossil fuels are subject to underground storage.\n\n Choose the correct answer/s by leaving an empty space in between: ", "3")
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

    std::vector<std::pair<int, int>> checkpointPositions = { {8,22}, {13,15}, {7,0}, {15,7}, {29,11}, {21,39}, {22,27} };
    for (const auto& pos : checkpointPositions) {
        int row = pos.first;
        int col = pos.second;
        cells[getIndex(row, col)].checkpoint = true;
    }
}

void Maze::draw(sf::RenderWindow& window) {
    // Calculate the cell size based on the window dimensions and number of rows/columns
    float cellSizeX = static_cast<float>(window.getSize().x - 2 * BORDER_SIZE) / COLS;
    float cellSizeY = static_cast<float>(window.getSize().y - 2 * BORDER_SIZE) / ROWS;

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

        if (cells[i].row == ROWS - 1 && cells[i].col == COLS - 1) {
            sf::RectangleShape exit;
            exit.setSize(sf::Vector2f(cellSizeX, cellSizeY));
            exit.setPosition(x, y);
            exit.setFillColor(sf::Color::Red); // Set exit cell color to red
            window.draw(exit);
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

const std::vector<Cell>& Maze::getCells() const {
    return cells;
}

int Maze::getIndex(int row, int col) const {
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
    float cellSizeX = static_cast<float>(window.getSize().x - 2 * BORDER_SIZE) / COLS;
    float cellSizeY = static_cast<float>(window.getSize().y - 2 * BORDER_SIZE) / ROWS;
    float radius = std::min(cellSizeX, cellSizeY) / 3;

    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(col * cellSizeX + BORDER_SIZE + cellSizeX / 2 - radius, row * cellSizeY + BORDER_SIZE + cellSizeY / 2 - radius);

    window.draw(circle);
}

Menu::Menu() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
    }

    float buttonWidth = desktop.width / 4;
    float buttonHeight = desktop.height / 12;
    float buttonX = (desktop.width - buttonWidth) / 2;
    float startButtonY = desktop.height / 3 - buttonHeight / 2;
    float exitButtonY = desktop.height / 2 + buttonHeight / 2;

    startButton = Button(buttonX, startButtonY, buttonWidth, buttonHeight, "Start Game", font);
    exitButton = Button(buttonX, exitButtonY, buttonWidth, buttonHeight, "Exit", font); // Adjusted text

    // Adjusting text position for both buttons
    sf::FloatRect startTextRect = startButton.m_text.getLocalBounds();
    startButton.m_text.setOrigin(startTextRect.left + startTextRect.width / 2.0f, startTextRect.top + startTextRect.height / 2.0f);
    startButton.m_text.setPosition(buttonX + buttonWidth / 2, startButtonY + buttonHeight / 2);

    sf::FloatRect exitTextRect = exitButton.m_text.getLocalBounds();
    exitButton.m_text.setOrigin(exitTextRect.left + exitTextRect.width / 2.0f, exitTextRect.top + exitTextRect.height / 2.0f);
    exitButton.m_text.setPosition(buttonX + buttonWidth / 2, exitButtonY + buttonHeight / 2);

    title.setFont(font);
    title.setString("Theseus");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::White);
    title.setPosition((desktop.width - title.getLocalBounds().width) / 2, desktop.height / 6);
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
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Maze Game", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    Menu menu;
    Maze maze;
    Player player(0, 0);

    bool gameStarted = false;
    bool gameWon = false;

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

                if (maze.isCheckpoint(player.row, player.col))
                {
                    int questionIndex = rand() % questions.size();
                    std::string question = questions[questionIndex].getQuestion();
                    std::string answer;
                    bool answerEntered = false;

                    sf::RectangleShape questionBox(sf::Vector2f(window.getSize().x, window.getSize().y));
                    questionBox.setFillColor(sf::Color(0, 0, 0, 200)); // Semi-transparent black
                    sf::Text questionText(question, menu.getFont(), 30);
                    questionText.setFillColor(sf::Color::White);
                    questionText.setPosition((window.getSize().x - questionText.getLocalBounds().width) / 2, (window.getSize().y - questionText.getLocalBounds().height) / 2);

                    sf::Text answerText("", menu.getFont(), 30);
                    answerText.setFillColor(sf::Color::White);
                    answerText.setPosition((window.getSize().x - answerText.getLocalBounds().width) / 2, (window.getSize().y - answerText.getLocalBounds().height) / 2 + 50);


                    while (window.isOpen() && !answerEntered) {
                        sf::Event answerEvent;
                        while (window.pollEvent(answerEvent)) {
                            if (answerEvent.type == sf::Event::TextEntered) {
                                if (answerEvent.text.unicode == 13) { // Enter key pressed
                                    answerEntered = true;
                                    break;
                                }
                                else if (answerEvent.text.unicode == 8) { // Backspace key pressed
                                    if (!answer.empty()) {
                                        answer.pop_back();
                                    }
                                }
                                else if (answerEvent.text.unicode < 128) {
                                    answer += static_cast<char>(answerEvent.text.unicode);
                                }
                                answerText.setString(answer);
                            }
                        }

                        window.clear();
                        maze.draw(window);
                        player.draw(window);
                        window.draw(questionBox);

                        // Adjust font size dynamically based on window dimensions
                        float fontSize = std::min(window.getSize().x / 40, window.getSize().y / 30); // Adjust divisor for different aspect ratios
                        sf::Text questionText(question, menu.getFont(), static_cast<unsigned int>(fontSize));
                        questionText.setFillColor(sf::Color::White);

                        // Position the question text with space between left and right sides
                        float padding = 20.0f; // Adjust as needed
                        float maxTextWidth = window.getSize().x - 2 * padding; // Calculate maximum text width
                        questionText.setPosition(padding, padding);
                        questionText.setOrigin(0, 0);
                        questionText.setScale(1.0f, 1.0f); // Reset scale before setting new scale
                        if (questionText.getLocalBounds().width > maxTextWidth) {
                            float scaleFactor = maxTextWidth / questionText.getLocalBounds().width;
                            questionText.setScale(scaleFactor, scaleFactor);
                        }

                        window.draw(questionText);
                        window.draw(answerText);
                        window.display();
                    }

                    if (questions[questionIndex].checkAnswer(answer)) {
                        maze.removeCheckpoint(player.row, player.col);
                        std::cout << "Correct!\n";
                    }
                    else {
                        std::cout << "Wrong! Try again later.\n";
                        player.row = 0;
                        player.col = 0;
                    }
                }

                // Check if the player reached the exit
                if (player.row == ROWS - 1 && player.col == COLS - 1) {
                    gameWon = true;
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
            if (gameWon) {
                sf::Font font;
                if (font.loadFromFile("arial.ttf")) {
                    sf::Text congratulations("Congratulations! You won!", font, 50);

                    // Adjust font size dynamically based on window dimensions
                    float fontSize = window.getSize().x / 20; // Adjust divisor for different aspect ratios
                    congratulations.setCharacterSize(static_cast<unsigned int>(fontSize));

                    congratulations.setFillColor(sf::Color::Green);

                    // Center the text in the window
                    sf::FloatRect textRect = congratulations.getLocalBounds();
                    congratulations.setOrigin(textRect.left + textRect.width / 2.0f,
                        textRect.top + textRect.height / 2.0f);
                    congratulations.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

                    window.draw(congratulations);
                }
                else {
                    std::cerr << "Error loading font\n";
                }
            }
        }
        window.display();
    }


    return 0;
}
