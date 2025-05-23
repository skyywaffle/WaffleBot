#include "Macro.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
using Json = nlohmann::json;
namespace fs = std::filesystem;

// File type is JSON, supports xdBot, MH Replay, and TASBot JSONs
Macro::Macro(std::string filepath)
{
    if (!fs::exists("config.json"))
    {
        std::cerr << "No config JSON found!\n";
        std::exit(1);
    }

    Json clickConfig = Json::parse(std::ifstream("config.json"));
    Json macroData = Json::parse(std::ifstream{filepath});
    m_name = fs::path(filepath).filename().string();

    // Strip ".json" from filename
    for (int i{0}; i < 5; i++)
    {
        m_name.pop_back();
    }

    // Determine what bot the macro comes from and set variables accordingly

    // Determine if macro is TASBot
    if (macroData.begin().key() == "fps") // TASBot has FPS as the first object
    {
        m_bot = Bot::TASBOT;
    }

    // Determine if macro is GDR and assign bot
    else if (macroData["bot"]["name"] != NULL)
    {
        if (macroData["bot"]["name"] == "MH_REPLAY")
        {
            m_bot = Bot::MH_REPLAY_GDR;
        }

        else if (macroData["bot"]["name"] == "xdBot")
        {
            m_bot = Bot::XDBOT_GDR;
        }
    }

    // else the macro is not supported
    else
    {
        std::cerr << "ERROR: " << m_name << " is either an unsupported macro or a corrupted one.\n";
        return;
    }

    // Parse xdBot JSON macro
    if (m_bot == Bot::XDBOT_GDR)
    {
        m_fps = macroData["framerate"];
        double durationInSec = macroData["duration"];
        m_frameCount = static_cast<int>(std::round(durationInSec * m_fps));
    }

    // Parse MH Replay JSON
    else if (m_bot == Bot::MH_REPLAY_GDR)
    {
        m_fps = 240; // Mega Hack Replay JSONs seem to only store time in 240fps frames, regardless of what FPS the macro was actually recorded at
        m_frameCount = macroData["duration"];
    }

    // Parse TASBot macro
    else if (m_bot == Bot::TASBOT)
    {
        m_fps = macroData["fps"];
        m_frameCount = macroData["macro"][macroData["macro"].size() - 1]["frame"]; // get the framecount from the last input of the macro
    }

    // Grab actions from GDR JSON
    if (m_bot == Bot::XDBOT_GDR || m_bot == Bot::MH_REPLAY_GDR)
    {
        for (Json &actionData : macroData["inputs"])
        {
            m_actions.push_back(Action(actionData, Bot::MH_REPLAY_GDR));
        }

        // Merge different actions on the same frame (happens if player 1 and player 2 make an action on the same frame)
        for (int i{1}; i < m_actions.size(); i++)
        {
            if (m_actions[i].getFrame() == m_actions[i - 1].getFrame())
            {
                // Transfer player 2's inputs to the previous action, where there are no player 2 inputs
                if (m_actions[i].getPlayerOneInputs().empty())
                {
                    m_actions[i - 1].setPlayerTwoInputs(m_actions[i].getPlayerTwoInputs());

                    // Remove the now redundant action
                    m_actions.erase(m_actions.begin() + i);
                    i--;
                }

                // Else transfer player 1's inputs to the previous action, where there are no player 1 inputs
                else if (m_actions[i].getPlayerTwoInputs().empty())
                {
                    m_actions[i - 1].setPlayerOneInputs(m_actions[i].getPlayerOneInputs());

                    // Remove the now redundant action
                    m_actions.erase(m_actions.begin() + i);
                    i--;
                }
            }
        }
    }

    // Grab inputs for TASBot
    else if (m_bot == Bot::TASBOT)
    {
        for (Json &actionData : macroData["macro"])
        {
            m_actions.push_back(Action(actionData, Bot::TASBOT));
        }

        // We don't need to consider merging different actions on same frame, already taken care of in Action.cpp
    }

    
    // CLICKTYPE FOR PLAYER 1 CLICKS
    // Determine click type (for presses only)
    for (int currentAction = 0; currentAction < m_actions.size(); ++currentAction)
    {
        for (Input &currentInput : m_actions[currentAction].getPlayerOneInputs())
        {
            if (currentInput.isPressed())
            {
                for (int previousAction = currentAction - 1; previousAction >= 0; --previousAction)
                {
                    for (Input &previousInput : m_actions[previousAction].getPlayerOneInputs())
                    {
                        if (previousInput.getButton() == currentInput.getButton())
                        {
                            float softClickAfterReleaseTime = clickConfig["softclickAfterReleaseTime"];
                            float frameDelta = m_actions[currentAction].getFrame() - m_actions[previousAction].getFrame();

                            if (frameDelta < m_fps * softClickAfterReleaseTime)
                            {
                                currentInput.setClickType(ClickType::SOFT);
                                goto p1ClickFound;
                            }
                        }
                    }
                }
            p1ClickFound:;
            }
        }
    }

    // Assign release types based on previous press
    for (int currentAction = 0; currentAction < m_actions.size(); ++currentAction)
    {
        for (Input &currentInput : m_actions[currentAction].getPlayerOneInputs())
        {
            if (!currentInput.isPressed())
            {
                for (int previousAction = currentAction - 1; previousAction >= 0; --previousAction)
                {
                    for (Input &previousInput : m_actions[previousAction].getPlayerOneInputs())
                    {
                        if (previousInput.getButton() == currentInput.getButton())
                        {
                            currentInput.setClickType(previousInput.getClickType());
                            goto p1ReleaseFound;
                        }
                    }
                }
            p1ReleaseFound:;
            }
        }
    }

    // CLICKTYPE FOR PLAYER 2 CLICKS
    // Determine click type (for presses only)
    for (int currentAction = 0; currentAction < m_actions.size(); ++currentAction)
    {
        for (Input &currentInput : m_actions[currentAction].getPlayerTwoInputs())
        {
            if (currentInput.isPressed())
            {
                for (int previousAction = currentAction - 1; previousAction >= 0; --previousAction)
                {
                    for (Input &previousInput : m_actions[previousAction].getPlayerTwoInputs())
                    {
                        if (previousInput.getButton() == currentInput.getButton())
                        {
                            float softClickAfterReleaseTime = clickConfig["softclickAfterReleaseTime"];
                            float frameDelta = m_actions[currentAction].getFrame() - m_actions[previousAction].getFrame();

                            if (frameDelta < m_fps * softClickAfterReleaseTime)
                            {
                                currentInput.setClickType(ClickType::SOFT);
                                goto p2ClickFound;
                            }
                        }
                    }
                }
            p2ClickFound:;
            }
        }
    }

    // Assign release types based on previous press
    for (int currentAction = 0; currentAction < m_actions.size(); ++currentAction)
    {
        for (Input &currentInput : m_actions[currentAction].getPlayerTwoInputs())
        {
            if (!currentInput.isPressed())
            {
                for (int previousAction = currentAction - 1; previousAction >= 0; --previousAction)
                {
                    for (Input &previousInput : m_actions[previousAction].getPlayerTwoInputs())
                    {
                        if (previousInput.getButton() == currentInput.getButton())
                        {
                            currentInput.setClickType(previousInput.getClickType());
                            goto p2ReleaseFound;
                        }
                    }
                }
            p2ReleaseFound:;
            }
        }
    }

    // For xdBot 2 player macros, 2p bool is flipped for some reason
    // Swap player one and player two inputs if it's an xdBot 2p macro
    if (isTwoPlayer() && m_bot == Bot::XDBOT_GDR) {
        for (Action &action : m_actions) {
            std::swap(action.getPlayerOneInputs(), action.getPlayerTwoInputs());
        }
    }
}

bool Macro::isTwoPlayer() {
    bool onePlayerExists{false};
    bool twoPlayerExists{false};

    for (Action &action : m_actions) {
        if (action.getPlayerOneInputs().size() > 0) {
            onePlayerExists = true;
        }
        else if (action.getPlayerTwoInputs().size() > 0) {
            twoPlayerExists = true;
        }
        if (onePlayerExists && twoPlayerExists) {
            return true;
        }
    }
    // Then it's not a two player macro
    return false;
}
