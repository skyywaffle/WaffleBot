#include "Macro.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

// File type is JSON
// Currently only supports xdBot macros
Macro::Macro(std::string filename)
{
    m_name = filename;
    for (int i {0}; i < 9; i++)
    {
        m_name.pop_back();
    }
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

        for (int index{2}; index < m_inputs.size(); index++)
        {
            // compare this with the previous click in the macro
            if (m_inputs[index].isDown())
            {
                // if time between this click and the previous click is less than 0.2, make it soft
                if (m_inputs[index].getFrame() - m_inputs[index - 2].getFrame() < 48) // because the indices are always click-release-click-release....
                {
                    m_inputs[index].setSoft(true);
                }

                // if time between this click and the previous release is less than 0.1, make it soft
                else if (m_inputs[index].getFrame() - m_inputs[index - 1].getFrame() < 24) // because the indices are always click-release-click-release....
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
