# WaffleBot, a clickbot for Geometry Dash written in C++
# Features
Generates clicks to a Geometry Dash level macro (***both classic AND platformer macros!***) with support for both normal clicks and soft clicks. Supports Mega Hack Replay JSON (2.2), xdBot JSON (2.2), and TASBot JSON macros (2.113). Also features a special "softClickAfterRelease" parameter in which it will softclick if you do a micro-release in the macro, to make it more realistic.
Compatible with both Windows and Linux x86-64.
## Usage
- In the same directory as the executable, create a **player_1** and **player_2** folder, and in each of those, have your various click folders, naming scheme is below. You **must** have at least one click and release for each player and each button if it's a platformer macro. Currently only supports **.wav** audio files.
- Supported click sound folders: 
  - **clicks**
  - **releases**
  - **softclicks**
  - **softreleases**
  - **leftclicks**
  - **leftreleases**
  - **leftsoftclicks**
  - **leftsoftreleases**
  - **rightclicks**
  - **rightreleases**
  - **rightsoftclicks**
  - **rightsoftreleases**
- Also make sure you have a properly formatted **config.json** file, with 2 parameters: 
**"softclickTime"**, followed by the threshold in seconds between the current click and the last click for the current click to be soft.
**"softclickAfterReleaseTime"**, followed by the threshold in seconds between the previous release and the current click for the current click to be soft. 
### Windows
Drag macros you want to generate clicks for on top of the .exe. Generated audio file(s) will appear in the same directory as the program.
### Linux
Launch the executable in a terminal, passing the filepaths of each macro as command-line arguments.
