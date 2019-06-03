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
#include "RenderAudio.h"
#include "Analyzer.h"
#include "ReferenceCountedObject.h"


//==============================================================================

class HelloLooperAudioProcessor  : public AudioProcessor,
                                    public ReferenceCountedObject
{
public:
    //==============================================================================
    HelloLooperAudioProcessor();
    ~HelloLooperAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    double currentPosition, currentSampleRate, samplesPerBeat;
    int positionSamples, syncOffsetSamples;
    bool playing, setButtonOn, syncBeat;
    ReferenceCountedArray<ReferenceCountedBuffer> buffers;
    ReferenceCountedBuffer::Ptr currentBuffer;

    AudioPlayHead::CurrentPositionInfo infoFromHost;
    AudioProcessorValueTreeState state;


    void getPlaybackPositionFromHost();

private:
    //==============================================================================
    RenderAudio Sampler;
    Analyzer ChordAnalyzer;
    void updatePosition();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelloLooperAudioProcessor)
};
