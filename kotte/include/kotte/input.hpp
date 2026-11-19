
#pragma once

#include <raylib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace kotte
{
    enum class Action {
        quit,
        place_bomb,
        detonate_bombs,
        activate_shield,
        restart_round
    };

    enum class Direction {
        move
    };

    class Input final {
    public:
        Input() = default;

        void bind(Action action, std::vector<int> keys);

        void load_bindings(const nlohmann::json& config);

        void bind(
            Direction direction,
            std::vector<int> up,
            std::vector<int> down,
            std::vector<int> left,
            std::vector<int> right);

        [[nodiscard]] bool down(Action action) const noexcept;
        [[nodiscard]] bool pressed(Action action) const noexcept;
        [[nodiscard]] Vector2 get(Direction direction) const noexcept;
        [[nodiscard]] static int key_from_string(const std::string& name);

    private:
        struct DirectionBinding final {
            std::vector<int> up;
            std::vector<int> down;
            std::vector<int> left;
            std::vector<int> right;
        };

        [[nodiscard]] static bool any_down(
            const std::vector<int>& keys) noexcept;

        [[nodiscard]] static bool any_pressed(
            const std::vector<int>& keys) noexcept;

        std::unordered_map<Action, std::vector<int>> actions_;
        std::unordered_map<Direction, DirectionBinding> directions_;
    };
}
