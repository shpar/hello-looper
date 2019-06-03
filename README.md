# hello looper

*‘I’m quite bad at listening to long pieces – I’ve got a pretty shit attention span.‘ - Mica Levi*

A simple one beat looper with pause/play functionality, tempo and start time sliders and four hotkeys to save positions in the file. It also allows to sync the tempo and beat to the host DAW and to export the current beat as a wave file.

### v1.0.1

Added feature to analyze musical key of beat

## How to build

1. Download [JUCE](https://juce.com/get-juce)
1. Download and build [libKeyFinder](https://github.com/ibsh/libKeyFinder) to libKeyFinder/
2. Open the hello looper.jucer file in Projucer
3. File->Save And Open In IDE...
4. Build plugin or standalone in your IDE of choice

## TODO
- [ ] Fade out/in to avoid clicks
- [x] Harmonic analysis
- [x] Save plugin states
- [x] Add plugin parameters
