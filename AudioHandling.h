#pragma once
#include "Macro.h"
#include "Input.h"
#include <sndfile.h>

void mix_click(std::vector<short> &outBuffer, const std::vector<short> &click, int insertIndex, int channels);
bool generateAudio(Macro &macro);
