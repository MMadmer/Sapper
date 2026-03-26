#pragma once

#include <array>
#include <cstddef>

namespace sapper::game
{
enum class DifficultyId : std::size_t
{
    Beginner = 0,
    Intermediate,
    Expert
};

struct Difficulty
{
    const char* label;
    int rows;
    int columns;
    int mineCount;
};

inline constexpr std::array<Difficulty, 3> kDifficulties{{
    {"Beginner", 9, 9, 10},
    {"Intermediate", 16, 16, 40},
    {"Expert", 16, 30, 99},
}};

inline constexpr const Difficulty& difficultyFor(const DifficultyId difficultyId)
{
    return kDifficulties[static_cast<std::size_t>(difficultyId)];
}
} // namespace sapper::game
