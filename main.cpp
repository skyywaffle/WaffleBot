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
    Macro macro{"Acheron.gdr.json"};

    // test for seeing what clicks are soft
    for (Input i : macro.getInputs())
    {
        std::cout << i.getFrame();
        if (i.isDown())
        {
            std::cout << "\t" << i.isSoft();
        }
        std::cout << "\n";
    }


    generateAudio(macro);
    return 0;
}
