#include "Action.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "Input.h"
#include "Macro.h"
using Json = nlohmann::json;

Action::Action(simdjson_result<ondemand::value> &actionData, Bot bot) {
    // Initialize relevant variables
    Button button{};
    bool isSecondPlayer{};

    // xdBot and Mega Hack 2.2 macros have identical input structures for this purpose
    if (bot == Bot::XDBOT_GDR || bot == Bot::MH_REPLAY_GDR) {
        int64_t frame;
        bool isSecondPlayer;
        int64_t buttonInt;
        bool pressed;

        auto err = actionData["frame"].get(frame);
        if (err) {
            std::cerr << "Failed to read 'frame': " << err << "\n";
            std::exit(1);
        }
        m_frame = frame;

        err = actionData["2p"].get(isSecondPlayer);
        if (err) {
            std::cerr << "Failed to read '2p': " << err << "\n";
            std::exit(1);
        }

        err = actionData["btn"].get(buttonInt);
        if (err) {
            std::cerr << "Failed to read 'btn': " << err << "\n";
            std::exit(1);
        }

        button = static_cast<Button>(buttonInt);

        err = actionData["down"].get(pressed);
        if (err) {
            std::cerr << "Failed to read 'down': " << err << "\n";
            std::exit(1);
        }

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
        int p1Click = static_cast<int>(actionData["player_1"]["click"]);
        int p2Click = static_cast<int>(actionData["player_2"]["click"]);

        // TASBot stores both a player_1 and player_2 input object regardless of if both of them changed or just one
        // The unchanged input will just store 0 and 0.0 into the click and xpos values
        bool isBothPlayers{(p1Click != 0 && p2Click != 0)};

        isSecondPlayer = (!isBothPlayers && p1Click == 0);
        button = Button::JUMP; // Because 2.113 macro

        // We can actually just add actions with both players here instead of having to merge elsewhere
        // TASBot stores presses and releases as 1 and 2 respectively in "click"
        if (isBothPlayers) {
            bool p1_pressed{p1Click == 1};
            bool p2_pressed{p2Click == 1};

            // Click type will be determined in Macro.cpp
            Input playerOneInput{button, p1_pressed, ClickType::NORMAL};
            Input playerTwoInput{button, p2_pressed, ClickType::NORMAL};

            m_playerOneInputs.push_back(playerOneInput);
            m_playerTwoInputs.push_back(playerTwoInput);
        }
        else if (!isSecondPlayer) {
            bool pressed{p1Click == 1};

            // Click type will be determined in Macro.cpp
            Input playerOneInput{button, pressed, ClickType::NORMAL};

            m_playerOneInputs.push_back(playerOneInput);
        }
        else {
            bool pressed{p2Click == 1};

            // Click type will be determined in Macro.cpp
            Input playerTwoInput{button, pressed, ClickType::NORMAL};

            m_playerTwoInputs.push_back(playerTwoInput);
        }
    }
}
