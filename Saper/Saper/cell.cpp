#pragma warning(disable: 4244)

#include "cell.h"

#include "globals.h"

Cell::Cell()
{
    Tileset.loadFromFile("assets/tileset.png");
}

void Cell::SetCellParameters(const int X, const int Y)
{
    if (GFail) Opened = true;
    Sprite.setPosition(X, Y);
}

void Cell::DrawCells()
{
    if (Decorate) SetTextureCell(0);
    else
    {
        if (Flag)
        {
            if (IsMine == false && GFail)
            {
                SetTextureCell(2);
                if (Miss == false) GMaxMines++;
                Miss = true;
            }
            else SetTextureCell(3);
        }
        else
        {
            if (!Opened) SetTextureCell(4);
            if (IsMine && !GFail && (Opened || GWin)) SetTextureCell(0); // If mine
            else if (IsMine && GFail) SetTextureCell(1); // If exploded mine
            else if (IsMine == false && NumberCell == 1 && Opened) SetTextureCell(6); // Show count of mines around
            else if (IsMine == false && NumberCell == 2 && Opened) SetTextureCell(7);
            else if (IsMine == false && NumberCell == 3 && Opened) SetTextureCell(8);
            else if (IsMine == false && NumberCell == 4 && Opened) SetTextureCell(9);
            else if (IsMine == false && NumberCell == 5 && Opened) SetTextureCell(10);
            else if (IsMine == false && NumberCell == 6 && Opened) SetTextureCell(11);
            else if (IsMine == false && NumberCell == 7 && Opened) SetTextureCell(12);
            else if (IsMine == false && NumberCell == 8 && Opened) SetTextureCell(13);
            else if (IsMine == false && NumberCell == 0 && Opened) SetTextureCell(5);
        }
    }
}

void Cell::SetTextureCell(const int X)
{
    if (Decorate) Sprite.setScale(0.55f, 0.55f);
    else
    {
        if (GNowNumberMap == 1) Sprite.setScale(1.0f, 1.01f);
        if (GNowNumberMap == 2) Sprite.setScale(0.58f, 0.561f);
        if (GNowNumberMap == 3) Sprite.setScale(0.445f, 0.551f);
    }
    Sprite.setTexture(Tileset);
    Sprite.setTextureRect(sf::IntRect(X * 49, 0, 49, 49));
}
