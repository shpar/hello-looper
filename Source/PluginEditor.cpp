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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "keyfinder.h"

//==============================================================================
HelloLooperAudioProcessorEditor::HelloLooperAudioProcessorEditor (HelloLooperAudioProcessor& p):
AudioProcessorEditor (&p), Thread ("Background Thread"), processor (p), tempoAttachment (p.state, "tempo",  tempoSlider),
              positionAttachment (p.state, "position", positionSlider), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache)
{
    setSize (400, 400);

    thumbnail.addChangeListener(this);

    addAndMakeVisible (tempoSlider);
    tempoSlider.setRange (1, 300, 1);
    tempoSlider.setValue (120);
    tempoSlider.setTextBoxIsEditable(true);
    tempoSlider.setTextValueSuffix (" BPM");
    tempoSlider.addListener (this);
    tempoSlider.setEnabled (false);
    tempoSlider.setNumDecimalPlacesToDisplay(1);

    addAndMakeVisible (positionSlider);
    positionSlider.setRange (0, 1.0);
    positionSlider.setValue (0);
    positionSlider.setTextBoxIsEditable(true);
    positionSlider.setTextValueSuffix (" s");
    positionSlider.addListener (this);
    positionSlider.setEnabled (false);
    positionSlider.setNumDecimalPlacesToDisplay(8);

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

    addAndMakeVisible (exportButton);
    exportButton.setButtonText("Export loop");
    exportButton.addListener(this);
    exportButton.setEnabled(false);

    addAndMakeVisible (analyzeButton);
    analyzeButton.setButtonText("Analyze");
    analyzeButton.addListener(this);
    analyzeButton.setEnabled(false);

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
    Rectangle<int> thumbnailBounds (10, 190, getWidth() - 20, 200);
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded (g, thumbnailBounds);
    else
        paintIfFileLoaded (g, thumbnailBounds);
}

void HelloLooperAudioProcessorEditor::paintIfNoFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour (Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

void HelloLooperAudioProcessorEditor::paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour (Colours::black);
    g.fillRect (thumbnailBounds);
    g.setColour (Colours::white);
    thumbnail.drawChannels (g,
                            thumbnailBounds,
                            0.0,
                            thumbnail.getTotalLength(),
                            1.0f);
//    g.setColour (Colours::red);
    auto audioPositionPercent = (processor.positionSamples / sampleDuration / processor.currentSampleRate);
    auto audioLoopLengthPercent = (processor.samplesPerBeat / sampleDuration / processor.currentSampleRate);
    auto drawPosition (audioPositionPercent * thumbnailBounds.getWidth()
                       + thumbnailBounds.getX());
//    g.drawLine (drawPosition, thumbnailBounds.getY(), drawPosition,
//                thumbnailBounds.getBottom(), 2.0f);
    DBG("audioPositionPercent" << audioPositionPercent << " drawPosition " << drawPosition);
    auto transparent_blue = Colours::blue;
    g.setColour(transparent_blue.withAlpha(0.5f));
    Rectangle<int> thumbnailLoopRect (drawPosition, thumbnailBounds.getY(), audioLoopLengthPercent * thumbnailBounds.getWidth(), thumbnailBounds.getHeight());
    g.fillRect (thumbnailLoopRect);
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
    syncTempoButton.setBounds (10, 160, (mainComponentWidth - 20) / 4, 20);
    syncBeatButton.setBounds ((mainComponentWidth - 20) / 4 + 10, 160, (mainComponentWidth - 20) / 4, 20);
    exportButton.setBounds((mainComponentWidth - 20) * 2 / 4 + 10, 160, (mainComponentWidth - 20) / 4, 20);
    analyzeButton.setBounds((mainComponentWidth - 20) * 3 / 4 + 10, 160, (mainComponentWidth - 20) / 4, 20);
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
            sampleDuration = duration;

            DBG("sample duration " << duration);

//            positionSlider.setRange(0, duration);

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
            {
                const MessageManagerLock mmLock;
                thumbnail.setSource(new FileInputSource (file));
            }
        }
    }
}

void HelloLooperAudioProcessorEditor::updatePosition()
{
    processor.positionSamples = positionSlider.getValue() * processor.currentSampleRate * sampleDuration;
    repaint();
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
    if (button == &exportButton)    exportButtonClicked();
    if (button == &analyzeButton)    analyzeButtonClicked();
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
        exportButton.setEnabled (true);
        analyzeButton.setEnabled (true);
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
    exportButton.setEnabled (false);
    analyzeButton.setEnabled (false);
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

void HelloLooperAudioProcessorEditor::exportButtonClicked ()
{
    FileChooser chooser ("Save loop as...",
                         File(),
                         "*.wav");
    if (chooser.browseForFileToSave(true))
    {
        auto file = chooser.getResult();
        FileOutputStream* fos = new FileOutputStream(file);

        double systemSampleRate = processor.currentSampleRate;
        std::unique_ptr<AudioFormatWriter> writer (WavAudioFormat().createWriterFor(fos, systemSampleRate, 2, 16, nullptr, 0));
        if (writer.get() != nullptr)
        {
            ReferenceCountedBuffer::Ptr retainedCurrentBuffer (processor.currentBuffer);
            AudioSampleBuffer* currentAudioSampleBuffer (retainedCurrentBuffer->getAudioSampleBuffer());
            int position = positionSlider.getValue() * systemSampleRate * sampleDuration;
            int numSamples =  systemSampleRate * 60 / tempoSlider.getValue();
            writer->writeFromAudioSampleBuffer(*currentAudioSampleBuffer, position, numSamples);
        }
    }
}

void HelloLooperAudioProcessorEditor::analyzeButtonClicked ()
{
    static KeyFinder::KeyFinder key_finder_object;
    int position_scanned = 0;
    for (int i = 0; i < 10; ++i) {
        chord_analyzer.analyze(processor.currentSampleRate, 2, processor.samplesPerBeat, processor.currentBuffer, position_scanned);
        auto r =  key_finder_object.keyOfAudio(chord_analyzer.audio_data);
        chord_analyzer.sample_analysis.emplace(position_scanned, r);
        DBG("key is " << key_name[r]);
        position_scanned += processor.samplesPerBeat;
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
    int samples_expected = ((60 / tempoSlider.getValue()) *
                                                 processor.currentSampleRate);
//    int n_samples = processor.currentSampleRate * sampleDuration;

    processor.samplesPerBeat = static_cast<int>(samples_expected);
    repaint();
}

void HelloLooperAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &thumbnail)       thumbnailChanged();
}

void HelloLooperAudioProcessorEditor::thumbnailChanged()
{
    repaint();
}
