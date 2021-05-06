#pragma once
#include <array>
#include <bitset>

#include <Windows.h>

#include <DirectXMath.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GamePad.h>

#pragma region enums

enum class ButtonState
{
    Press,
    Release,
    Held,
    None,
};

enum class Keycode : size_t
{
    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,

    D0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,


    Left,
    Up,
    Right,
    Down,

    Esc,
    Space,
    Enter,
    Tab,
    Back,
    LeftShift,
    RightShift,
    LeftAlt,
    RightAlt,
    LeftCtrl,
    RightCtrl,

    End,

    // キーを追加したらInputState::Update()も変更すること
};

enum class MouseButton : size_t
{
    Left,
    Middle,
    Right,

    End,
};

enum class GamePadButton : size_t
{
    A,
    B,
    X,
    Y,

    LeftStick,      // 左スティックの押し込み
    RightStick,     // 右スティックの押し込み

    LeftShoulder,
    RightShoulder,

    LeftTrigger,
    RightTrigger,

    Back,
    Start,

    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight,

    End,
};

#pragma endregion

class KeyboardState final
{
    friend class InputSystem;

public:
    KeyboardState();
    ~KeyboardState() = default;

    [[nodiscard]] bool GetValue(const Keycode keycodes) const;
    [[nodiscard]] ButtonState GetState(const Keycode keycode) const;

private:
    std::unique_ptr<DirectX::Keyboard> keyboard = {};

    std::bitset<static_cast<size_t>(Keycode::End)> key_values = {};
    std::bitset<static_cast<size_t>(Keycode::End)> old_key_values = {};
};

class MouseState final
{
    friend class InputSystem;

public:
    MouseState(const HWND hwnd);
    ~MouseState() = default;

    [[nodiscard]] bool GetValue(const MouseButton buttons) const;
    [[nodiscard]] ButtonState GetState(const MouseButton button) const;

    [[nodiscard]] int PosX() const;
    [[nodiscard]] int PosY() const;
    [[nodiscard]] int DeltaX() const;
    [[nodiscard]] int DeltaY() const;
    [[nodiscard]] int ScrollWheelValue() const;

private:
    std::unique_ptr<DirectX::Mouse> mouse = {};

    std::bitset<static_cast<size_t>(MouseButton::End)> button_values = {};
    std::bitset<static_cast<size_t>(MouseButton::End)> old_button_values = {};

    int x = {};
    int y = {};
    int old_x = {};
    int old_y = {};
    int delta_x = {};
    int delta_y = {};
    int scroll_wheel_value = {};
};

class GamePadState final
{
    friend class InputSystem;

public:
    GamePadState();
    ~GamePadState() = default;

    [[nodiscard]] bool GetValue(const GamePadButton buttons,const int index = 0) const;
    [[nodiscard]] ButtonState GetState(const GamePadButton button,const int index = 0) const;

    [[nodiscard]] const DirectX::XMFLOAT2 RightThumbStick(const int index) const;
    [[nodiscard]] const DirectX::XMFLOAT2 LeftThumbStick(const int index) const;

public:
    using Bitset = std::bitset<static_cast<size_t>(GamePadButton::End)>;

private:
    std::unique_ptr<DirectX::GamePad> gamepad = {};
    std::array < DirectX::GamePad::State, DirectX::GamePad::MAX_PLAYER_COUNT> states;

    std::array<Bitset, DirectX::GamePad::MAX_PLAYER_COUNT> button_values;
    std::array<Bitset, DirectX::GamePad::MAX_PLAYER_COUNT> old_button_values;
};

class InputSystem final
{
public:
    explicit InputSystem(const HWND hwnd);
    ~InputSystem() = default;

    //// シングルトンを使用
    //[[nodiscard]] static InputSystem* Instance()
    //{
    //    static InputSystem instance{};
    //    return &instance;
    //}

    void Initialize(const HWND hwnd);
    void UpdateStates();

    [[nodiscard]] const KeyboardState& Keyboard() const { return *keyboard; }
    [[nodiscard]] const MouseState& Mouse() const { return *mouse; }
    [[nodiscard]] const GamePadState& GamePad() const { return *gamepad; }

private:
    std::unique_ptr<KeyboardState>  keyboard = {};
    std::unique_ptr<MouseState>     mouse = {};
    std::unique_ptr<GamePadState>   gamepad = {};
};
