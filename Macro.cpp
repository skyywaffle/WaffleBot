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

    for (int index{0}; index < m_jsonData["inputs"].size(); index++) // Json i : m_jsonData["inputs"]
    {
        m_inputs.push_back(Input(m_jsonData["inputs"][index]["frame"],
                                 m_jsonData["inputs"][index]["2p"],
                                 m_jsonData["inputs"][index]["btn"],
                                 m_jsonData["inputs"][index]["down"],
                                 false));

        // if time between this click and the previous click is less than 0.2, make it soft
        // compare this with the previous click in the macro
        for (int index{2}; index < m_inputs.size(); index++)
        {
            if (m_inputs[index].isDown())
            {
                if (m_inputs[index].getFrame() - m_inputs[index - 2].getFrame() < 48) // because the indices are always click-release-click-release....
                {
                    m_inputs[index].setSoft(true);
                }
            }

            else // released
            {
                m_inputs[index].setSoft(m_inputs[index - 1].isSoft()); // matching soft release for each soft click
            }
        }
    }
}
