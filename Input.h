#pragma once

enum class Button
{
    JUMP = 1,
    LEFT = 2,
    RIGHT = 3,
};

enum class ClickType
{
    VERY_SOFT,
    SOFT,
    NORMAL,
    HARD,
    VERY_HARD
};

class Input
{
private:
    Button m_button{};
    bool m_pressed{};
    ClickType m_clickType{};

public:
    Input() = delete;

    explicit Input(Button button, bool pressed, ClickType clickType)
        : m_button{button}, m_pressed{pressed}, m_clickType{clickType}
    {
    }

    Button getButton() { return m_button; }
    bool isPressed() { return m_pressed; }
    ClickType getClickType() { return m_clickType; }
    void setClickType(ClickType clickType) { m_clickType = clickType; }
};
