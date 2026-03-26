#include "sapper/app/GameApp.hpp"

#include <algorithm>
#include <cstdint>
#include <string>

#include <SFML/Window/Mouse.hpp>

namespace sapper::app
{
namespace
{
constexpr unsigned int kWindowWidth = 600;
constexpr unsigned int kBoardAreaHeight = 432;
constexpr unsigned int kHudHeight = 50;
constexpr float kTilesetCellSize = 49.0f;
constexpr float kHudTop = static_cast<float>(kBoardAreaHeight);

constexpr sf::Color kWindowClearColor{16, 30, 48};
constexpr sf::Color kBoardBackgroundColor{33, 63, 99};
constexpr sf::Color kHudBackgroundColor{21, 40, 64};
constexpr sf::Color kTextColor{240, 244, 248};

int numberTileIndex(const std::uint8_t adjacentMineCount)
{
    return 5 + static_cast<int>(adjacentMineCount);
}
} // namespace

GameApp::GameApp()
    : window_(sf::VideoMode({kWindowWidth, kBoardAreaHeight + kHudHeight}), "Sapper")
    , assets_()
    , board_()
    , boardLayout_(makeBoardLayout())
    , tileSprite_(assets_.tileset())
    , mineIconSprite_(assets_.tileset())
    , mineText_(assets_.font(), "", 26)
    , difficultyText_(assets_.font(), "", 18)
    , statusText_(assets_.font(), "", 18)
{
    window_.setFramerateLimit(60);
    window_.setVerticalSyncEnabled(true);

    boardBackground_.setPosition({0.0f, 0.0f});
    boardBackground_.setSize({static_cast<float>(kWindowWidth), static_cast<float>(kBoardAreaHeight)});
    boardBackground_.setFillColor(kBoardBackgroundColor);

    hudBackground_.setPosition({0.0f, kHudTop});
    hudBackground_.setSize({static_cast<float>(kWindowWidth), static_cast<float>(kHudHeight)});
    hudBackground_.setFillColor(kHudBackgroundColor);

    mineIconSprite_.setTextureRect(sf::IntRect({0, 0}, {49, 49}));
    mineIconSprite_.setScale({0.72f, 0.72f});
    mineIconSprite_.setPosition({16.0f, kHudTop + 7.0f});

    for (sf::Text* text : {&mineText_, &difficultyText_, &statusText_})
    {
        text->setFillColor(kTextColor);
    }

    mineText_.setPosition({58.0f, kHudTop + 9.0f});
    difficultyText_.setPosition({148.0f, kHudTop + 13.0f});
    statusText_.setPosition({392.0f, kHudTop + 13.0f});

    updateHud();
}

int GameApp::run()
{
    while (window_.isOpen())
    {
        while (const auto event = window_.pollEvent())
        {
            handleEvent(*event);
        }

        render();
    }

    return 0;
}

void GameApp::handleEvent(const sf::Event& event)
{
    if (event.is<sf::Event::Closed>())
    {
        window_.close();
        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        handleKeyPressed(*keyPressed);
        return;
    }

    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
        handleMouseReleased(*mouseReleased);
    }
}

void GameApp::handleKeyPressed(const sf::Event::KeyPressed& keyPressed)
{
    switch (keyPressed.code)
    {
    case sf::Keyboard::Key::Escape:
        window_.close();
        break;
    case sf::Keyboard::Key::Num1:
        setDifficulty(game::DifficultyId::Beginner);
        break;
    case sf::Keyboard::Key::Num2:
        setDifficulty(game::DifficultyId::Intermediate);
        break;
    case sf::Keyboard::Key::Num3:
        setDifficulty(game::DifficultyId::Expert);
        break;
    default:
        break;
    }
}

void GameApp::handleMouseReleased(const sf::Event::MouseButtonReleased& mouseReleased)
{
    const auto boardCell = cellFromMousePosition(sf::Mouse::getPosition(window_));
    if (!boardCell.has_value())
    {
        return;
    }

    if (mouseReleased.button == sf::Mouse::Button::Left)
    {
        board_.handleLeftClick(boardCell->y, boardCell->x);
        return;
    }

    if (mouseReleased.button == sf::Mouse::Button::Right)
    {
        board_.toggleFlag(boardCell->y, boardCell->x);
    }
}

void GameApp::setDifficulty(const game::DifficultyId difficultyId)
{
    board_.setDifficulty(difficultyId);
    boardLayout_ = makeBoardLayout();
    updateHud();
}

void GameApp::updateHud()
{
    const game::Difficulty& difficulty = board_.difficulty();

    mineText_.setString(std::to_string(board_.remainingMineCounter()));
    difficultyText_.setString(
        std::string(difficulty.label) + "  " +
        std::to_string(difficulty.rows) + "x" + std::to_string(difficulty.columns) + "  " +
        std::to_string(difficulty.mineCount) + " mines");

    switch (board_.state())
    {
    case game::BoardState::Running:
        statusText_.setString("1/2/3 change mode");
        break;
    case game::BoardState::Won:
        statusText_.setString("Victory");
        break;
    case game::BoardState::Lost:
        statusText_.setString("Game over");
        break;
    }
}

void GameApp::render()
{
    updateHud();

    window_.clear(kWindowClearColor);
    window_.draw(boardBackground_);
    drawBoard();
    window_.draw(hudBackground_);
    window_.draw(mineIconSprite_);
    window_.draw(mineText_);
    window_.draw(difficultyText_);
    window_.draw(statusText_);
    window_.display();
}

void GameApp::drawBoard()
{
    tileSprite_.setScale({boardLayout_.cellSize / kTilesetCellSize, boardLayout_.cellSize / kTilesetCellSize});

    for (int row = 0; row < board_.rows(); ++row)
    {
        for (int column = 0; column < board_.columns(); ++column)
        {
            const game::CellState& cellState = board_.cell(row, column);

            tileSprite_.setTextureRect(sf::IntRect({tileIndexFor(cellState) * 49, 0}, {49, 49}));
            tileSprite_.setPosition({
                boardLayout_.origin.x + static_cast<float>(column) * boardLayout_.cellSize,
                boardLayout_.origin.y + static_cast<float>(row) * boardLayout_.cellSize,
            });

            window_.draw(tileSprite_);
        }
    }
}

GameApp::BoardLayout GameApp::makeBoardLayout() const
{
    const game::Difficulty& difficulty = board_.difficulty();
    const float cellSize = std::min(
        static_cast<float>(kWindowWidth) / static_cast<float>(difficulty.columns),
        static_cast<float>(kBoardAreaHeight) / static_cast<float>(difficulty.rows));

    const sf::Vector2f boardSize{
        cellSize * static_cast<float>(difficulty.columns),
        cellSize * static_cast<float>(difficulty.rows),
    };

    return {
        cellSize,
        {
            (static_cast<float>(kWindowWidth) - boardSize.x) * 0.5f,
            (static_cast<float>(kBoardAreaHeight) - boardSize.y) * 0.5f,
        },
        boardSize,
    };
}

std::optional<sf::Vector2i> GameApp::cellFromMousePosition(const sf::Vector2i mousePosition) const
{
    const float mouseX = static_cast<float>(mousePosition.x);
    const float mouseY = static_cast<float>(mousePosition.y);

    if (mouseX < boardLayout_.origin.x || mouseY < boardLayout_.origin.y)
    {
        return std::nullopt;
    }

    if (mouseX >= boardLayout_.origin.x + boardLayout_.size.x ||
        mouseY >= boardLayout_.origin.y + boardLayout_.size.y)
    {
        return std::nullopt;
    }

    const int column = static_cast<int>((mouseX - boardLayout_.origin.x) / boardLayout_.cellSize);
    const int row = static_cast<int>((mouseY - boardLayout_.origin.y) / boardLayout_.cellSize);

    if (!board_.isInBounds(row, column))
    {
        return std::nullopt;
    }

    return sf::Vector2i{column, row};
}

int GameApp::tileIndexFor(const game::CellState& cellState) const
{
    if (board_.state() == game::BoardState::Won && cellState.hasMine)
    {
        return 0;
    }

    if (cellState.isFlagged)
    {
        if (board_.state() == game::BoardState::Lost && !cellState.hasMine)
        {
            return 2;
        }

        return 3;
    }

    if (board_.state() == game::BoardState::Lost)
    {
        if (cellState.hasMine)
        {
            return 1;
        }

        return numberTileIndex(cellState.adjacentMineCount);
    }

    if (!cellState.isOpen)
    {
        return 4;
    }

    if (cellState.hasMine)
    {
        return 0;
    }

    return numberTileIndex(cellState.adjacentMineCount);
}
} // namespace sapper::app
