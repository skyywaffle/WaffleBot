#include "AudioHandling.h"
#include "Macro.h"
#include "Input.h"
#include <sndfile.h>
#include <sndfile.hh>
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

struct AudioFile
{
    SF_INFO info;
    SNDFILE *file;
    std::vector<short> buffer;
};

// Returns whether audio generation succeeded
bool generateAudio(const Macro &macro)
{
    // TODO: refactor the fuck out of this function

    std::vector<AudioFile> clicks{};
    std::vector<AudioFile> releases{};
    std::vector<AudioFile> softClicks{};
    std::vector<AudioFile> softReleases{};

    // TODO: iterate over files in "clicks" folder, add each to clicks vector
    // ...

    // TODO: iterate over files in "releases" folder, add each to releases vector
    // ...

    // TODO: iterate over files in "softClicks" folder, add each to softClicks vector
    // ...

    // TODO: iterate over files in "softReleases" folder, add each to softReleases vector
    // ...

    AudioFile click{};
    click.file = sf_open("click.wav", SFM_READ, &click.info);

    SF_INFO sfinfoRelease;
    SNDFILE *releaseFile = sf_open("release.wav", SFM_READ, &sfinfoRelease);

    SF_INFO sfinfoSoftClick;
    SNDFILE *softClickFile = sf_open("softclick.wav", SFM_READ, &sfinfoSoftClick);

    SF_INFO sfinfoSoftRelease;
    SNDFILE *softReleaseFile = sf_open("softrelease.wav", SFM_READ, &sfinfoSoftRelease);

    // Read the click samples
    click.buffer.resize(click.info.frames * click.info.channels);
    sf_read_short(click.file, click.buffer.data(), click.buffer.size());
    sf_close(click.file);

    // Read the release samples
    std::vector<short> releaseBuffer(sfinfoRelease.frames * sfinfoRelease.channels);
    sf_read_short(releaseFile, releaseBuffer.data(), releaseBuffer.size());
    sf_close(releaseFile);

    // Read the softclick samples
    std::vector<short> softClickBuffer(sfinfoSoftClick.frames * sfinfoSoftClick.channels);
    sf_read_short(softClickFile, softClickBuffer.data(), softClickBuffer.size());
    sf_close(softClickFile);

    // Read the softrelease samples
    std::vector<short> softReleaseBuffer(sfinfoSoftRelease.frames * sfinfoSoftRelease.channels);
    sf_read_short(softReleaseFile, softReleaseBuffer.data(), softReleaseBuffer.size());
    sf_close(softReleaseFile);

    // Define the total duration in seconds of the output file
    float durationSeconds = (float)(macro.getDurationInSec() + 1);
    int sampleRate = click.info.samplerate;
    int channels = click.info.channels;
    sf_count_t totalFrames = static_cast<sf_count_t>(durationSeconds * sampleRate);
    std::vector<short> outputBuffer(totalFrames * channels, 0); // Silent buffer

    // Define time points where clicks and releases (and soft clicks/releases) occur (in seconds)
    std::vector<float> clickTimes{};
    std::vector<float> releaseTimes{};
    std::vector<float> softClickTimes{};
    std::vector<float> softReleaseTimes{};

    for (Input i : macro.getInputs())
    {
        if (i.isDown())
        {
            if (i.isSoft())
            {
                softClickTimes.push_back(i.getFrame() / 240.0f);
            }
            else
            {
                clickTimes.push_back(i.getFrame() / 240.0f);
            }
        }
        else
        {
            if (i.isSoft())
            {
                softReleaseTimes.push_back(i.getFrame() / 240.0f);
            }
            else
            {
                releaseTimes.push_back(i.getFrame() / 240.0f);
            }
        }
    }

    // Add clicks to output buffer
    for (float t : clickTimes)
    {
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, click.buffer, sampleIndex, channels);
    }

    // Add releases to output buffer
    for (float t : releaseTimes)
    {
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, releaseBuffer, sampleIndex, channels);
    }

    // Add soft clicks to output buffer
    for (float t : softClickTimes)
    {
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, softClickBuffer, sampleIndex, channels);
    }

    // Add soft releases to output buffer
    for (float t : softReleaseTimes)
    {
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, softReleaseBuffer, sampleIndex, channels);
    }

    // Write to new WAV file
    SF_INFO sfinfoOut = click.info;
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
