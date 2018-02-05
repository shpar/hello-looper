/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HelloLooperAudioProcessorEditor::HelloLooperAudioProcessorEditor (HelloLooperAudioProcessor& p):
AudioProcessorEditor (&p), Thread ("Background Thread"), processor (p)
{
    setSize (300, 200);

    addAndMakeVisible (tempoSlider);
    tempoSlider.setRange (1, 300, 1);
    tempoSlider.setValue (120);
    tempoSlider.setTextBoxIsEditable(true);
    tempoSlider.setTextValueSuffix (" BPM");
    tempoSlider.addListener (this);
    tempoSlider.setEnabled (false);

    addAndMakeVisible (positionSlider);
    positionSlider.setRange (0, 3.0);
    positionSlider.setValue (0);
    positionSlider.setTextBoxIsEditable(true);
    positionSlider.setTextValueSuffix (" s");
    positionSlider.addListener (this);
    positionSlider.setEnabled (false);

    addAndMakeVisible (openButton);
    openButton.setButtonText ("Open");
    openButton.addListener (this);

    addAndMakeVisible (clearButton);
    clearButton.setButtonText ("Clear");
    clearButton.addListener (this);
    clearButton.setEnabled (false);

    addAndMakeVisible (pauseButton);
    pauseButton.setButtonText ("Pause");
    pauseButton.addListener (this);
    pauseButton.setEnabled (false);

    addAndMakeVisible (setButton);
    setButton.setButtonText ("Set");
    setButton.addListener (this);
    setButton.setEnabled (false);
    setButton.setClickingTogglesState (true);

    addAndMakeVisible (syncTempoButton);
    syncTempoButton.setButtonText ("Sync Tempo");
    syncTempoButton.addListener (this);
    syncTempoButton.setEnabled (false);
    syncTempoButton.setToggleState(false, dontSendNotification);
    syncTempoButton.setClickingTogglesState (true);

    addAndMakeVisible (syncBeatButton);
    syncBeatButton.setButtonText ("Sync Beat");
    syncBeatButton.addListener (this);
    syncBeatButton.setEnabled (false);
    syncBeatButton.setToggleState(false, dontSendNotification);
    syncBeatButton.setClickingTogglesState (true);

    for (int i = 0; i < 4; i++)
    {
        hotkeys.add (new Hotkey(i + 1));
        addAndMakeVisible (*hotkeys[i]);
        hotkeys[i]->setButtonText (std::to_string(i+1));
        hotkeys[i]->addListener (this);
        hotkeys[i]->setEnabled (false);
    }

    for (int i = 0; i < 4; i++)
    {
        if (hotkeys[i] != nullptr)
        {
            (hotkeys[i])->setBounds (10 + i * (getWidth() - 20) / 4, 130,
                                     (getWidth() - 20) / 4, 20);
        }
    }
    formatManager.registerBasicFormats();
    startThread();
}

HelloLooperAudioProcessorEditor::~HelloLooperAudioProcessorEditor()
{
    stopThread (4000);
}

//==============================================================================
void HelloLooperAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void HelloLooperAudioProcessorEditor::resized()
{
    int mainComponentWidth = this->getWidth();

    openButton.setBounds (10, 10, (mainComponentWidth - 20) / 2, 20);
    clearButton.setBounds ((mainComponentWidth - 20) / 2 + 10, 10, (mainComponentWidth - 20) / 2, 20);
    pauseButton.setBounds (10, 40, (mainComponentWidth - 20) / 2, 20);
    setButton.setBounds ((mainComponentWidth - 20) / 2 + 10, 40, (mainComponentWidth - 20) / 2, 20);
    tempoSlider.setBounds (10, 70, getWidth() - 20, 20);
    positionSlider.setBounds (10, 100, getWidth() - 20, 20);

    for (int i = 0; i < hotkeys.size(); i++)
    {
        if (hotkeys[i] != NULL)
        {
            hotkeys[i]->setBounds ((mainComponentWidth - 20) * i / 4 + 10, 130,
                                   (mainComponentWidth - 20) / 4, 20);
        }
    }
    syncTempoButton.setBounds (10, 160, (mainComponentWidth - 20) / 2, 20);
    syncBeatButton.setBounds ((mainComponentWidth - 20) / 2 + 10, 160, (mainComponentWidth - 20) / 2, 20);
}

void HelloLooperAudioProcessorEditor::run()
{
    while (!threadShouldExit())
    {
        checkForPathToOpen();
        checkForBuffersToFree();
        wait (500);
    }
}

void HelloLooperAudioProcessorEditor::checkForBuffersToFree()
{
    for (int i = processor.buffers.size(); --i >= 0;)
    {
        ReferenceCountedBuffer::Ptr buffer (processor.buffers.getUnchecked (i));

        if (buffer->getReferenceCount() == 2)
            processor.buffers.remove (i);
    }
}

void HelloLooperAudioProcessorEditor::checkForPathToOpen()
{
    String pathToOpen;
    pathToOpen.swapWith(chosenPath);

    if (pathToOpen.isNotEmpty())
    {
        const File file (pathToOpen);
        ScopedPointer<AudioFormatReader> reader (formatManager.createReaderFor (file));

        if (reader != nullptr)
        {
            int duration = reader->lengthInSamples / reader->sampleRate;
            // limit samples to 60 seconds
            duration = jmin (60, duration);

            positionSlider.setRange(0, duration);

            ReferenceCountedBuffer::Ptr newBuffer
            = new ReferenceCountedBuffer (reader->numChannels,
                                              reader->lengthInSamples);
            reader->read (newBuffer->getAudioSampleBuffer(),
                              0,
                              duration * reader->sampleRate,
                              0,
                              true,
                              true);
            processor.currentBuffer = newBuffer;
            processor.buffers.add (newBuffer);
        }
    }
}

void HelloLooperAudioProcessorEditor::updatePosition()
{
    processor.positionSamples = positionSlider.getValue() * processor.currentSampleRate;
}

void HelloLooperAudioProcessorEditor::timerCallback()
{
    updateTempo(processor.infoFromHost);
}

void HelloLooperAudioProcessorEditor::updateTempo(AudioPlayHead::CurrentPositionInfo currentInfoFromHost)
{
    // update tempo from host
    if (syncTempoButton.getToggleState())
    {
        double currentBpm = currentInfoFromHost.bpm;
        tempoSlider.setValue(currentBpm);
        tempoSliderChanged();
    }
    // sync beat with host
    if (currentInfoFromHost.isPlaying && syncBeatButton.getToggleState())
    {
        int64 timeInSamples = currentInfoFromHost.timeInSamples;
        processor.syncOffsetSamples = processor.samplesPerBeat - timeInSamples %
                                    static_cast<int64>(processor.samplesPerBeat);
        if (timerCountdown % 30 == 0)
        {
            processor.syncBeat = true;
            timerCountdown = 0;
        }
    }
    else
    {
        processor.syncOffsetSamples = 0;
    }
    ++timerCountdown;
}

void HelloLooperAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &openButton)      openButtonClicked();
    if (button == &clearButton)     clearButtonClicked();
    if (button == &pauseButton)     pauseButtonClicked();
    if (button == &setButton)       setButtonClicked();
    if (button == &syncTempoButton) syncTempoButtonClicked();
    if (button == &syncBeatButton) syncBeatButtonClicked();
    for (int i = 0; i < hotkeys.size(); i++)
    {
        if (button == hotkeys[i])
        {
            hotkeyClicked(i);
        }
    }
}

void HelloLooperAudioProcessorEditor::openButtonClicked ()
{

    FileChooser chooser ("Select a Wave file...", File::nonexistent, "*.wav");

    if (chooser.browseForFileToOpen())
    {
        const File file (chooser.getResult());
        String path (file.getFullPathName());
        chosenPath.swapWith(path);
        notify();
        clearButton.setEnabled (true);
        pauseButton.setEnabled (true);
        setButton.setEnabled (true);
        positionSlider.setEnabled (true);
        if (!syncTempoButton.getToggleState())
        {
            tempoSlider.setEnabled (true);
        }
        syncTempoButton.setEnabled (true);
        syncBeatButton.setEnabled (true);
    }
}

void HelloLooperAudioProcessorEditor::clearButtonClicked ()
{
    processor.currentBuffer = nullptr;
    clearButton.setEnabled (false);
    pauseButton.setEnabled (false);
    setButton.setEnabled (false);
    positionSlider.setEnabled (false);
    tempoSlider.setEnabled (false);
    syncTempoButton.setEnabled (false);
    syncBeatButton.setEnabled (false);
    for (auto hotkey : hotkeys)
    {
        hotkey->setEnabled (false);
        hotkey->setInactive();
        hotkey->setColour(0x1000100, findColour(0x1000100, false));
    }
    processor.setButtonOn = false;
}

void HelloLooperAudioProcessorEditor::pauseButtonClicked ()
{
    if (processor.playing)
    {
        processor.playing = false;
        pauseButton.setButtonText("Play");
    }
    else
    {
        processor.playing = true;
        pauseButton.setButtonText("Pause");
        if (!syncTempoButton.getToggleState())
        {
            tempoSlider.setEnabled (true);
        }
        if (syncBeatButton.getToggleState())
        {
            timerCountdown = 0;
        }
    }
}

void HelloLooperAudioProcessorEditor::setButtonClicked ()
{
    (processor.setButtonOn == false) ? processor.setButtonOn = true : processor.setButtonOn = false;
    for (auto& hotkey : hotkeys)
    {
        hotkey->setEnabled (true);
    }
}

void HelloLooperAudioProcessorEditor::syncTempoButtonClicked ()
{
    if (syncTempoButton.getToggleState())
    {
        tempoSlider.setEnabled (false);
        startTimerHz(30);
    }
    else if (!syncTempoButton.getToggleState())
    {
        tempoSlider.setEnabled (true);
        syncBeatButton.setToggleState(false, dontSendNotification);
    }
}

void HelloLooperAudioProcessorEditor::syncBeatButtonClicked ()
{
    if (!syncTempoButton.getToggleState() && syncBeatButton.getToggleState())
    {
        syncTempoButton.setToggleState(true, dontSendNotification);
        syncTempoButtonClicked();
    }
    if (syncBeatButton.getToggleState())
    {
        processor.syncBeat = true;
    }
}

void HelloLooperAudioProcessorEditor::hotkeyClicked (int hotkeyId)
{
    if (processor.setButtonOn)
    {
        hotkeys[hotkeyId]->setPosition(positionSlider.getValue());
        hotkeys[hotkeyId]->setActive();
        hotkeys[hotkeyId]->setColour(0x1000100, Colour(0, 0, 0));
        setButton.setToggleState(false, dontSendNotification);
        processor.setButtonOn = false;
    }
    else
    {
        positionSlider.setValue (hotkeys[hotkeyId]->getPosition());
        updatePosition();
        if (syncBeatButton.getToggleState())
        {
            timerCountdown = 0;
        }
    }
    for (auto &hotkey : hotkeys)
    {
        if (!hotkey->isActive())
        {
            hotkey->setEnabled (false);
        }
    }
}

void HelloLooperAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &tempoSlider)     tempoSliderChanged();
    if (slider == &positionSlider)  positionSliderChanged();
}

void HelloLooperAudioProcessorEditor::positionSliderChanged()
{
    if (processor.currentSampleRate > 0.0)
        updatePosition();
}

void HelloLooperAudioProcessorEditor::tempoSliderChanged ()
{
    processor.samplesPerBeat = static_cast<int>(((60 / tempoSlider.getValue()) *
                                                 processor.currentSampleRate));
}
