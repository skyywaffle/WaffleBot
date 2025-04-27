#include "Macro.h"
#include "Input.h"
#include "AudioHandling.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using Json = nlohmann::json;

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        for (std::size_t arg{1}; arg < argc; arg++)
        {
            generateAudio(Macro(argv[arg]));
        }
    }
    else
    {
        std::cout << "No macro files given.\n";
    }

    return 0;
}
