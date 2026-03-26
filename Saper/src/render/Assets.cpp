#include "sapper/render/Assets.hpp"

#include <stdexcept>

namespace sapper::render
{
Assets::Assets()
{
    if (!tileset_.loadFromFile("assets/tileset.png"))
    {
        throw std::runtime_error("Failed to load assets/tileset.png");
    }

    if (!font_.openFromFile("assets/Context_Reprise_SSi.ttf"))
    {
        throw std::runtime_error("Failed to load assets/Context_Reprise_SSi.ttf");
    }
}

const sf::Texture& Assets::tileset() const
{
    return tileset_;
}

const sf::Font& Assets::font() const
{
    return font_;
}
} // namespace sapper::render
