#ifndef AUDIOHANDLING_H
#define AUDIOHANDLING_H
#include "Macro.h"

void mix_click(std::vector<short> &outBuffer, const std::vector<short> &click, int insertIndex, int channels);
bool generateAudio(Macro &macro);
#endif // AUDIOHANDLING_H
