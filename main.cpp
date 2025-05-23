#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "Action.h"
#include "AudioHandling.h"
#include "Input.h"
#include "Macro.h"
#define DEBUG

using Json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    std::string versionNumber{"1.2.0"};
    std::cout << "WaffleBot " << versionNumber << " by skyywaffle\n\n";

#ifndef DEBUG
    if (argc >= 2)
    {
        // set working directory to exe directory, drag and dropping files from outside the exe directory changes the working directory unintentionally......
        fs::current_path(fs::canonical(argv[0]).parent_path());

        for (std::size_t arg{1}; arg < argc; arg++)
        {
            Macro macro{argv[arg]};
            generateAudio(macro);
        }
    }
    else
    {
        std::cout << "No macro files given.\n";
    }
#endif

// Test a specific macro, debug feature
#ifdef DEBUG
    Macro macro{"Duelo Maestro Solo 62.gdr.json"};
    generateAudio(macro);
#endif

    std::cout << "\nPress Enter to exit...";
    std::string dummy;
    std::getline(std::cin, dummy);

    return 0;
}
