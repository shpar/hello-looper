/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "ReferenceCountedObject.h"
#include "Hotkey.h"


//==============================================================================
class HelloLooperAudioProcessorEditor  :    public AudioProcessorEditor,
                                            public ReferenceCountedObject,
                                            private Slider::Listener,
                                            private Button::Listener,
                                            private Thread,
                                            private Timer
{
public:
    HelloLooperAudioProcessorEditor (HelloLooperAudioProcessor&);
    ~HelloLooperAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    void timerCallback () override;

private:
    HelloLooperAudioProcessor& processor;

    void run() override;
    void checkForBuffersToFree ();
    void checkForPathToOpen ();
    void updatePosition();
    void openButtonClicked ();
    void clearButtonClicked ();
    void pauseButtonClicked ();
    void setButtonClicked ();
    void syncTempoButtonClicked ();
    void syncBeatButtonClicked ();
    void hotkeyClicked (int hotkeyId);
    void positionSliderChanged ();
    void tempoSliderChanged ();
    void updateTempo (AudioPlayHead::CurrentPositionInfo);

    Slider tempoSlider;
    Slider positionSlider;
    TextButton openButton, clearButton, pauseButton, setButton;
    ToggleButton syncTempoButton, syncBeatButton;

    OwnedArray<Hotkey> hotkeys;
    String chosenPath;
    AudioFormatManager formatManager;

    int timerCountdown;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelloLooperAudioProcessorEditor)
};
