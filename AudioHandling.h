#pragma once
#include "Macro.h"
#include "Input.h"
#include <libsndfile/sndfile.h>

void mix_click(std::vector<short> &outBuffer, const std::vector<short> &click, int insertIndex, int channels);
bool generateAudio(const Macro &macro);
