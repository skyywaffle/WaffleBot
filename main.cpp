#include "Macro.h"
#include "Input.h"
#include "AudioHandling.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using Json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    std::string versionNumber{"1.0.0-hotfix"};
    std::cout << "WaffleBot " << versionNumber << " by skyywaffle\n\n";

    if (argc >= 2)
    {
        // set working directory to exe directory, drag and dropping files from outside the exe directory changes the working directory unintentionally......
        fs::current_path(fs::canonical(argv[0]).parent_path());

        for (std::size_t arg{1}; arg < argc; arg++)
        {
            generateAudio(Macro(argv[arg]));
        }
    }
    else
    {
        std::cout << "No macro files given.\n";
    }

    std::cout << "\nPress Enter to exit...";
    std::string dummy;
    std::getline(std::cin, dummy);

    return 0;
}
