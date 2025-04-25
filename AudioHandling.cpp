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
    std::vector<AudioFile> clicks{};
    std::vector<AudioFile> releases{};
    std::vector<AudioFile> softClicks{};
    std::vector<AudioFile> softReleases{};

    // iterate over files in "clicks" folder, add each to clicks vector
    fs::path clicksPath{"clicks"};
    if (fs::exists(clicksPath) && fs::is_directory(clicksPath))
    {
        for (const auto &item : fs::directory_iterator(clicksPath))
        {
            // Construct the filename and add a new AudioFile to clicks
            if (fs::is_regular_file(item))
            {
                std::string fileString{clicksPath.string()};
                fileString.append("/");
                fileString.append(item.path().filename().string());
                AudioFile af{};
                af.file = sf_open(fileString.c_str(), SFM_READ, &af.info);

                af.buffer.resize(af.info.frames * af.info.channels);
                sf_read_short(af.file, af.buffer.data(), af.buffer.size());
                sf_close(af.file);

                clicks.push_back(af);
            }
        }
    }

    // iterate over files in "releases" folder, add each to releases vector
    fs::path releasesPath{"releases"};
    if (fs::exists(releasesPath) && fs::is_directory(releasesPath))
    {
        for (const auto &item : fs::directory_iterator(releasesPath))
        {
            if (fs::is_regular_file(item))
            {
                std::string fileString{releasesPath.string()};
                fileString.append("/");
                fileString.append(item.path().filename().string());
                AudioFile af{};
                af.file = sf_open(fileString.c_str(), SFM_READ, &af.info);

                af.buffer.resize(af.info.frames * af.info.channels);
                sf_read_short(af.file, af.buffer.data(), af.buffer.size());
                sf_close(af.file);

                releases.push_back(af);
            }
        }
    }

    // iterate over files in "softclicks" folder, add each to softClicks vector
    fs::path softClicksPath{"softclicks"};
    if (fs::exists(softClicksPath) && fs::is_directory(softClicksPath))
    {
        for (const auto &item : fs::directory_iterator(softClicksPath))
        {
            if (fs::is_regular_file(item))
            {
                std::string fileString{softClicksPath.string()};
                fileString.append("/");
                fileString.append(item.path().filename().string());
                AudioFile af{};
                af.file = sf_open(fileString.c_str(), SFM_READ, &af.info);

                af.buffer.resize(af.info.frames * af.info.channels);
                sf_read_short(af.file, af.buffer.data(), af.buffer.size());
                sf_close(af.file);

                softClicks.push_back(af);
            }
        }
    }

    // iterate over files in "softreleases" folder, add each to softReleases vector
    fs::path softReleasesPath{"softreleases"};
    if (fs::exists(softReleasesPath) && fs::is_directory(softReleasesPath))
    {
        for (const auto &item : fs::directory_iterator(softReleasesPath))
        {
            if (fs::is_regular_file(item))
            {
                std::string fileString{softReleasesPath.string()};
                fileString.append("/");
                fileString.append(item.path().filename().string());
                AudioFile af{};
                af.file = sf_open(fileString.c_str(), SFM_READ, &af.info);

                af.buffer.resize(af.info.frames * af.info.channels);
                sf_read_short(af.file, af.buffer.data(), af.buffer.size());
                sf_close(af.file);

                softReleases.push_back(af);
            }
        }
    }

    AudioFile click{};
    click.file = sf_open("clicks/1.wav", SFM_READ, &click.info);

    SF_INFO sfinfoRelease;
    SNDFILE *releaseFile = sf_open("release.wav", SFM_READ, &sfinfoRelease);

    SF_INFO sfinfoSoftClick;
    SNDFILE *softClickFile = sf_open("softclick.wav", SFM_READ, &sfinfoSoftClick);

    SF_INFO sfinfoSoftRelease;
    SNDFILE *softReleaseFile = sf_open("softrelease.wav", SFM_READ, &sfinfoSoftRelease);

    // Read the click sample
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
        // set up random click picker
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, clicks.size() - 1);

        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels; // Interleaved
        mix_click(outputBuffer, clicks[distrib(gen)].buffer, sampleIndex, channels);
    }

    // Add releases to output buffer
    for (float t : releaseTimes)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, releases.size() - 1);

        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels;
        mix_click(outputBuffer, releases[distrib(gen)].buffer, sampleIndex, channels);
    }

    // Add soft clicks to output buffer
    for (float t : softClickTimes)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, softClicks.size() - 1);

        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels;
        mix_click(outputBuffer, softClicks[distrib(gen)].buffer, sampleIndex, channels);
    }

    // Add soft releases to output buffer
    for (float t : softReleaseTimes)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, softReleases.size() - 1);

        sf_count_t frameIndex = static_cast<sf_count_t>(t * sampleRate);
        int sampleIndex = frameIndex * channels;
        mix_click(outputBuffer, softReleases[distrib(gen)].buffer, sampleIndex, channels);
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
