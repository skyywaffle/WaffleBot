#include "Macro.h"
#include "Input.h"
#include "AudioHandling.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using Json = nlohmann::json;

int main()
{
    Macro macro{"VSC.gdr.json"};
    generateAudio(macro);
    return 0;
}
