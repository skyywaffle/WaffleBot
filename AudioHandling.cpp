#include "AudioHandling.h"

#include <filesystem>
#include <iostream>
#include <random>
#include <sndfile.h>
#include <vector>

#include "Input.h"
#include "Macro.h"

namespace fs = std::filesystem;

struct AudioFile {
    SF_INFO info;
    SNDFILE *file;
    std::vector<short> buffer;
};

void mix_click(std::vector<short> &outBuffer, const std::vector<short> &click, int insertIndex, int channels) {
    for (size_t i = 0; i < click.size(); ++i) {
        size_t outIndex = insertIndex + i;
        if (outIndex < outBuffer.size()) {
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

std::vector<AudioFile> getAudioFiles(const char *folderName) {
    std::vector<AudioFile> files{};

    fs::path folderPath{folderName};
    if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
        for (const auto &item : fs::directory_iterator(folderPath)) {
            // Construct the filename and add a new AudioFile to the files vector
            if (fs::is_regular_file(item)) {
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
    else { // if the folder name is invalid
        std::cerr << "Could not find folder name: " << folderName << '\n';
    }
    return files;
}

void addToBuffer(std::vector<float> &inputTimes, std::vector<AudioFile> &files, std::vector<short> &buffer, int sampleRate, int channels) {
    // Add clicks to output buffer
    for (float t : inputTimes) {
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
bool generateAudio(Macro &macro) {
    bool isTwoPlayerMacro {macro.isTwoPlayer()};

    std::vector<AudioFile> p1Clicks{};
    std::vector<AudioFile> p1Releases{};
    std::vector<AudioFile> p1SoftClicks{};
    std::vector<AudioFile> p1SoftReleases{};

    std::vector<AudioFile> p2Clicks{};
    std::vector<AudioFile> p2Releases{};
    std::vector<AudioFile> p2SoftClicks{};
    std::vector<AudioFile> p2SoftReleases{};

    // Get click audio files
    p1Clicks = getAudioFiles("player_1/clicks");
    p1Releases = getAudioFiles("player_1/releases");
    p1SoftClicks = getAudioFiles("player_1/softclicks");
    p1SoftReleases = getAudioFiles("player_1/softreleases");

    if (p1Clicks.size() == 0) {
        std::cerr << "Error: P1 clicks not found.\n";
        return false;
    }
    if (p1Releases.size() == 0) {
        std::cout << "Warning: P1 releases not found.\n";
    }
    if (p1SoftClicks.size() == 0) {
        std::cout << "Warning: P1 softclicks not found, using normal clicks.\n";
        p1SoftClicks = p1Clicks;
    }
    if (p1SoftReleases.size() == 0) {
        std::cout << "Warning: P1 softreleases not found, using normal releases.\n";
        p1SoftReleases = p1Releases;
    }

    if (isTwoPlayerMacro) {
        p2Clicks = getAudioFiles("player_2/clicks");
        p2Releases = getAudioFiles("player_2/releases");
        p2SoftClicks = getAudioFiles("player_2/softclicks");
        p2SoftReleases = getAudioFiles("player_2/softreleases");

        if (p2Clicks.size() == 0) {
            std::cerr << "Error: P2 clicks not found.\n";
            return false;
        }
        if (p2Releases.size() == 0) {
            std::cout << "Warning: P2 releases not found.\n";
        }
        if (p2SoftClicks.size() == 0) {
            std::cout << "Warning: P2 softclicks not found, using normal clicks.\n";
            p2SoftClicks = p2Clicks;
        }
        if (p2SoftReleases.size() == 0) {
            std::cout << "Warning: P2 softreleases not found, using normal releases.\n";
            p2SoftReleases = p2Releases;
        }
    }

    // Define the total duration in seconds of the output file
    float durationSeconds = (float)macro.getFrameCount() / macro.getFps() + 2; // add an extra 2 seconds so no releases get cut off
    int sampleRate = p1Clicks[0].info.samplerate;
    int channels = p1Clicks[0].info.channels;
    sf_count_t totalFrames = static_cast<sf_count_t>(durationSeconds * sampleRate);
    std::vector<short> outputBuffer(totalFrames * channels, 0); // Silent buffer

    // Define time points where clicks and releases (and soft clicks/releases) occur (in seconds)
    std::vector<float> p1ClickTimes{};
    std::vector<float> p1ReleaseTimes{};
    std::vector<float> p1SoftClickTimes{};
    std::vector<float> p1SoftReleaseTimes{};

    std::vector<float> p2ClickTimes{};
    std::vector<float> p2ReleaseTimes{};
    std::vector<float> p2SoftClickTimes{};
    std::vector<float> p2SoftReleaseTimes{};

    // Add the times of inputs to their corresponding vectors
    for (Action action : macro.getActions()) {
        for (Input input : action.getPlayerOneInputs()) {
            if (input.isPressed()) {
                if (input.getClickType() == ClickType::NORMAL) {
                    p1ClickTimes.push_back(action.getFrame() / (float)macro.getFps());
                }
                else if (input.getClickType() == ClickType::SOFT) {
                    p1SoftClickTimes.push_back(action.getFrame() / (float)macro.getFps());
                }
            }
            else {
                if (input.getClickType() == ClickType::NORMAL) {
                    p1ReleaseTimes.push_back(action.getFrame() / (float)macro.getFps());
                }
                else if (input.getClickType() == ClickType::SOFT) {
                    p1SoftReleaseTimes.push_back(action.getFrame() / (float)macro.getFps());
                }
            }
        }

        if (isTwoPlayerMacro) {
            for (Input input : action.getPlayerTwoInputs()) {
                if (input.isPressed()) {
                    if (input.getClickType() == ClickType::NORMAL) {
                        p2ClickTimes.push_back(action.getFrame() / (float)macro.getFps());
                    }
                    else if (input.getClickType() == ClickType::SOFT) {
                        p2SoftClickTimes.push_back(action.getFrame() / (float)macro.getFps());
                    }
                }
                else {
                    if (input.getClickType() == ClickType::NORMAL) {
                        p2ReleaseTimes.push_back(action.getFrame() / (float)macro.getFps());
                    }
                    else if (input.getClickType() == ClickType::SOFT) {
                        p2SoftReleaseTimes.push_back(action.getFrame() / (float)macro.getFps());
                    }
                }
            }
        }
    }

    // Add click sounds to the output buffer
    addToBuffer(p1ClickTimes, p1Clicks, outputBuffer, sampleRate, channels);
    addToBuffer(p1ReleaseTimes, p1Releases, outputBuffer, sampleRate, channels);
    addToBuffer(p1SoftClickTimes, p1SoftClicks, outputBuffer, sampleRate, channels);
    addToBuffer(p1SoftReleaseTimes, p1SoftReleases, outputBuffer, sampleRate, channels);

    if (isTwoPlayerMacro) {
        addToBuffer(p2ClickTimes, p2Clicks, outputBuffer, sampleRate, channels);
        addToBuffer(p2ReleaseTimes, p2Releases, outputBuffer, sampleRate, channels);
        addToBuffer(p2SoftClickTimes, p2SoftClicks, outputBuffer, sampleRate, channels);
        addToBuffer(p2SoftReleaseTimes, p2SoftReleases, outputBuffer, sampleRate, channels);
    }

    // Write to new WAV file
    SF_INFO sfinfoOut = p1Clicks[0].info;
    sfinfoOut.frames = totalFrames;
    std::string macroName {macro.getModifiableName()};
    SNDFILE *outFile = sf_open(macroName.append(".wav").c_str(), SFM_WRITE, &sfinfoOut);

    if (!outFile) {
        std::cerr << "Error creating output file for " << macroName << '\n';
        return false;
    }

    sf_write_short(outFile, outputBuffer.data(), outputBuffer.size());
    sf_close(outFile);

    std::cout << "Successfully generated clicks for " << macroName << '\n';

    return true;
}
