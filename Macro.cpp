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
    Json clickConfig = Json::parse(std::ifstream("config.json"));

    // determine the index of the start of the actual level name
    // [SOMETHING IN THIS SNIPPET OF CODE GIVES SEGMENTATION FAULT]
    int startOfFilename{static_cast<int>(filename.length() - 1)};
    while (filename[startOfFilename] != '\\' && filename[startOfFilename] != '/' && startOfFilename != -1)
    {
        startOfFilename--;
    }
    startOfFilename++;

    m_name = filename.substr(startOfFilename);
    m_jsonData = Json::parse(std::ifstream{filename});
    m_framerate = m_jsonData["framerate"];
    m_durationInSec = m_jsonData["duration"];
    m_frameCount = static_cast<int>(std::round(m_durationInSec * m_framerate));

    // Strip ".gdr.json" from filename
    for (int i{0}; i < 9; i++)
    {
        m_name.pop_back();
    }

    for (int index{0}; index < m_jsonData["inputs"].size(); index++) // Json i : m_jsonData["inputs"]
    {
        m_inputs.push_back(Input(m_jsonData["inputs"][index]["frame"],
                                 m_jsonData["inputs"][index]["2p"],
                                 m_jsonData["inputs"][index]["btn"],
                                 m_jsonData["inputs"][index]["down"],
                                 false));

        // First click and release will always be normal
        for (int index{2}; index < m_inputs.size(); index++)
        {
            // compare this with the previous click in the macro
            if (m_inputs[index].isDown())
            {
                // if time between this click and the previous click is less than user config time, make it soft
                if (m_inputs[index].getFrame() - m_inputs[index - 2].getFrame() < m_framerate * clickConfig["softclickTime"].get<double>()) // because the indices are always click-release-click-release....
                {
                    m_inputs[index].setSoft(true);
                }

                // if time between this click and the previous release is less than user config time, make it soft
                else if (m_inputs[index].getFrame() - m_inputs[index - 1].getFrame() < m_framerate * clickConfig["softclickAfterReleaseTime"].get<double>()) // because the indices are always click-release-click-release....
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
