
#include "kotte/input.hpp"

#include <raymath.h>
#include <utility>
#include <stdexcept>
#include <string>

namespace kotte
{
    // converts names from JSON to raylib
    int Input::key_from_string(const std::string& name) {

        if (name == "SPACE") return KEY_SPACE;
        if (name == "ESCAPE") return KEY_ESCAPE;
        if (name == "ENTER") return KEY_ENTER;

        if (name == "LEFT_CONTROL") return KEY_LEFT_CONTROL;
        if (name == "RIGHT_CONTROL") return KEY_RIGHT_CONTROL;
        if (name == "LEFT_SHIFT") return KEY_LEFT_SHIFT;
        if (name == "RIGHT_SHIFT") return KEY_RIGHT_SHIFT;
        if (name == "LEFT_ALT") return KEY_LEFT_ALT;
        if (name == "RIGHT_ALT") return KEY_RIGHT_ALT;

        if (name == "UP") return KEY_UP;
        if (name == "DOWN") return KEY_DOWN;
        if (name == "LEFT") return KEY_LEFT;
        if (name == "RIGHT") return KEY_RIGHT;

        //handle letters and numbers without checking each one of them
        if (name.size() == 1) {
            const char c = name[0];

            if (c >= 'A' && c <= 'Z') {
                return KEY_A + (c - 'A');
            }

            if (c >= '0' && c <= '9') {
                return KEY_ZERO + (c - '0');
            }
        }

        throw std::runtime_error{
            "Unsupported key name: " + name
        };
    }

    // read and check bindings from JSON
    void Input::load_bindings(const nlohmann::json& config) {
        const auto& actions = config.at("actions");
        const auto& directions = config.at("directions").at("move");

        auto read_keys = [](const nlohmann::json& values) {
            if (!values.is_array() || values.empty()) {
                throw std::runtime_error{
                    "Key binding must be a non-empty array"
                };
            }

            std::vector<int> keys;

            for (const auto& value : values) {
                keys.push_back(
                    Input::key_from_string(value.get<std::string>())
                );
            }

            return keys;
            };

        //keep new bindings seperate until they are valid
        Input candidate;

        candidate.bind(Action::quit,
            read_keys(actions.at("quit")));

        candidate.bind(Action::place_bomb,
            read_keys(actions.at("place_bomb")));

        candidate.bind(Action::detonate_bombs,
            read_keys(actions.at("detonate_bombs")));

        candidate.bind(Action::activate_shield,
            read_keys(actions.at("activate_shield")));

        candidate.bind(Action::restart_round,
            read_keys(actions.at("restart_round")));

        candidate.bind(Direction::move,
            read_keys(directions.at("up")),
            read_keys(directions.at("down")),
            read_keys(directions.at("left")),
            read_keys(directions.at("right")));

        actions_ = std::move(candidate.actions_);
        directions_ = std::move(candidate.directions_);
    }


    void Input::bind(Action action, std::vector<int> keys) {
        actions_[action] = std::move(keys);
    }

    void Input::bind(
        Direction direction,
        std::vector<int> up,
        std::vector<int> down,
        std::vector<int> left,
        std::vector<int> right) {
        directions_[direction] = {
            std::move(up),
            std::move(down),
            std::move(left),
            std::move(right)
        };
    }

    bool Input::down(Action action) const noexcept {
        const auto it = actions_.find(action);
        return it != actions_.end() && any_down(it->second);
    }

    bool Input::pressed(Action action) const noexcept {
        const auto it = actions_.find(action);
        return it != actions_.end() && any_pressed(it->second);
    }

    Vector2 Input::get(Direction direction) const noexcept {
        const auto it = directions_.find(direction);
        if (it == directions_.end()) {
            return {};
        }

        const DirectionBinding& binding = it->second;
        Vector2 value{
            float(any_down(binding.right)) - float(any_down(binding.left)),
            float(any_down(binding.down)) - float(any_down(binding.up))
        };

        if (Vector2LengthSqr(value) > 0.0f) {
            value = Vector2Normalize(value);
        }
        return value;
    }

    bool Input::any_down(const std::vector<int>& keys) noexcept {
        for (const int key : keys) {
            if (IsKeyDown(key)) {
                return true;
            }
        }
        return false;
    }

    bool Input::any_pressed(const std::vector<int>& keys) noexcept {
        for (const int key : keys) {
            if (IsKeyPressed(key)) {
                return true;
            }
        }
        return false;
    }
}
