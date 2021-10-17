#include "input_system.h"

#include <assert.h>

//---------------< KeyboardState >---------------------------------------------------------------
#pragma region Keyboard
KeyboardState::KeyboardState()
    : keyboard{ std::make_unique<DirectX::Keyboard>() } {}

bool KeyboardState::GetValue(const Keycode keycodes) const
{
    return key_values.test(static_cast<size_t>(keycodes));
}

ButtonState KeyboardState::GetState(const Keycode keycode) const {
    if (key_values.test(static_cast<size_t>(keycode))) {
        if (old_key_values.test(static_cast<size_t>(keycode))) {
            return ButtonState::Held;
        }
        return ButtonState::Press;
    }

    if (old_key_values.test(static_cast<size_t>(keycode))) {
        return ButtonState::Release;
    }

    return ButtonState::None;
}

#pragma endregion Keyboard

//---------------< MouseState >-------------------------------------------------------------
#pragma region Mouse
MouseState::MouseState(const HWND hwnd)
    : mouse{ std::make_unique<DirectX::Mouse>() }
{
    mouse->SetWindow(hwnd);
    //mouse_->SetMode(DirectX::Mouse::MODE_RELATIVE);
}

bool MouseState::GetValue(const MouseButton buttons) const
{
    return button_values.test(static_cast<size_t>(buttons));
}

ButtonState MouseState::GetState(const MouseButton button) const
{
    if (button_values.test(static_cast<size_t>(button)))
    {
        if (old_button_values.test(static_cast<size_t>(button)))
        {
            return ButtonState::Held;
        }
        return ButtonState::Press;
    }

    if (old_button_values.test(static_cast<size_t>(button)))
    {
        return ButtonState::Release;
    }

    return ButtonState::None;
}

int MouseState::PosX() const
{
    return x;
}

int MouseState::PosY() const
{
    return y;
}

int MouseState::DeltaX() const
{
    return delta_x;
}

int MouseState::DeltaY() const
{
    return delta_y;
}

int MouseState::ScrollWheelValue() const
{
    return scroll_wheel_value;
}

int MouseState::DeltaScrollWheelValue() const
{
    return delta_scroll_wheel_value;
}

#pragma endregion Mouse

//---------------< GamePadState >-----------------------------------------------------------
#pragma region GamePad

GamePadState::GamePadState()
    :gamepad{ std::make_unique<DirectX::GamePad>() } {}


bool GamePadState::GetValue(const GamePadButton buttons, const int index) const
{
    return button_values.at(index).test(static_cast<size_t>(buttons));
}

ButtonState GamePadState::GetState(const GamePadButton button, const int index) const
{
    if (button_values.at(index).test(static_cast<size_t>(button)))
    {
        if (old_button_values.at(index).test(static_cast<size_t>(button)))
        {
            return ButtonState::Held;
        }
        return ButtonState::Press;
    }

    if (old_button_values.at(index).test(static_cast<size_t>(button)))
    {
        return ButtonState::Release;
    }

    return ButtonState::None;
}

const DirectX::XMFLOAT2 GamePadState::RightThumbStick(const int index) const
{
    if (index >= DirectX::GamePad::MAX_PLAYER_COUNT)
        assert(!"The registration limit on the gamepad has been exceeded");

    return DirectX::XMFLOAT2{ states.at(index).thumbSticks.rightX,states.at(index).thumbSticks.rightY };
}

const DirectX::XMFLOAT2 GamePadState::LeftThumbStick(const int index) const
{
    if (index >= DirectX::GamePad::MAX_PLAYER_COUNT)
        assert(!"The registration limit on the gamepad has been exceeded");
    return DirectX::XMFLOAT2{ states.at(index).thumbSticks.leftX,states.at(index).thumbSticks.leftY };
}

const float GamePadState::RightTrigger(const int index) const
{
    if (index >= DirectX::GamePad::MAX_PLAYER_COUNT)
        assert(!"The registration limit on the gamepad has been exceeded");
    return states.at(index).triggers.right;
}

const float GamePadState::LeftTrigger(const int index) const
{
    if (index >= DirectX::GamePad::MAX_PLAYER_COUNT)
        assert(!"The registration limit on the gamepad has been exceeded");
    return states.at(index).triggers.left;
}

#pragma endregion GamePad

//---------------< InputSystem >------------------------------------------------------------
#pragma region InputSystem

InputSystem::InputSystem(const HWND hwnd)
{
    keyboard = std::make_unique<KeyboardState>();
    mouse    = std::make_unique<MouseState>(hwnd);
    gamepad  = std::make_unique<GamePadState>();
}

void InputSystem::Initialize(const HWND hwnd)
{
    //keyboard = std::make_unique<KeyboardState>();
    //mouse = std::make_unique<MouseState>(hwnd);
    //gamepad = std::make_unique<GamePadState>();
}

void InputSystem::UpdateStates() {
    // キーボードの状態更新
    {
        keyboard->old_key_values = keyboard->key_values;
        keyboard->key_values = 0;
        if (keyboard->keyboard->IsConnected())
        {
            const auto s = keyboard->keyboard->GetState();
            auto& v = keyboard->key_values;

            if (s.Q) v.set(static_cast<size_t>(Keycode::Q));
            if (s.W) v.set(static_cast<size_t>(Keycode::W));
            if (s.E) v.set(static_cast<size_t>(Keycode::E));
            if (s.R) v.set(static_cast<size_t>(Keycode::R));
            if (s.T) v.set(static_cast<size_t>(Keycode::T));
            if (s.Y) v.set(static_cast<size_t>(Keycode::Y));
            if (s.U) v.set(static_cast<size_t>(Keycode::U));
            if (s.I) v.set(static_cast<size_t>(Keycode::I));
            if (s.O) v.set(static_cast<size_t>(Keycode::O));
            if (s.P) v.set(static_cast<size_t>(Keycode::P));
            if (s.A) v.set(static_cast<size_t>(Keycode::A));
            if (s.S) v.set(static_cast<size_t>(Keycode::S));
            if (s.D) v.set(static_cast<size_t>(Keycode::D));
            if (s.F) v.set(static_cast<size_t>(Keycode::F));
            if (s.G) v.set(static_cast<size_t>(Keycode::G));
            if (s.H) v.set(static_cast<size_t>(Keycode::H));
            if (s.J) v.set(static_cast<size_t>(Keycode::J));
            if (s.K) v.set(static_cast<size_t>(Keycode::K));
            if (s.L) v.set(static_cast<size_t>(Keycode::L));
            if (s.Z) v.set(static_cast<size_t>(Keycode::Z));
            if (s.X) v.set(static_cast<size_t>(Keycode::X));
            if (s.C) v.set(static_cast<size_t>(Keycode::C));
            if (s.V) v.set(static_cast<size_t>(Keycode::V));
            if (s.B) v.set(static_cast<size_t>(Keycode::B));
            if (s.N) v.set(static_cast<size_t>(Keycode::N));
            if (s.M) v.set(static_cast<size_t>(Keycode::M));

            if (s.D0) v.set(static_cast<size_t>(Keycode::D0));
            if (s.D1) v.set(static_cast<size_t>(Keycode::D1));
            if (s.D2) v.set(static_cast<size_t>(Keycode::D2));
            if (s.D3) v.set(static_cast<size_t>(Keycode::D3));
            if (s.D4) v.set(static_cast<size_t>(Keycode::D4));
            if (s.D5) v.set(static_cast<size_t>(Keycode::D5));
            if (s.D6) v.set(static_cast<size_t>(Keycode::D6));
            if (s.D7) v.set(static_cast<size_t>(Keycode::D7));
            if (s.D8) v.set(static_cast<size_t>(Keycode::D8));
            if (s.D9) v.set(static_cast<size_t>(Keycode::D9));

            if (s.F1)  v.set(static_cast<size_t>(Keycode::F1));
            if (s.F2)  v.set(static_cast<size_t>(Keycode::F2));
            if (s.F3)  v.set(static_cast<size_t>(Keycode::F3));
            if (s.F4)  v.set(static_cast<size_t>(Keycode::F4));
            if (s.F5)  v.set(static_cast<size_t>(Keycode::F5));
            if (s.F6)  v.set(static_cast<size_t>(Keycode::F6));
            if (s.F7)  v.set(static_cast<size_t>(Keycode::F7));
            if (s.F8)  v.set(static_cast<size_t>(Keycode::F8));
            if (s.F9)  v.set(static_cast<size_t>(Keycode::F9));
            if (s.F10) v.set(static_cast<size_t>(Keycode::F10));
            if (s.F11) v.set(static_cast<size_t>(Keycode::F11));
            if (s.F12) v.set(static_cast<size_t>(Keycode::F12));

            if (s.Left)  v.set(static_cast<size_t>(Keycode::Left));
            if (s.Up)    v.set(static_cast<size_t>(Keycode::Up));
            if (s.Right) v.set(static_cast<size_t>(Keycode::Right));
            if (s.Down)  v.set(static_cast<size_t>(Keycode::Down));

            if (s.Escape)       v.set(static_cast<size_t>(Keycode::Esc));
            if (s.Space)        v.set(static_cast<size_t>(Keycode::Space));
            if (s.Enter)        v.set(static_cast<size_t>(Keycode::Enter));
            if (s.Tab)          v.set(static_cast<size_t>(Keycode::Tab));
            if (s.Back)         v.set(static_cast<size_t>(Keycode::Back));
            if (s.LeftShift)    v.set(static_cast<size_t>(Keycode::LeftShift));
            if (s.RightShift)   v.set(static_cast<size_t>(Keycode::RightShift));
            if (s.LeftAlt)      v.set(static_cast<size_t>(Keycode::LeftAlt));
            if (s.RightAlt)     v.set(static_cast<size_t>(Keycode::RightAlt));
            if (s.LeftControl)  v.set(static_cast<size_t>(Keycode::LeftCtrl));
            if (s.RightControl) v.set(static_cast<size_t>(Keycode::RightCtrl));
        }
    }

    // マウスの状態更新
    {
        mouse->old_button_values = mouse->button_values;
        mouse->button_values = 0;
        if (mouse->mouse->IsConnected())
        {
            const auto s = mouse->mouse->GetState();
            auto& v = mouse->button_values;
            if (s.leftButton)   v.set(static_cast<size_t>(MouseButton::Left));
            if (s.middleButton) v.set(static_cast<size_t>(MouseButton::Middle));
            if (s.rightButton)  v.set(static_cast<size_t>(MouseButton::Right));

            mouse->old_x = mouse->x;
            mouse->old_y = mouse->y;
            mouse->x = s.x;
            mouse->y = s.y;
            mouse->delta_x = mouse->x - mouse->old_x;
            mouse->delta_y = mouse->y - mouse->old_y;
            mouse->old_scroll_wheel_value   = mouse->scroll_wheel_value;
            mouse->scroll_wheel_value       = s.scrollWheelValue;
            mouse->delta_scroll_wheel_value = mouse->scroll_wheel_value - mouse->old_scroll_wheel_value;
        }
    }

    // ゲームパッドの状態更新
    {
        for (int i = 0; i < DirectX::GamePad::MAX_PLAYER_COUNT; ++i)
        {
            auto s = gamepad->gamepad->GetState(i);

            gamepad->old_button_values.at(i) = gamepad->button_values.at(i);
            gamepad->button_values.at(i) = 0;
            if (s.IsConnected())
            {
                auto& v = gamepad->button_values.at(i);

                if (s.IsAPressed()) v.set(static_cast<size_t>(GamePadButton::A));
                if (s.IsBPressed()) v.set(static_cast<size_t>(GamePadButton::B));
                if (s.IsXPressed()) v.set(static_cast<size_t>(GamePadButton::X));
                if (s.IsYPressed()) v.set(static_cast<size_t>(GamePadButton::Y));

                if (s.IsLeftStickPressed())  v.set(static_cast<size_t>(GamePadButton::LeftStick));
                if (s.IsRightStickPressed()) v.set(static_cast<size_t>(GamePadButton::RightStick));

                if (s.IsLeftShoulderPressed())  v.set(static_cast<size_t>(GamePadButton::LeftShoulder));
                if (s.IsRightShoulderPressed()) v.set(static_cast<size_t>(GamePadButton::RightShoulder));

                if (s.IsLeftTriggerPressed())  v.set(static_cast<size_t>(GamePadButton::LeftTrigger));
                if (s.IsRightTriggerPressed()) v.set(static_cast<size_t>(GamePadButton::RightTrigger));

                if (s.IsBackPressed())  v.set(static_cast<size_t>(GamePadButton::Back));
                if (s.IsStartPressed()) v.set(static_cast<size_t>(GamePadButton::Start));

                if (s.IsDPadUpPressed())      v.set(static_cast<size_t>(GamePadButton::DPadUp));
                if (s.IsDPadDownPressed())    v.set(static_cast<size_t>(GamePadButton::DPadDown));
                if (s.IsDPadLeftPressed())    v.set(static_cast<size_t>(GamePadButton::DPadLeft));
                if (s.IsDPadRightPressed())   v.set(static_cast<size_t>(GamePadButton::DPadRight));

                gamepad->states.at(i) = s;
            }
        }
    }
}

#pragma endregion InputSystem