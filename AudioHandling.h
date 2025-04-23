#pragma once
#include "Macro.h"
#include "Input.h"
#include <libsndfile/sndfile.h>

void createWavFromSilence(const std::string &effectWav,
                          const std::string &outputWav,
                          std::vector<int> insertPoints,
                          int totalDuration,
                          int sampleRate);

bool generateAudio(const Macro &macro);
