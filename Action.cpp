#include "Action.h"

#include <string>
#include <vector>

#include "Input.h"
#include "Macro.h"

Action::Action(simdjson_result<ondemand::value> &actionData, Bot bot) {
    // Initialize relevant variables
    Button button{};
    bool isSecondPlayer{};

    // xdBot and Mega Hack 2.2 macros have identical input structures for this purpose
    if (bot == Bot::XDBOT_GDR || bot == Bot::MH_REPLAY_GDR) {
        int64_t frame;
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
        int64_t frame;

        auto err = actionData["frame"].get(frame);
        if (err) {
            std::cerr << "Failed to read 'frame': " << err << "\n";
            std::exit(1);
        }
        m_frame = frame;

        int64_t p1Click, p2Click;

        err = actionData["player_1"]["click"].get(p1Click);
        if (err) {
            std::cerr << "Failed to read 'player_1.click': " << err << "\n";
            std::exit(1);
        }

        err = actionData["player_2"]["click"].get(p2Click);
        if (err) {
            std::cerr << "Failed to read 'player_2.click': " << err << "\n";
            std::exit(1);
        }

        // Determine player logic
        bool isBothPlayers = (p1Click != 0 && p2Click != 0);
        bool isSecondPlayer = (!isBothPlayers && p1Click == 0);
        button = Button::JUMP;

        if (isBothPlayers) {
            bool p1_pressed = (p1Click == 1);
            bool p2_pressed = (p2Click == 1);

            m_playerOneInputs.emplace_back(button, p1_pressed, ClickType::NORMAL);
            m_playerTwoInputs.emplace_back(button, p2_pressed, ClickType::NORMAL);
        } else if (!isSecondPlayer) {
            bool pressed = (p1Click == 1);
            m_playerOneInputs.emplace_back(button, pressed, ClickType::NORMAL);
        } else {
            bool pressed = (p2Click == 1);
            m_playerTwoInputs.emplace_back(button, pressed, ClickType::NORMAL);
        }
    }
}