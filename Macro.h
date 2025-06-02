#ifndef MACRO_H
#define MACRO_H
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "Action.h"

class Action;

using Json = nlohmann::json;

enum class Bot {
    XDBOT_GDR,
    MH_REPLAY_GDR,
    TASBOT,
};

class Macro {
    std::string m_name{};
    Bot m_bot{};
    int m_fps{};
    int m_frameCount{};
    std::vector<Action> m_actions{};

public:
    explicit Macro(std::string filename);

    int getFps() { return m_fps; }
    int getFrameCount() { return m_frameCount; }
    bool isTwoPlayer();
    Bot getBot() { return m_bot; }
    std::vector<Action> &getActions() { return m_actions; }
    std::string &getName() { return m_name; }

    void determineClickType(int player, bool click);
};
#endif // MACRO_H