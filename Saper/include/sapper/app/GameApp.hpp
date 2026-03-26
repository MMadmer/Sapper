#pragma once

#include <optional>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "sapper/game/Board.hpp"
#include "sapper/render/Assets.hpp"

namespace sapper::app
{
class GameApp
{
public:
    GameApp();

    int run();

private:
    struct BoardLayout
    {
        float cellSize = 0.0f;
        sf::Vector2f origin{};
        sf::Vector2f size{};
    };

    void handleEvent(const sf::Event& event);
    void handleKeyPressed(const sf::Event::KeyPressed& keyPressed);
    void handleMouseReleased(const sf::Event::MouseButtonReleased& mouseReleased);
    void setDifficulty(game::DifficultyId difficultyId);
    void updateHud();
    void render();
    void drawBoard();
    [[nodiscard]] BoardLayout makeBoardLayout() const;
    [[nodiscard]] std::optional<sf::Vector2i> cellFromMousePosition(sf::Vector2i mousePosition) const;
    [[nodiscard]] int tileIndexFor(const game::CellState& cellState) const;

    sf::RenderWindow window_;
    render::Assets assets_;
    game::Board board_;
    BoardLayout boardLayout_;
    sf::Sprite tileSprite_;
    sf::Sprite mineIconSprite_;
    sf::Text mineText_;
    sf::Text difficultyText_;
    sf::Text statusText_;
    sf::RectangleShape boardBackground_;
    sf::RectangleShape hudBackground_;
};
} // namespace sapper::app
