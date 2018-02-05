/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "RenderAudio.h"
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

    void getPlaybackPositionFromHost();

private:
    //==============================================================================
    RenderAudio Sampler;
    void updatePosition();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelloLooperAudioProcessor)
};
