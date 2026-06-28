#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <functional>

#include "../actions.hpp"
#include "../utils/bounded_counter.hpp"
#include "ring_buffer.hpp"

enum class CallbackMode : unsigned char {
    NEVER,   // disabled
    ALWAYS,  // every event

    PRESS,    // useless: call callback even if counter == counter.max
    RELEASE,  // useless: call callback even if counter != 0

    // useful guys:
    JUST_PRESS,
    JUST_RELEASE,
    // not very useful bro:
    JUST_ANY,
};
enum class ActionState : unsigned char {
    PRESSED = 0,
    RELEASED = 1,
    JUST_PRESSED = 2,
    JUST_RELEASED = 3,
};
constexpr const char* ActionState_to_string(const ActionState state) {
    switch (state) {
        case ActionState::PRESSED:
            return "PRESSED";
        case ActionState::RELEASED:
            return "RELEASED";
        case ActionState::JUST_PRESSED:
            return "JUST_PRESSED";
        case ActionState::JUST_RELEASED:
            return "JUST_RELEASED";
        default:
            return "UNKNOWN";
    }
}

class ActionSet {
public:
    static constexpr size_t actions_count = static_cast<size_t>(Actions::ACTIONS_MAX);
    using ActionId = std::underlying_type_t<Actions>;

private:
    static constexpr ActionId _cast(const Actions action) {
        return static_cast<ActionId>(action);
    }

private:
    template <class T>
    using _array = std::array<T, actions_count>;

    struct _Command {
    private:
        std::function<void(ActionState state, Actions action)> m_cb{nullptr};
        Actions m_action = Actions::ACTIONS_MAX;
        ActionState m_state = ActionState::PRESSED;

    public:
        void apply() const {
            if (m_cb) m_cb(m_state, m_action);
        }
        constexpr bool is_valid() const { return m_action != Actions::ACTIONS_MAX; }

        _Command();
        _Command(const std::function<void(ActionState state, Actions action)> cb,
                 const Actions action, const ActionState state);
    };

private:
    _array<BoundedCounter<unsigned char>> m_presses{};
    _array<CallbackMode> m_callback_modes{};
    _array<std::function<void(ActionState state, Actions action)>> m_callbacks{};
    std::bitset<actions_count> m_is_just{};
    RingBuffer<_Command, 32> m_commandqueue{};

public:
    void update_just() { m_is_just.reset(); }
    void dispatch_callbacks();
    void reset_presses();

    constexpr bool _check_callback(const Actions action, const ActionState state) {
        using enum ActionState;
        switch (m_callback_modes[_cast(action)]) {
            case CallbackMode::NEVER:
                return false;
            case CallbackMode::ALWAYS:
                return true;
            case CallbackMode::PRESS:
                return state == PRESSED || state == JUST_PRESSED;
            case CallbackMode::RELEASE:
                return state == RELEASED || state == JUST_RELEASED;
            case CallbackMode::JUST_PRESS:
                return state == JUST_PRESSED;
            case CallbackMode::JUST_RELEASE:
                return state == JUST_RELEASED;
            case CallbackMode::JUST_ANY:
                return state == JUST_PRESSED || state == JUST_RELEASED;
            default:
                return false;
        }
    }

private:
    [[nodiscard]] constexpr static ActionState _to_state(const bool press, const bool just) {
        if (just) {
            if (press)
                return ActionState::JUST_PRESSED;
            else
                return ActionState::JUST_RELEASED;
        } else {
            if (press)
                return ActionState::PRESSED;
            else
                return ActionState::RELEASED;
        }
    }

    void _update(const Actions action, const bool press, const bool just);

public:
    void press(const Actions action);
    void release(const Actions action);

    [[nodiscard]] constexpr bool is_just_pressed(const Actions action) const {
        return (m_presses[_cast(action)] != 0) && m_is_just[_cast(action)];
    }
    [[nodiscard]] constexpr bool is_just_released(const Actions action) const {
        return (m_presses[_cast(action)] == 0) && m_is_just[_cast(action)];
    }
    [[nodiscard]] constexpr bool is_pressed(const Actions action) const {
        return m_presses[_cast(action)] != 0;
    }
    [[nodiscard]] constexpr bool is_released(const Actions action) const {
        return m_presses[_cast(action)] == 0;
    }

    void set_callback(const Actions action,
                      const std::function<void(ActionState state, Actions action)>& cb);
    void set_callback_mode(const Actions action, const CallbackMode mode);

    class ActionWrapper {
        ActionSet& m_set;
        const Actions m_id;

    public:
        void press() { m_set.press(m_id); }
        void release() { m_set.release(m_id); }

        [[nodiscard]] constexpr bool is_just_pressed() const {
            return m_set.is_just_pressed(m_id);
        }
        [[nodiscard]] constexpr bool is_just_released() const {
            return m_set.is_just_released(m_id);
        }
        [[nodiscard]] constexpr bool is_pressed() const { return m_set.is_pressed(m_id); }
        [[nodiscard]] constexpr bool is_released() const { return m_set.is_released(m_id); }

        ActionWrapper& set_callback(
            const std::function<void(ActionState state, Actions action)>& cb) {
            m_set.set_callback(m_id, cb);
            return *this;
        }
        ActionWrapper& set_callback_mode(const CallbackMode mode) {
            m_set.set_callback_mode(m_id, mode);
            return *this;
        }
        // operator overloading flex
        ActionWrapper& operator<<(
            const std::function<void(ActionState state, Actions action)>& cb) {
            return set_callback(cb);
        }
        ActionWrapper& operator<<(const CallbackMode mode) { return set_callback_mode(mode); }

        // better use ActionSet::operator[]
        constexpr ActionWrapper(ActionSet& set, Actions action) : m_set(set), m_id(action) {}
    };

    constexpr ActionWrapper operator[](Actions id) { return ActionWrapper(*this, id); }
};
