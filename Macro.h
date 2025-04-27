#pragma once
#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include "Input.h"

using Json = nlohmann::json;

class Macro
{
private:
    std::string m_name{};
    Json m_jsonData{};
    int m_framerate{};
    double m_durationInSec{};
    int m_frameCount{};
    std::vector<Input> m_inputs{};

public:
    explicit Macro(std::string filename);

    Json getJson() const { return m_jsonData; }
    int getFramerate() const { return m_framerate; }
    double getDurationInSec() const { return m_durationInSec; }
    int getFrameCount() const { return m_frameCount; }
    std::vector<Input> getInputs() const { return m_inputs; }
    std::string getName() const { return m_name; }
};
