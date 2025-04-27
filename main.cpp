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
    Macro macro{std::string(argv[1])};
    generateAudio(macro);
    return 0;
}
