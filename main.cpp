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
    Macro macro{"Cataclysm.gdr.json"};

    for (Input i : macro.getInputs())
    {
        std::cout << i.getFrame() << '\t' << i.isDown() << '\n';
    }
    std::cout << '\n'
              << macro.getFrameCount() << '\n';

    generateAudio(macro);
    return 0;
}
