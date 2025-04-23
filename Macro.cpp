#include "Macro.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

// File type is JSON
Macro::Macro(std::string filename)
{
    // I previously tried this with regular constructor initialization and the piece of shit kept throwing errors
    // I'm not touching this shit again
    // Stay mad
    m_jsonData = Json::parse(std::ifstream{filename});
    m_framerate = m_jsonData["framerate"];
    m_durationInSec = m_jsonData["duration"];
    m_frameCount = static_cast<int>(std::round(m_durationInSec * m_framerate));

    for (Json i : m_jsonData["inputs"])
    {
        m_inputs.push_back(Input(i["frame"], i["2p"], i["btn"], i["down"]));
    }
}

/*
    int m_frame{};
    bool m_is2Player{};
    int m_button{};
    bool m_isDown{};
*/
