#pragma once

class Input
{
private:
    int m_frame{};
    bool m_is2Player{};
    int m_button{};
    bool m_isDown{};
    bool m_isSoft{};

public:
    explicit Input(int frame, bool is2Player, int button, bool isDown, bool isSoft)
        : m_frame{frame}, m_is2Player{is2Player},
          m_button{button}, m_isDown{isDown},
          m_isSoft{isSoft}
    {
    }

    int getFrame() { return m_frame; }
    bool is2Player() { return m_is2Player; }
    int getButton() { return m_button; }
    bool isDown() { return m_isDown; }
    bool isSoft() { return m_isSoft; }
    void setSoft(bool b) { m_isSoft = b; }
};
