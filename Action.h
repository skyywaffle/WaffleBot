#ifndef ACTION_H
#define ACTION_H
#include <vector>
#include <simdjson.h>

#include "Input.h"
#include "Macro.h"
using namespace simdjson;

enum class Bot;

class Action {
private:
    int m_frame{};
    std::vector<Input> m_playerOneInputs{};
    std::vector<Input> m_playerTwoInputs{};

public:
    Action(simdjson_result<ondemand::value> &actionData, Bot bot);

    int getFrame() { return m_frame; }
    std::vector<Input> &getPlayerOneInputs() { return m_playerOneInputs; }
    std::vector<Input> &getPlayerTwoInputs() { return m_playerTwoInputs; }

    void setPlayerOneInputs(std::vector<Input> &p1) { m_playerOneInputs = p1; }
    void setPlayerTwoInputs(std::vector<Input> &p2) { m_playerTwoInputs = p2; }
};
#endif // ACTION_H
