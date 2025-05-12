#include "AudioHandling.h"
#include "Macro.h"
#include "Input.h"
#include <sndfile.h>
#include <sndfile.hh>
#include <iostream>
#include <vector>
#include <cstring>
#include <filesystem>
#include <random>
#include <algorithm>

namespace fs = std::filesystem;

struct AudioFile
{
    SF_INFO info;
    SNDFILE *file;
    std::vector<short> buffer;
};

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

std::vector<AudioFile> getAudioFiles(const char *folderName)
{
    std::vector<AudioFile> files{};

    fs::path folderPath{folderName};
    if (fs::exists(folderPath) && fs::is_directory(folderPath))
    {
        for (const auto &item : fs::directory_iterator(folderPath))
        {
            // Construct the filename and add a new AudioFile to the files vector
            if (fs::is_regular_file(item))
            {
                std::string fileString{folderName};
                fileString.append("/");
                fileString.append(item.path().filename().string());
                AudioFile audioFile{};
                audioFile.file = sf_open(fileString.c_str(), SFM_READ, &audioFile.info);

                audioFile.buffer.resize(audioFile.info.frames * audioFile.info.channels);
                sf_read_short(audioFile.file, audioFile.buffer.data(), audioFile.buffer.size());
                sf_close(audioFile.file);

                files.push_back(audioFile);
            }
        }
    }
    else // if the folder name is invalid
    {
        std::cerr << "Could not find folder name: " << folderName << '\n';
    }
    return files;
}

void addToBuffer(std::vector<float> &inputTimes, std::vector<AudioFile> &files, std::vector<short> &buffer, int sampleRate, int channels)
{
    // Add clicks to output buffer
    for (float t : inputTimes)
    {
        // set up random index choice
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, files.size() - 1);

        // Add to buffer
        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(buffer, files[distrib(gen)].buffer, sampleIndex, channels);
    }
}

// Returns whether audio generation succeeded
bool generateAudio(Macro &macro)
{
    // Get click audio files
    std::vector<AudioFile> clicks{getAudioFiles("player_1/clicks")};
    std::vector<AudioFile> releases{getAudioFiles("player_1/releases")};
    std::vector<AudioFile> softClicks{getAudioFiles("player_1/softclicks")};
    std::vector<AudioFile> softReleases{getAudioFiles("player_1/softreleases")};

    // Check that clicks actually exist
    if (clicks.size() == 0)
    {
        std::cerr << "Error: clicks not found.\n";
        return false;
    }
    if (releases.size() == 0)
    {
        std::cout << "Warning: releases not found.\n";
    }
    if (softClicks.size() == 0)
    {
        std::cout << "Warning: softclicks not found, using normal clicks.\n";
        softClicks = clicks;
    }
    if (softReleases.size() == 0)
    {
        std::cout << "Warning: softreleases not found, using normal releases.\n";
        softReleases = releases;
    }

    // Define the total duration in seconds of the output file
    float durationSeconds = (float)macro.getFrameCount() / macro.getFps() + 2; // add an extra 2 seconds so no releases get cut off
    int sampleRate = clicks[0].info.samplerate;
    int channels = clicks[0].info.channels;
    sf_count_t totalFrames = static_cast<sf_count_t>(durationSeconds * sampleRate);
    std::vector<short> outputBuffer(totalFrames * channels, 0); // Silent buffer

    // Define time points where clicks and releases (and soft clicks/releases) occur (in seconds)
    std::vector<float> clickTimes{};
    std::vector<float> releaseTimes{};
    std::vector<float> softClickTimes{};
    std::vector<float> softReleaseTimes{};

    // Add the times of inputs to their corresponding vectors
    // Add the times of inputs to their corresponding vectors
    for (Action action : macro.getActions())
    {
        for (Input input : action.getPlayerOneInputs())
        {
            if (input.isPressed())
            {
                if (input.getClickType() == ClickType::NORMAL)
                {
                    clickTimes.push_back(action.getFrame() / (float)macro.getFps());
                }

                else if (input.getClickType() == ClickType::SOFT)
                {
                    softClickTimes.push_back(action.getFrame() / (float)macro.getFps());
                }
            }
            else
            {
                if (input.getClickType() == ClickType::NORMAL)
                {
                    releaseTimes.push_back(action.getFrame() / (float)macro.getFps());
                }

                else if (input.getClickType() == ClickType::SOFT)
                {
                    softReleaseTimes.push_back(action.getFrame() / (float)macro.getFps());
                }
            }
        }
    }

    // Add click sounds to the output buffer
    addToBuffer(clickTimes, clicks, outputBuffer, sampleRate, channels);
    addToBuffer(releaseTimes, releases, outputBuffer, sampleRate, channels);
    addToBuffer(softClickTimes, softClicks, outputBuffer, sampleRate, channels);
    addToBuffer(softReleaseTimes, softReleases, outputBuffer, sampleRate, channels);

    // Write to new WAV file
    SF_INFO sfinfoOut = clicks[0].info;
    sfinfoOut.frames = totalFrames;
    SNDFILE *outFile = sf_open(macro.getName().append(".wav").c_str(), SFM_WRITE, &sfinfoOut);

    if (!outFile)
    {
        std::cerr << "Error creating output file for " << macro.getName() << '\n';
        return false;
    }

    sf_write_short(outFile, outputBuffer.data(), outputBuffer.size());
    sf_close(outFile);

    std::cout << "Successfully generated clicks for " << macro.getName() << '\n';

    return true;
}
