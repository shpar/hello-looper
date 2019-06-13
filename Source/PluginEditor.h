/*
    hello looper - a simple one-beat sampler
    Copyright (C) 2019 Dan Grahelj

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Hotkey.h"
#include "KeyAnalyzer.h"
#include "PluginProcessor.h"
#include "ReferenceCountedObject.h"

//==============================================================================
class HelloLooperAudioProcessorEditor : public AudioProcessorEditor,
                                        public ReferenceCountedObject,
                                        private Slider::Listener,
                                        private Button::Listener,
                                        private Thread,
                                        private Timer,
                                        private ChangeListener {
public:
    HelloLooperAudioProcessorEditor(HelloLooperAudioProcessor&);
    ~HelloLooperAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    void timerCallback() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    HelloLooperAudioProcessor& processor;

    void run() override;
    void checkForBuffersToFree();
    void checkForPathToOpen();
    void updatePosition();
    void openButtonClicked();
    void clearButtonClicked();
    void pauseButtonClicked();
    void setButtonClicked();
    void exportButtonClicked();
    void analyzeButtonClicked();
    void syncTempoButtonClicked();
    void syncBeatButtonClicked();
    void hotkeyClicked(int hotkeyId);
    void positionSliderChanged();
    void tempoSliderChanged();
    void updateTempo(AudioPlayHead::CurrentPositionInfo);
    void thumbnailChanged();
    void paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);

    Slider tempoSlider;
    Slider positionSlider;
    AudioProcessorValueTreeState::SliderAttachment tempoAttachment, positionAttachment;

    TextButton openButton, clearButton, pauseButton, setButton, exportButton, analyzeButton;
    ToggleButton syncTempoButton, syncBeatButton;

    OwnedArray<Hotkey> hotkeys;
    String chosenPath;
    AudioFormatManager formatManager;

    int timerCountdown;
    int sampleDuration;

    KeyAnalyzer chord_analyzer;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelloLooperAudioProcessorEditor)
};
