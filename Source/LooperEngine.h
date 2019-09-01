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
#include "ReferenceCountedObject.h"

struct LooperEngineParameters {
    int totalNumInputChannels = 2;
    int totalNumOutputChannels = 2;
    bool playing = false;
    int samplesPerBeat = 512;
    int positionSamples = 0;
    int syncOffsetSamples = 0;
    bool syncBeat = false;
};

class LooperEngine : public ReferenceCountedObject {
public:
    LooperEngine(){};
    ~LooperEngine(){};
    void updateProcessingParameters(LooperEngineParameters& inputParameters) {
        currentParameters = inputParameters;
    }
    void processAudio(AudioSampleBuffer& bufferToFill,
                      ReferenceCountedBuffer::Ptr& currentBuffer) {

        const int& totalNumInputChannels = currentParameters.totalNumInputChannels;
        const int& totalNumOutputChannels = currentParameters.totalNumOutputChannels;
        const bool& playing = currentParameters.playing;
        const int& samplesPerBeat = currentParameters.samplesPerBeat;
        const int& positionSamples = currentParameters.positionSamples;
        const int& syncOffsetSamples = currentParameters.syncOffsetSamples;
        bool& syncBeat = currentParameters.syncBeat;
        ReferenceCountedBuffer::Ptr retainedCurrentBuffer(currentBuffer);

        auto clearBuffer = [&bufferToFill, &totalNumInputChannels, &totalNumOutputChannels] (int position, int numSamples) {
            for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
                bufferToFill.clear(i, position, numSamples);
            }
        };

        if (retainedCurrentBuffer == nullptr || playing == false) {
            clearBuffer(0, bufferToFill.getNumSamples());
        } else {
            clearBuffer(0, bufferToFill.getNumSamples());

            AudioSampleBuffer* currentAudioSampleBuffer(
                retainedCurrentBuffer->getAudioSampleBuffer());
            int position = retainedCurrentBuffer->position;

            // sync the beat with the host
            if (syncOffsetSamples > 0 && syncBeat) {
                syncBeat = false;
                position = positionSamples + samplesPerBeat - syncOffsetSamples;
            }
            int currentPositionSamples = positionSamples;

            if (position < currentPositionSamples) position = currentPositionSamples;

            const int bufferInputChannels = currentAudioSampleBuffer->getNumChannels();

            int outputSamplesRemaining = bufferToFill.getNumSamples();
            int outputSamplesOffset = 0;

            // loop one bar from selected position
            while (outputSamplesRemaining > 0) {
                int bufferSamplesRemainingFile =
                    currentAudioSampleBuffer->getNumSamples() - position;
                if (position > samplesPerBeat + currentPositionSamples) {
                    position = currentPositionSamples;
                }
                int bufferSamplesRemainingBeat = samplesPerBeat - position + currentPositionSamples;
                int bufferSamplesRemaining =
                    jmin(bufferSamplesRemainingBeat, bufferSamplesRemainingFile);

                // NOT WORKING
                if (bufferSamplesRemainingBeat > 0 && bufferSamplesRemainingFile <= 0) {
                    clearBuffer(position, outputSamplesRemaining);
                }
                // END OF
                int samplesThisTime = jmin(outputSamplesRemaining, bufferSamplesRemaining);

                for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
                    bufferToFill.copyFrom(channel, outputSamplesOffset, *currentAudioSampleBuffer,
                                          channel % bufferInputChannels, position, samplesThisTime);
                }

                outputSamplesRemaining -= samplesThisTime;
                outputSamplesOffset += samplesThisTime;
                position += samplesThisTime;

                if (position == currentAudioSampleBuffer->getNumSamples() ||
                    position >= currentPositionSamples + samplesPerBeat) {
                    position = positionSamples;
                }
                retainedCurrentBuffer->position = position;
            }
        }
    };
private:
    LooperEngineParameters currentParameters;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperEngine)
};
