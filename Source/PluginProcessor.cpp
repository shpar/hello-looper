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
#include "RenderAudio.h"

//==============================================================================
HelloLooperAudioProcessor::HelloLooperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
                         ),
      currentSampleRate(44100),
      samplesPerBeat(22050),
      positionSamples(0),
      syncOffsetSamples(0),
      playing(true),
      setButtonOn(false),
      syncBeat(true),
      state(*this,
            nullptr,
            "state",
            {std::make_unique<AudioParameterFloat>(
                 "tempo",
                 "Tempo",
                 NormalisableRange<float>(1.0f, 300.0f),
                 120.0f,
                 String(),
                 AudioProcessorParameter::genericParameter,
                 [](float value, int max_string_length) { return String(value, 0); }),
             std::make_unique<AudioParameterFloat>(
                 "position",
                 "Position",
                 NormalisableRange<float>(0.0f, 1.0f),
                 0.0f,
                 String(),
                 AudioProcessorParameter::genericParameter,
                 [](float value, int max_string_length) { return String(value, 2); })})
#endif
{
    infoFromHost.resetToDefault();
}

HelloLooperAudioProcessor::~HelloLooperAudioProcessor() {}

//==============================================================================
const String HelloLooperAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool HelloLooperAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool HelloLooperAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool HelloLooperAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double HelloLooperAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int HelloLooperAudioProcessor::getNumPrograms() {
    return 1;
}

int HelloLooperAudioProcessor::getCurrentProgram() {
    return 0;
}

void HelloLooperAudioProcessor::setCurrentProgram(int index) {}

const String HelloLooperAudioProcessor::getProgramName(int index) {
    return {};
}

void HelloLooperAudioProcessor::changeProgramName(int index, const String& newName) {}

//==============================================================================
void HelloLooperAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    currentSampleRate = sampleRate;
}

void HelloLooperAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HelloLooperAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;
#endif

    return true;
#endif
}
#endif

void HelloLooperAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) {
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    Sampler.processAudio(buffer, currentBuffer, totalNumInputChannels, totalNumOutputChannels,
                         playing, samplesPerBeat, positionSamples, syncOffsetSamples, syncBeat);
    syncBeat = false;
    getPlaybackPositionFromHost();
}

//==============================================================================
bool HelloLooperAudioProcessor::hasEditor() const {
    return true;
}

AudioProcessorEditor* HelloLooperAudioProcessor::createEditor() {
    return new HelloLooperAudioProcessorEditor(*this);
}

//==============================================================================
void HelloLooperAudioProcessor::getStateInformation(MemoryBlock& destData) {
    // Store an xml representation of our state.
    std::unique_ptr<XmlElement> xmlState(state.copyState().createXml());

    if (xmlState.get() != nullptr) copyXmlToBinary(*xmlState, destData);
}

void HelloLooperAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // Restore our plug-in's state from the xml representation stored in the above
    // method.
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) state.replaceState(ValueTree::fromXml(*xmlState));
}

void HelloLooperAudioProcessor::updatePosition() {}

void HelloLooperAudioProcessor::getPlaybackPositionFromHost() {
    if (AudioPlayHead* currentPlayHead = getPlayHead()) {
        AudioPlayHead::CurrentPositionInfo newInfoFromHost;

        if (currentPlayHead->getCurrentPosition(newInfoFromHost)) {
            infoFromHost = newInfoFromHost;
            return;
        }
    }
    infoFromHost.resetToDefault();
}

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new HelloLooperAudioProcessor();
}
