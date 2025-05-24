#include "Action.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "Input.h"
#include "Macro.h"
using Json = nlohmann::json;

Action::Action(Json &actionData, Bot bot) {
    // Initialize relevant variables
    Button button{};
    bool isSecondPlayer{};

    // xdBot and Mega Hack 2.2 macros have identical input structures for this purpose
    if (bot == Bot::XDBOT_GDR || bot == Bot::MH_REPLAY_GDR) {
        m_frame = actionData["frame"];
        isSecondPlayer = actionData["2p"];
        button = actionData["btn"];
        bool pressed{actionData["down"]};

        // Click type will be determined in Macro.cpp
        Input input{button, pressed, ClickType::NORMAL};

        if (!isSecondPlayer) {
            m_playerOneInputs.push_back(input);
        }
        else {
            m_playerTwoInputs.push_back(input);
        }

        // if there are 2 input objects with the same frame, they'll be merged in Macro.cpp
        // (this happens when both players' inputs change on the same frame)
    }

    else if (bot == Bot::TASBOT) {
        m_frame = actionData["frame"];

        // TASBot stores both a player_1 and player_2 input object regardless of if both of them changed or just one
        // The unchanged input will just store 0 and 0.0 into the click and xpos values
        bool isBothPlayers{actionData["player_1"]["click"] != 0 && actionData["player_2"]["click"] != 0};

        isSecondPlayer = (!isBothPlayers && actionData["player_1"]["click"] == 0);
        button = Button::JUMP; // Because 2.113 macro

        // We can actually just add actions with both players here instead of having to merge elsewhere
        // TASBot stores presses and releases as 1 and 2 respectively in "click"
        if (isBothPlayers) {
            bool p1_pressed{actionData["player_1"]["click"] == 1};
            bool p2_pressed{actionData["player_2"]["click"] == 1};

            // Click type will be determined in Macro.cpp
            Input playerOneInput{button, p1_pressed, ClickType::NORMAL};
            Input playerTwoInput{button, p2_pressed, ClickType::NORMAL};

            m_playerOneInputs.push_back(playerOneInput);
            m_playerTwoInputs.push_back(playerTwoInput);
        }
        else if (!isSecondPlayer) {
            bool pressed{actionData["player_1"]["click"] == 1};

            // Click type will be determined in Macro.cpp
            Input playerOneInput{button, pressed, ClickType::NORMAL};

            m_playerOneInputs.push_back(playerOneInput);
        }
        else {
            bool pressed{actionData["player_2"]["click"] == 1};

            // Click type will be determined in Macro.cpp
            Input playerTwoInput{button, pressed, ClickType::NORMAL};

            m_playerTwoInputs.push_back(playerTwoInput);
        }
    }
}
