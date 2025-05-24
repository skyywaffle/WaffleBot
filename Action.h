#ifndef ACTION_H
#define ACTION_H
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "Input.h"
#include "Macro.h"
using Json = nlohmann::json;

enum class Bot;

class Action {
private:
    int m_frame{};
    std::vector<Input> m_playerOneInputs{};
    std::vector<Input> m_playerTwoInputs{};

public:
    Action(Json &actionData, Bot bot); // Bot has not been declared (Even though it's in Macro.h)

    int getFrame() { return m_frame; }
    std::vector<Input> &getPlayerOneInputs() { return m_playerOneInputs; }
    std::vector<Input> &getPlayerTwoInputs() { return m_playerTwoInputs; }

    void setPlayerOneInputs(std::vector<Input> &p1) { m_playerOneInputs = p1; }
    void setPlayerTwoInputs(std::vector<Input> &p2) { m_playerTwoInputs = p2; }
};
#endif // ACTION_H
