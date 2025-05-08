# WaffleBot, a clickbot for Geometry Dash written in C++
# Features
Generates clicks to a Geometry Dash level macro with support for both normal clicks and soft clicks. Supports both Mega Hack Replay JSON (2.2), xdBot JSON (2.2), and TASBot macros (2.113). Also features a special "softClickAfterRelease" parameter in which it will softclick if you do a micro-release in the macro, to make it more realistic.
Compatible with both Windows and Linux x86-64.
## Usage
- Make sure you at least have a clicks folder in the same directory as the program. Supported folders are **"clicks"**, **"releases"**, **"softclicks"** and **"softreleases"**.
- Also make sure you have a properly formatted **config.json** file, with 2 parameters: 
**"softclickTime"**, followed by the threshold in seconds between the current click and the last click for the current click to be soft.
**"softclickAfterReleaseTime"**, followed by the threshold in seconds between the previous release and the current click for the current click to be soft. 
### Windows
Drag macros you want to generate clicks for on top of the .exe. Audio file(s) will appear in the same directory as the program.
### Linux
Launch the executable in a terminal, passing the filepaths of each macro as command-line arguments.
## Building info
Make sure to have external includes set to the /include folder and have -lsndfile as a command-line argument when building.
