#include "AudioHandling.h"
#include "Macro.h"
#include "Input.h"
#include <libsndfile/sndfile.h>
#include <libsndfile/sndfile.hh>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

void mix_click(std::vector<short> &outBuffer, const std::vector<short> &click, int insertIndex, int channels)
{
    for (size_t i = 0; i < click.size(); ++i)
    {
        size_t outIndex = insertIndex + i;
        if (outIndex < outBuffer.size())
        {
            int mixed = outBuffer[outIndex] + click[i];
            // Clipping
            if (mixed > 32767)
                mixed = 32767;
            if (mixed < -32768)
                mixed = -32768;
            outBuffer[outIndex] = static_cast<short>(mixed);
        }
    }
}

// Returns whether audio generation succeeded
bool generateAudio(const Macro &macro)
{
    SF_INFO sfinfoClick;
    SNDFILE *clickFile = sf_open("click.wav", SFM_READ, &sfinfoClick);

    SF_INFO sfinfoRelease;
    SNDFILE *releaseFile = sf_open("release.wav", SFM_READ, &sfinfoRelease);

    // Read the click samples
    std::vector<short> clickBuffer(sfinfoClick.frames * sfinfoClick.channels);
    sf_read_short(clickFile, clickBuffer.data(), clickBuffer.size());
    sf_close(clickFile);

    // Read the release samples
    std::vector<short> releaseBuffer(sfinfoRelease.frames * sfinfoRelease.channels);
    sf_read_short(releaseFile, releaseBuffer.data(), releaseBuffer.size());
    sf_close(releaseFile);

    // Define the total duration in seconds of the output file
    float durationSeconds = (float)(macro.getDurationInSec() + 1);
    int sampleRate = sfinfoClick.samplerate;
    int channels = sfinfoClick.channels;
    sf_count_t totalFrames = static_cast<sf_count_t>(durationSeconds * sampleRate);
    std::vector<short> outputBuffer(totalFrames * channels, 0); // Silent buffer

    // Define time points where clicks and releases occur (in seconds)
    std::vector<float> clickTimes{};
    std::vector<float> releaseTimes{};

    for (Input i : macro.getInputs())
    {
        if (i.isDown())
        {
            clickTimes.push_back(i.getFrame() / 240.0f);
        }
        else
        {
            releaseTimes.push_back(i.getFrame() / 240.0f);
        }
    }

    // Add clicks to output buffer
    for (float t : clickTimes)
    {
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, clickBuffer, sampleIndex, channels);
    }

    // Add releases to output buffer
    for (float t : releaseTimes)
    {
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, releaseBuffer, sampleIndex, channels);
    }

    // Write to new WAV file
    SF_INFO sfinfoOut = sfinfoClick;
    sfinfoOut.frames = totalFrames;
    SNDFILE *outFile = sf_open("output.wav", SFM_WRITE, &sfinfoOut);

    if (!outFile)
    {
        std::cerr << "Error creating output file." << std::endl;
        return false;
    }

    sf_write_short(outFile, outputBuffer.data(), outputBuffer.size());
    sf_close(outFile);

    std::cout << "Click sequence written to: " << "output.wav" << std::endl;

    return true;
}
