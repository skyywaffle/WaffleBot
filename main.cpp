#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <simdjson.h>

#include "AudioHandling.h"
#include "Macro.h"
#include "Timer.h"
// #define DEBUG

namespace fs = std::filesystem;
using namespace simdjson;

int main(int argc, char *argv[]) {
    std::string versionNumber{"1.3.1"};
    std::cout << "WaffleBot " << versionNumber << " by skyywaffle\n\n";

    Timer programTimer;

#ifndef DEBUG
    if (argc >= 2) {
        // set working directory to exe directory, drag and dropping files from outside the exe directory changes the working directory unintentionally......
        fs::current_path(fs::canonical(argv[0]).parent_path());

        for (std::size_t arg{1}; arg < argc; arg++) {
            // Timer t;
            Macro macro{argv[arg]};
            generateAudio(macro);
            // const double runtime {t.elapsed()};
            // std::cout << "Completed in " << runtime << " seconds.\n\n";
        }
    }
    else {
        std::cout << "No macro files given.\n\n";
    }
#endif

// Test a specific macro, debug feature
#ifdef DEBUG
    Macro macro{"Acheron.gdr.json"};
    generateAudio(macro);
#endif

    std::cout << "Total runtime: " << programTimer.elapsed() << " seconds\n\n";
    std::cout << "\nPress Enter to exit...";
    std::string dummy;
    std::getline(std::cin, dummy);

    return 0;
}
