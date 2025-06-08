#ifndef MACRO_H
#define MACRO_H
#include <string>
#include <vector>
#include <simdjson.h>

#include "Action.h"

class Action;

using namespace simdjson;

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

    padded_string m_macroBuffer;
    ondemand::parser m_parser;

public:
    explicit Macro(const std::string& filepath);

    // START OF GETTERS AND SETTERS
    const std::string& getName() const { return m_name; }
    std::string getModifiableName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    const Bot getBot() const { return m_bot; }
    void setBot(const Bot& bot) { m_bot = bot; }

    int getFps() const { return m_fps; }
    void setFps(int fps) { m_fps = fps; }

    int getFrameCount() const { return m_frameCount; }
    void setFrameCount(int frameCount) { m_frameCount = frameCount; }

    std::vector<Action>& getActions() { return m_actions; }
    void setActions(const std::vector<Action>& actions) { m_actions = actions; }

    ondemand::document getMacroData();

    // END OF GETTERS AND SETTERS

    void determineBotType();
    void parseMacroJson();
    void determineClickTypes();
    bool isTwoPlayer();
    void swapPlayerOneAndTwoActions();
};
#endif // MACRO_H