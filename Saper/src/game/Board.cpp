#include "sapper/game/Board.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

namespace sapper::game
{
Board::Board()
    : randomEngine_(std::random_device{}())
{
    restart();
}

void Board::setDifficulty(const DifficultyId difficultyId)
{
    difficultyId_ = difficultyId;
    restart();
}

void Board::restart()
{
    cells_.assign(static_cast<std::size_t>(rows() * columns()), {});
    state_ = BoardState::Running;
    revealedSafeCells_ = 0;
    flaggedCells_ = 0;

    placeMines();
    computeAdjacentMineCounts();
}

void Board::handleLeftClick(const int row, const int column)
{
    if (!isInBounds(row, column) || state_ != BoardState::Running)
    {
        return;
    }

    const CellState& selectedCell = cell(row, column);
    if (selectedCell.isFlagged)
    {
        return;
    }

    if (selectedCell.isOpen)
    {
        chordCell(row, column);
        return;
    }

    revealCell(row, column);
}

void Board::toggleFlag(const int row, const int column)
{
    if (!isInBounds(row, column) || state_ != BoardState::Running)
    {
        return;
    }

    CellState& selectedCell = cells_[indexOf(row, column)];
    if (selectedCell.isOpen)
    {
        return;
    }

    if (selectedCell.isFlagged)
    {
        selectedCell.isFlagged = false;
        --flaggedCells_;
        return;
    }

    if (flaggedCells_ >= mineCount())
    {
        return;
    }

    selectedCell.isFlagged = true;
    ++flaggedCells_;
}

bool Board::isInBounds(const int row, const int column) const
{
    return row >= 0 && row < rows() && column >= 0 && column < columns();
}

const CellState& Board::cell(const int row, const int column) const
{
    return cells_[indexOf(row, column)];
}

const Difficulty& Board::difficulty() const
{
    return difficultyFor(difficultyId_);
}

DifficultyId Board::difficultyId() const
{
    return difficultyId_;
}

BoardState Board::state() const
{
    return state_;
}

int Board::rows() const
{
    return difficulty().rows;
}

int Board::columns() const
{
    return difficulty().columns;
}

int Board::mineCount() const
{
    return difficulty().mineCount;
}

int Board::remainingMineCounter() const
{
    if (state_ == BoardState::Won)
    {
        return 0;
    }

    return mineCount() - flaggedCells_;
}

void Board::placeMines()
{
    std::vector<std::size_t> positions(cells_.size());
    std::iota(positions.begin(), positions.end(), 0);
    std::shuffle(positions.begin(), positions.end(), randomEngine_);

    for (int mineIndex = 0; mineIndex < mineCount(); ++mineIndex)
    {
        cells_[positions[static_cast<std::size_t>(mineIndex)]].hasMine = true;
    }
}

void Board::computeAdjacentMineCounts()
{
    for (int row = 0; row < rows(); ++row)
    {
        for (int column = 0; column < columns(); ++column)
        {
            CellState& currentCell = cells_[indexOf(row, column)];
            if (currentCell.hasMine)
            {
                continue;
            }

            int adjacentMineCount = 0;
            forEachNeighbor(row, column, [this, &adjacentMineCount](const int neighborRow, const int neighborColumn, const std::size_t) {
                if (cell(neighborRow, neighborColumn).hasMine)
                {
                    ++adjacentMineCount;
                }
            });

            currentCell.adjacentMineCount = static_cast<std::uint8_t>(adjacentMineCount);
        }
    }
}

void Board::revealCell(const int row, const int column)
{
    if (state_ != BoardState::Running)
    {
        return;
    }

    CellState& selectedCell = cells_[indexOf(row, column)];
    if (selectedCell.isOpen || selectedCell.isFlagged)
    {
        return;
    }

    if (selectedCell.hasMine)
    {
        selectedCell.isOpen = true;
        state_ = BoardState::Lost;
        return;
    }

    revealRegionFrom(row, column);
    updateWinState();
}

void Board::revealRegionFrom(const int startRow, const int startColumn)
{
    std::vector<std::size_t> queue;
    queue.push_back(indexOf(startRow, startColumn));

    CellState& startCell = cells_[queue.front()];
    startCell.isOpen = true;
    ++revealedSafeCells_;

    std::size_t queueHead = 0;
    while (queueHead < queue.size())
    {
        const std::size_t currentIndex = queue[queueHead++];
        const CellState& currentCell = cells_[currentIndex];
        if (currentCell.adjacentMineCount != 0)
        {
            continue;
        }

        const int currentRow = static_cast<int>(currentIndex / static_cast<std::size_t>(columns()));
        const int currentColumn = static_cast<int>(currentIndex % static_cast<std::size_t>(columns()));

        forEachNeighbor(currentRow, currentColumn, [this, &queue](const int, const int, const std::size_t neighborIndex) {
            CellState& neighborCell = cells_[neighborIndex];
            if (neighborCell.isOpen || neighborCell.isFlagged || neighborCell.hasMine)
            {
                return;
            }

            neighborCell.isOpen = true;
            ++revealedSafeCells_;

            if (neighborCell.adjacentMineCount == 0)
            {
                queue.push_back(neighborIndex);
            }
        });
    }
}

void Board::chordCell(const int row, const int column)
{
    if (state_ != BoardState::Running)
    {
        return;
    }

    const CellState& selectedCell = cell(row, column);
    if (!selectedCell.isOpen || selectedCell.adjacentMineCount == 0)
    {
        return;
    }

    if (flaggedNeighborCount(row, column) != selectedCell.adjacentMineCount)
    {
        return;
    }

    forEachNeighbor(row, column, [this](const int neighborRow, const int neighborColumn, const std::size_t neighborIndex) {
        if (state_ != BoardState::Running)
        {
            return;
        }

        const CellState& neighborCell = cells_[neighborIndex];
        if (neighborCell.isOpen || neighborCell.isFlagged)
        {
            return;
        }

        revealCell(neighborRow, neighborColumn);
    });
}

void Board::updateWinState()
{
    if (state_ != BoardState::Running)
    {
        return;
    }

    const int safeCellCount = rows() * columns() - mineCount();
    if (revealedSafeCells_ == safeCellCount)
    {
        state_ = BoardState::Won;
    }
}

std::size_t Board::indexOf(const int row, const int column) const
{
    return static_cast<std::size_t>(row * columns() + column);
}

int Board::flaggedNeighborCount(const int row, const int column) const
{
    int flaggedNeighborCount = 0;

    forEachNeighbor(row, column, [this, &flaggedNeighborCount](const int neighborRow, const int neighborColumn, const std::size_t) {
        if (cell(neighborRow, neighborColumn).isFlagged)
        {
            ++flaggedNeighborCount;
        }
    });

    return flaggedNeighborCount;
}
} // namespace sapper::game
