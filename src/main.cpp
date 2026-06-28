#include "actionset.hpp"
#include "input/input.hpp"
#include "log.cpp"
#include "utils/log.cpp"

int main() {
    _init_log();

    Input input{};
    input.bind_key(Key::KEY_ENTER, Actions::PRINT_HELLO) <<
        [](ActionState state, Actions action) {
            LINFO("Hello! {} {}", ActionState_to_string(state), _cast_enum(action));
            ;
        } << CallbackMode::ALWAYS;
    input.key_cb(Key::KEY_0, Input::ButtonState::PRESS, 0);
    input.dispatch_callbacks();
    input.update_just();
    input.key_cb(Key::KEY_0, Input::ButtonState::RELEASE, 0);
    input.dispatch_callbacks();
    input.update_just();

    input.key_cb(Key::KEY_ENTER, Input::ButtonState::PRESS, 0);  // 2
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::PRESS, 0);  // 0
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::PRESS, 0);  // 0
    input.dispatch_callbacks();
    input.update_just();
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::PRESS, 0);  // 0
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::RELEASE, 0);
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::RELEASE, 0);
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::RELEASE, 0);
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::RELEASE, 0);
    input.key_cb(Key::KEY_ENTER, Input::ButtonState::PRESS, 0);  // 2
    input.dispatch_callbacks();
    input.update_just();
}
