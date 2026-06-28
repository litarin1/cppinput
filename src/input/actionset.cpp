#include "actionset.hpp"

void ActionSet::_update(const Actions action, const bool press, const bool just) {
    const ActionId&& id = _cast(action);
    if (just) m_is_just[id] = true;

    if (_check_callback(action, _to_state(press, just)))
        m_commandqueue.push_back({m_callbacks[id], action, _to_state(press, just)});
}

void ActionSet::dispatch_callbacks() {
    while (auto x = m_commandqueue.pop_front())
        x->apply();
}

ActionSet::_Command::_Command(const std::function<void(ActionState state, Actions action)> cb,
                              const Actions action, const ActionState state)
    : m_cb(cb), m_state(state) {}

ActionSet::_Command::_Command() {}

void ActionSet::press(const Actions action) {
    const ActionId&& id = _cast(action);

    if (m_presses[id].is_max()) return;

    const bool just = m_presses[id] == 0;
    m_presses[id]++;
    _update(action, true, just);
}

void ActionSet::release(const Actions action) {
    const ActionId&& id = _cast(action);

    if (m_presses[id].is_min()) return;

    const bool just = m_presses[id] == 1;
    m_presses[id]--;
    _update(action, false, just);
}

void ActionSet::set_callback(
    const Actions action, const std::function<void(ActionState state, Actions action)>& cb) {
    m_callbacks[_cast(action)] = cb;
}

void ActionSet::set_callback_mode(const Actions action, const CallbackMode mode) {
    m_callback_modes[_cast(action)] = mode;
}
void ActionSet::reset_presses() {
    for (auto& counter : m_presses)
        counter = 0;
    m_is_just.reset();
}
