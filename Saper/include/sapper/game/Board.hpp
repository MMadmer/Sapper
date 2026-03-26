#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

#include "sapper/game/Difficulty.hpp"

namespace sapper::game
{
enum class BoardState
{
    Running,
    Won,
    Lost
};

struct CellState
{
    bool hasMine = false;
    bool isOpen = false;
    bool isFlagged = false;
    std::uint8_t adjacentMineCount = 0;
};

class Board
{
public:
    Board();

    void setDifficulty(DifficultyId difficultyId);
    void restart();

    void handleLeftClick(int row, int column);
    void toggleFlag(int row, int column);

    [[nodiscard]] bool isInBounds(int row, int column) const;
    [[nodiscard]] const CellState& cell(int row, int column) const;
    [[nodiscard]] const Difficulty& difficulty() const;
    [[nodiscard]] DifficultyId difficultyId() const;
    [[nodiscard]] BoardState state() const;
    [[nodiscard]] int rows() const;
    [[nodiscard]] int columns() const;
    [[nodiscard]] int mineCount() const;
    [[nodiscard]] int remainingMineCounter() const;

private:
    template <typename Func>
    void forEachNeighbor(int row, int column, Func&& func) const
    {
        static constexpr std::array<std::pair<int, int>, 8> kNeighborOffsets{{
            {-1, 0},
            {1, 0},
            {0, -1},
            {0, 1},
            {-1, -1},
            {-1, 1},
            {1, -1},
            {1, 1},
        }};

        for (const auto [rowOffset, columnOffset] : kNeighborOffsets)
        {
            const int neighborRow = row + rowOffset;
            const int neighborColumn = column + columnOffset;

            if (!isInBounds(neighborRow, neighborColumn))
            {
                continue;
            }

            std::forward<Func>(func)(neighborRow, neighborColumn, indexOf(neighborRow, neighborColumn));
        }
    }

    void placeMines();
    void computeAdjacentMineCounts();
    void revealCell(int row, int column);
    void revealRegionFrom(int startRow, int startColumn);
    void chordCell(int row, int column);
    void updateWinState();
    [[nodiscard]] std::size_t indexOf(int row, int column) const;
    [[nodiscard]] int flaggedNeighborCount(int row, int column) const;

    DifficultyId difficultyId_ = DifficultyId::Beginner;
    std::vector<CellState> cells_;
    BoardState state_ = BoardState::Running;
    int revealedSafeCells_ = 0;
    int flaggedCells_ = 0;
    std::mt19937 randomEngine_;
};
} // namespace sapper::game
