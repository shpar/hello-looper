/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RenderAudio.h"


//==============================================================================
HelloLooperAudioProcessor::HelloLooperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), currentPosition(0), currentSampleRate(44100), samplesPerBeat(22050),
                        positionSamples(0), syncOffsetSamples(0), playing(true), setButtonOn(false),
                        syncBeat(true)
#endif
{
    infoFromHost.resetToDefault();
}

HelloLooperAudioProcessor::~HelloLooperAudioProcessor()
{
}

//==============================================================================
const String HelloLooperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HelloLooperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HelloLooperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HelloLooperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HelloLooperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HelloLooperAudioProcessor::getNumPrograms()
{
    return 1;
}

int HelloLooperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HelloLooperAudioProcessor::setCurrentProgram (int index)
{
}

const String HelloLooperAudioProcessor::getProgramName (int index)
{
    return {};
}

void HelloLooperAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void HelloLooperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
}

void HelloLooperAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HelloLooperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HelloLooperAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    Sampler.processAudio(buffer, currentBuffer, totalNumInputChannels, totalNumOutputChannels,
                         playing, samplesPerBeat, positionSamples, syncOffsetSamples, syncBeat);
    syncBeat = false;
    getPlaybackPositionFromHost();
}

//==============================================================================
bool HelloLooperAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* HelloLooperAudioProcessor::createEditor()
{
    return new HelloLooperAudioProcessorEditor (*this);
}

//==============================================================================
void HelloLooperAudioProcessor::getStateInformation (MemoryBlock& destData)
{
}

void HelloLooperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

void HelloLooperAudioProcessor::updatePosition()
{
}

void HelloLooperAudioProcessor::getPlaybackPositionFromHost()
{
    if (AudioPlayHead* currentPlayHead = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo newInfoFromHost;

        if (currentPlayHead->getCurrentPosition (newInfoFromHost))
        {
            infoFromHost = newInfoFromHost;
            return;
        }
    }
    infoFromHost.resetToDefault();
}

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HelloLooperAudioProcessor();
}
