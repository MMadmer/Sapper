#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma warning(disable: 4244)

#include "globals.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

#include "cell.h"

std::vector<Cell*> CurrentCell;
std::vector<Cell*>::iterator It;

int WindowHeight = 432;
int WindowWidth = 432;
int Rows = 9; // Field rows
int Columns = 9; // Field columns
bool LeftPressed = false;
bool RightPressed = false;

int Map[16][30];

int GetRandomNumber(int Min, int Max);
void Game();
int ChooseMap();
void CloseCells(int I);
void NewGame(const int& Mines, int Chance);
void OpenCells(int I);
void AutoOpenCells(int Xm, int Ym, bool Z);
void CountFlagsAbout(int I, int P);
void DrawField(sf::RenderWindow& Window);
void SetWindowBackground(sf::RenderWindow& Window);
void GridDraw(sf::RenderWindow& Window);
void DrawCells(sf::RenderWindow& Window);

sf::Font GameFont;

int main()
{
    GameFont.loadFromFile("assets/Context_Reprise_SSi.ttf");

    for (int i = 16 * 30; i > 0; i--)
    {
        CurrentCell.push_back(new Cell());
    }

    Game();

    return 0;
}

void Game()
{
    int X = 39;
    int Y = 19;
    sf::Event Event;
    Cell Icon; // Mines count icon

    Icon.Decorate = true;
    Icon.DrawCells();
    Icon.Sprite.setPosition(static_cast<float>(WindowWidth) / 2.0f - 25.0f, static_cast<float>(WindowHeight) + 10);

    // Game window creating
    sf::RenderWindow Window(sf::VideoMode(WindowWidth, WindowHeight + 50), "Sapper");
    sf::Text GameText("", GameFont, 25);
    GameText.setPosition(static_cast<float>(WindowWidth) / 2.0f + 5.0f, static_cast<float>(WindowHeight) + 8.0f);

    Window.setFramerateLimit(30);
    Window.setVerticalSyncEnabled(true);

    while (Window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        // Bind processing
        while (Window.pollEvent(Event))
        {
            if (Event.type == sf::Event::MouseButtonReleased)
            {
                if (Event.key.code == sf::Mouse::Right) RightPressed = true;
                if (Event.key.code == sf::Mouse::Left) LeftPressed = true;
            }
            if (Event.type == sf::Event::Closed)
                Window.close();
        }

        // Text
        std::ostringstream StringOfMines;
        StringOfMines << GMaxMines;
        GameText.setString(StringOfMines.str());

        if (GNowNumberMap == 1 && GLastNumberMap == 3)
        {
            GLastNumberMap = 1;
            Window.close();
            Game();
        }
        if (GNowNumberMap == 2 && GLastNumberMap == 3)
        {
            GLastNumberMap = 2;
            Window.close();
            Game();
        }
        if (GNowNumberMap == 3 && GLastNumberMap < 3)
        {
            GLastNumberMap = 3;
            Window.close();
            Game();
        }

        // Set random
        srand(static_cast<unsigned int>(time(nullptr)));
        // Choose and create map
        GNowNumberMap = ChooseMap();

        DrawField(Window);

        Window.draw(GameText);
        Window.draw(Icon.Sprite);

        Window.display();
    }
}

//////////////////////////////////////////////////////////////////////////////
// Draw field REGION BEGIN
//////////////////////////////////////////////////////////////////////////////
void DrawField(sf::RenderWindow& Window)
{
    // Create background
    SetWindowBackground(Window);

    if (GWin == false)
    {
        if (GVoidCells == GClickedVoidCells)
        {
            GWin = true;
            GMaxMines = 0;
        }
    }

    DrawCells(Window);
    GridDraw(Window);
}

void SetWindowBackground(sf::RenderWindow& Window)
{
    It = CurrentCell.begin();
    Window.clear(sf::Color::Blue);
}

void GridDraw(sf::RenderWindow& Window)
{
    for (int i = WindowWidth / Columns; i < WindowWidth; i += WindowWidth / Columns)
    {
        const sf::Vertex Line[] =
        {
            sf::Vertex(sf::Vector2f(i, 0)),
            sf::Vertex(sf::Vector2f(i, WindowHeight))
        };
        Window.draw(Line, 2, sf::Lines);
    }
    for (int i = WindowHeight / Rows; i < WindowHeight; i += WindowHeight / Rows)
    {
        const sf::Vertex Line[] =
        {
            sf::Vertex(sf::Vector2f(0, i)),
            sf::Vertex(sf::Vector2f(WindowWidth, i))
        };
        Window.draw(Line, 2, sf::Lines);
    }
}

void DrawCells(sf::RenderWindow& Window)
{
    for (int Ym = 0; Ym < Rows; Ym++)
        for (int Xm = 0; Xm < Columns; Xm++)
        {
            if (GWin) (*It)->Flag = false;

            // Open cells around already opened cell
            if ((*It)->NumberCell == 0 && (*It)->Opened && (*It)->Used == false) AutoOpenCells(Xm, Ym, true);

            // Change cell
            (*It)->SetCellParameters(Xm * (WindowWidth / Columns), Ym * (WindowHeight / Rows));

            // Right mouse button bind(flags)
            if (RightPressed)
            {
                const sf::Vector2i PixelPos = sf::Mouse::getPosition(Window); // Нахождение координат курсора

                if ((*It)->Sprite.getGlobalBounds().contains(PixelPos.x, PixelPos.y))
                {
                    // Set flag
                    if ((*It)->Flag == false && (*It)->Opened == false && RightPressed && GWin == false && GMaxMines >
                        0)
                    {
                        (*It)->Flag = true;
                        RightPressed = false;
                        GMaxMines--;

                        // Increase flags count around this cell
                        if (Ym > 0) CountFlagsAbout(-Columns, 1);
                        if (Ym < Rows - 1) CountFlagsAbout(Columns, 1);
                        if (Xm > 0) CountFlagsAbout(-1, 1);
                        if (Xm < Columns - 1) CountFlagsAbout(1, 1);
                        if (Ym > 0 && Xm > 0) CountFlagsAbout(-Columns - 1, 1);
                        if (Ym > 0 && Xm < Columns - 1) CountFlagsAbout(-Columns + 1, 1);
                        if (Ym < Rows - 1 && Xm > 0) CountFlagsAbout(Columns - 1, 1);
                        if (Ym < Rows - 1 && Xm < Columns - 1) CountFlagsAbout(Columns + 1, 1);
                    }
                    // Remove flag
                    if ((*It)->Flag && (*It)->Opened == false && RightPressed && GWin == false)
                    {
                        (*It)->Flag = false;
                        RightPressed = false;
                        GMaxMines++;

                        // Decrease flags count around this cell
                        if (Ym > 0) CountFlagsAbout(-Columns, -1);
                        if (Ym < Rows - 1) CountFlagsAbout(Columns, -1);
                        if (Xm > 0) CountFlagsAbout(-1, -1);
                        if (Xm < Columns - 1) CountFlagsAbout(1, -1);
                        if (Ym > 0 && Xm > 0) CountFlagsAbout(-Columns - 1, -1);
                        if (Ym > 0 && Xm < Columns - 1) CountFlagsAbout(-Columns + 1, -1);
                        if (Ym < Rows - 1 && Xm > 0) CountFlagsAbout(Columns - 1, -1);
                        if (Ym < Rows - 1 && Xm < Columns - 1) CountFlagsAbout(Columns + 1, -1);
                    }
                    RightPressed = false;
                }
            }
            // Left mouse button bind(open)
            if (LeftPressed)
            {
                const sf::Vector2i PixelPos = sf::Mouse::getPosition(Window); // Get cursor coordinates

                if ((*It)->Sprite.getGlobalBounds().contains(PixelPos.x, PixelPos.y))
                {
                    if (GWin == false && (*It)->Flag == false)
                    {
                        if ((*It)->Opened && (*It)->NumberCell > 0 && (*It)->NumberCell == (*It)->FlagsAbout)
                            AutoOpenCells(Xm, Ym, false);

                        // Lose and open all cells if mine
                        if ((*It)->IsMine) GFail = true;
                        else if ((*It)->Opened == false && (*It)->IsMine == false)
                        {
                            // If it's not a mine, the cell is definitely empty and can be considered
                            (*It)->Opened = true;
                            GClickedVoidCells++;
                        }
                    }
                    LeftPressed = false;
                }
            }

            (*It)->DrawCells();
            Window.draw((*It)->Sprite);

            ++It;
        }
}

//////////////////////////////////////////////////////////////////////////////
// Draw field REGION END
//////////////////////////////////////////////////////////////////////////////

int ChooseMap()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
    {
        Rows = Columns = 9;
        GMaxMines = 10;
        WindowHeight = 432;
        WindowWidth = 432;

        NewGame(GMaxMines, 7);

        return 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
    {
        Rows = Columns = 16;
        GMaxMines = 40;
        WindowHeight = 432;
        WindowWidth = 432;

        NewGame(GMaxMines, 7);

        return 2;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
    {
        Rows = 16;
        Columns = 30;
        GMaxMines = 99;
        WindowHeight = 432;
        WindowWidth = 600;

        NewGame(GMaxMines, 5);

        return 3;
    }

    return 0;
}

void CloseCells(int I)
{
    It = CurrentCell.begin();
    for (; I > 0; I--)
    {
        (*It)->Opened = false;
        (*It)->Flag = false;
        (*It)->Used = false;
        (*It)->FlagsAbout = 0;
        (*It)->NumberCell = 0;
        ++It;
    }
    It = CurrentCell.begin();
}

void OpenCells(const int I)
{
    advance(It, I);
    if ((*It)->Flag == false)
    {
        if ((*It)->Opened == false)
        {
            (*It)->Opened = true;
            if ((*It)->IsMine) GFail = true;
            else GClickedVoidCells++;
        }
    }
    advance(It, -I);
}

void AutoOpenCells(const int Xm, const int Ym, const bool Z)
{
    if (Ym > 0) OpenCells(-Columns); // Open up cell
    if (Ym < Rows - 1) OpenCells(Columns); // Open down cell
    if (Xm > 0) OpenCells(-1); // Open left cell
    if (Xm < Columns - 1) OpenCells(1); // Open right cell
    if (Ym > 0 && Xm > 0) OpenCells(-Columns - 1); // Open up-left cell
    if (Ym > 0 && Xm < Columns - 1) OpenCells(-Columns + 1); // Open up-right cell
    if (Ym < Rows - 1 && Xm > 0) OpenCells(Columns - 1); // Open down-left cell
    if (Ym < Rows - 1 && Xm < Columns - 1) OpenCells(Columns + 1); // Open down-right cell

    if (Z)(*It)->Used = true;
}

void CountFlagsAbout(const int I, const int P)
{
    advance(It, I);
    (*It)->FlagsAbout += P;
    advance(It, -I);
}

void NewGame(const int& Mines, const int Chance)
{
    int MinesCount = 0;
    int I = 0;
    GWin = false;
    GFail = false;
    GVoidCells = Rows * Columns - Mines;
    GClickedVoidCells = 0;

    CloseCells(Rows * Columns);

    for (auto& Ym : Map)
    {
        for (int& Xm : Ym)
        {
            Xm = 0;
        }
    }

    do
    {
        for (int Ym = 0; Ym < Rows; Ym++)
        {
            for (int Xm = 0; Xm < Columns; Xm++)
            {
                if (Map[Ym][Xm] == 1) break;

                I = GetRandomNumber(0, Chance);
                if (I == 1 && MinesCount < Mines)
                {
                    Map[Ym][Xm] = 1;
                    MinesCount++;
                }
            }
        }
    }
    while (MinesCount < Mines);

    int Count = 0;
    It = CurrentCell.begin();
    for (int Ym = 0; Ym < Rows; Ym++)
        for (int Xm = 0; Xm < Columns; Xm++)
        {
            // Find mines around
            if (Map[Ym][Xm] == 0)
            {
                (*It)->IsMine = false;

                // Check if mine up
                if (Ym > 0 && Map[Ym - 1][Xm] == 1) (*It)->NumberCell++;
                // Check if mine down
                if (Ym < (Rows - 1) && Map[Ym + 1][Xm] == 1) (*It)->NumberCell++;
                // Check if mine left
                if (Xm > 0 && Map[Ym][Xm - 1] == 1) (*It)->NumberCell++;
                // Check if mine right
                if (Xm < Columns - 1 && Map[Ym][Xm + 1] == 1) (*It)->NumberCell++;
                // Check if mine up-left
                if (Ym > 0 && Xm > 0 && Map[Ym - 1][Xm - 1] == 1) (*It)->NumberCell++;
                // Check if mine up-right
                if (Ym > 0 && Xm < Columns - 1 && Map[Ym - 1][Xm + 1] == 1) (*It)->NumberCell++;
                // Check if mine down-left
                if (Ym < Rows - 1 && Xm > 0 && Map[Ym + 1][Xm - 1] == 1) (*It)->NumberCell++;
                // Check if mine down-right
                if (Ym < Rows - 1 && Xm < Columns - 1 && Map[Ym + 1][Xm + 1] == 1) (*It)->NumberCell++;
            }
            else
            {
                (*It)->IsMine = true;
                Count++;
            }
            ++It;
        }
    It = CurrentCell.begin();
}

int GetRandomNumber(const int Min, const int Max)
{
    static constexpr double Fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<int>(rand() * Fraction * (Max - Min + 1) + Min);
}
