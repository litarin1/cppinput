#include "input.hpp"

void Input::update_just() { m_actions.update_just(); }
void Input::dispatch_callbacks() { m_actions.dispatch_callbacks(); }
void Input::reset() {}
void Input::key_cb(const Key key, const ButtonState state, const int mods) {
    const Actions action = m_binds[_cast_enum(key)];
    if (action == Actions::ACTIONS_MAX) return;
    if (state == ButtonState::PRESS)
        m_actions.press(action);
    else
        m_actions.release(action);
}
Input::ActionWrapper Input::bind_key(const Key key, const Actions action) {
    m_binds[_cast_enum(key)] = action;
    return m_actions[action];
}
Actions Input::unbind_key(const Key key) {
    Actions out = m_binds[_cast_enum(key)];
    m_binds[_cast_enum(key)] = Actions::ACTIONS_MAX;
    return out;
}
