#include "AudioHandling.h"
#include "Macro.h"
#include "Input.h"
#include <libsndfile/sndfile.h>
#include <libsndfile/sndfile.hh>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

void createWavFromSilence(const std::string &effectWav, const std::string &outputWav, std::vector<int> insertPoints, int totalDuration, int sampleRate)
{
    // Open the sound effect file
    SF_INFO effectInfo;
    SNDFILE *effectFile = sf_open(effectWav.c_str(), SFM_READ, &effectInfo);
    if (!effectFile)
    {
        std::cerr << "Error opening effect file: " << sf_strerror(effectFile) << std::endl;
        return;
    }

    // Create an output SF_INFO structure for the new file (empty silent WAV)
    SF_INFO outputInfo = {};
    outputInfo.samplerate = sampleRate;
    outputInfo.channels = effectInfo.channels;
    outputInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // PCM 16-bit WAV file

    // Open the output file for writing
    SNDFILE *outputFile = sf_open(outputWav.c_str(), SFM_WRITE, &outputInfo);
    if (!outputFile)
    {
        std::cerr << "Error creating output file: " << sf_strerror(outputFile) << std::endl;
        sf_close(effectFile);
        return;
    }

    // Create a silent buffer with a size equal to the total duration
    std::vector<short> silentBuffer(sampleRate * effectInfo.channels * totalDuration);

    // Buffers for the sound effect
    std::vector<short> effectBuffer(effectInfo.frames * effectInfo.channels);
    sf_readf_short(effectFile, effectBuffer.data(), effectInfo.frames);

    // Insert the sound effect into the silence buffer at the specified points
    for (size_t i = 0; i < insertPoints.size(); ++i)
    {
        int insertPoint = insertPoints[i];
        // Ensure the insert point is within the buffer size
        if (insertPoint < silentBuffer.size() / effectInfo.channels)
        {
            for (size_t j = 0; j < effectBuffer.size(); ++j)
            {
                // Insert effect at the given point (make sure not to overwrite existing data)
                int insertIndex = insertPoint * effectInfo.channels + j;
                if (insertIndex < silentBuffer.size())
                {
                    silentBuffer[insertIndex] = effectBuffer[j];
                }
            }
        }
    }

    // Write the modified audio (silence + inserted effects) to the output file
    sf_writef_short(outputFile, silentBuffer.data(), silentBuffer.size() / effectInfo.channels);

    // Close the files
    sf_close(effectFile);
    sf_close(outputFile);

    std::cout << "Audio file saved to: " << outputWav << std::endl;
}

// Returns whether audio generation succeeded
bool generateAudio(const Macro &macro)
{
    std::string clickFile{"click.wav"};
    std::string releaseFile{"release.wav"};
    const char *outputFile = "output.wav";
    const int sampleRate = 44100;
    const int channels = 2; // stereo
    const int seconds = macro.getDurationInSec();

    std::vector<int> clickPoints{};
    for (Input i : macro.getInputs())
    {
        if (i.isDown())
        {
            clickPoints.push_back((int)(i.getFrame() / 240.0 * 44100.0));
        }
    }

    createWavFromSilence(clickFile, outputFile, clickPoints, macro.getDurationInSec(), 44100);
    return true;
}
