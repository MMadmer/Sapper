#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sapper::render
{
class Assets
{
public:
    Assets();

    [[nodiscard]] const sf::Texture& tileset() const;
    [[nodiscard]] const sf::Font& font() const;

private:
    sf::Texture tileset_;
    sf::Font font_;
};
} // namespace sapper::render
