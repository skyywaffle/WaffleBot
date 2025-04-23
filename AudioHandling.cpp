#include "AudioHandling.h"
#include "Macro.h"
#include "Input.h"
#include <libsndfile/sndfile.h>
#include <libsndfile/sndfile.hh>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>


// Returns whether audio generation succeeded
bool generateAudio(const Macro &macro)
{
    std::string clickFile{"click.wav"};
    std::string releaseFile{"release.wav"};
    const char *outputFile = "output.wav";
    const int sampleRate = 44100;
    const int channels = 2; // stereo
    const int seconds = macro.getDurationInSec();

    return true;
}
