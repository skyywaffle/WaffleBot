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

// File type is JSON, supports xdBot and MH Replay JSONs
Macro::Macro(std::string filepath)
{
    if (!fs::exists("config.json"))
    {
        std::cerr << "No config JSON found!\n";
        std::exit(1);
    }
    
    Json clickConfig = Json::parse(std::ifstream("config.json"));
    m_jsonData = Json::parse(std::ifstream{filepath});
    std::string bot = m_jsonData["bot"]["name"];
    m_name = fs::path(filepath).filename().string();

    // Parse xdBot JSON macro
    if (bot == "xdBot")
    {
        m_framerate = m_jsonData["framerate"];
        m_durationInSec = m_jsonData["duration"];
        m_frameCount = static_cast<int>(std::round(m_durationInSec * m_framerate));
    }

    // Parse MH Replay JSON
    else if (bot == "MH_REPLAY")
    {
        m_framerate = 240;
        m_frameCount = m_jsonData["duration"];
        m_durationInSec = (double)m_frameCount / m_framerate;
    }

    // Strip ".json" from filename
    for (int i{0}; i < 5; i++)
    {
        m_name.pop_back();
    }

    for (int index{0}; index < m_jsonData["inputs"].size(); index++)
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
