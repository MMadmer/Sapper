#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

class Cell
{
public:
    sf::Texture Tileset;
    sf::Sprite Sprite;
    bool Opened = false;
    bool IsMine = false;
    bool Flag = false;
    bool Used = false;
    int FlagsAbout = 0;
    int NumberCell = 0;
    bool Decorate = false;
    int Miss = false;

    Cell();

    void SetCellParameters(int X, int Y);
    void DrawCells();
    void SetTextureCell(int X);
};
