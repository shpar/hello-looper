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

#include "ReferenceCountedObject.h"

#pragma once

class RenderAudio : public ReferenceCountedObject
{
public:
    RenderAudio () {};
    ~RenderAudio () {};
    void processAudio (AudioSampleBuffer& bufferToFill, ReferenceCountedBuffer::Ptr& currentBuffer,
                       const int totalNumInputChannels, const int totalNumOutputChannels, bool playing,
                       int samplesPerBeat, std::atomic<int>& positionSamples, int syncOffsetSamples, bool syncBeat)
    {
        ReferenceCountedBuffer::Ptr retainedCurrentBuffer (currentBuffer);

        if (retainedCurrentBuffer == nullptr || playing == false)
        {
            for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            {
                bufferToFill.clear (i, 0, bufferToFill.getNumSamples());
            }
        }
        else
        {
            for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            {
                bufferToFill.clear (i, 0, bufferToFill.getNumSamples());
            }

            AudioSampleBuffer* currentAudioSampleBuffer (retainedCurrentBuffer->getAudioSampleBuffer());
            int position = retainedCurrentBuffer->position;

            // sync the beat with the host
            if (syncOffsetSamples > 0 && syncBeat)
            {
                syncBeat = false;
                position = positionSamples + samplesPerBeat - syncOffsetSamples;
            }
            int currentPositionSamples = positionSamples;

            if (position < currentPositionSamples)
                position = currentPositionSamples;

            const int bufferInputChannels = currentAudioSampleBuffer->getNumChannels();

            int outputSamplesRemaining = bufferToFill.getNumSamples();
            int outputSamplesOffset = 0;

            // loop one bar from selected position
            while (outputSamplesRemaining > 0)
            {
                int bufferSamplesRemainingFile = currentAudioSampleBuffer->getNumSamples()
                                               - position;
                if (position > samplesPerBeat + currentPositionSamples)
                {
                    position = currentPositionSamples;
                }
                int bufferSamplesRemainingBeat = samplesPerBeat - position + currentPositionSamples;
                int bufferSamplesRemaining = jmin (bufferSamplesRemainingBeat,
                                               bufferSamplesRemainingFile);

                // NOT WORKING
                if (bufferSamplesRemainingBeat > 0 && bufferSamplesRemainingFile <= 0) {
                    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
                        {
                            bufferToFill.clear (i, position, outputSamplesRemaining);
                        }
                }
                // END OF
                int samplesThisTime = jmin (outputSamplesRemaining, bufferSamplesRemaining);

                for (int channel = 0; channel < totalNumOutputChannels; ++channel)
                {
                    bufferToFill.copyFrom (channel, outputSamplesOffset,
                                               *currentAudioSampleBuffer,
                                               channel % bufferInputChannels,
                                               position,
                                               samplesThisTime);
                }

                outputSamplesRemaining -= samplesThisTime;
                outputSamplesOffset += samplesThisTime;
                position += samplesThisTime;

                if (position == currentAudioSampleBuffer->getNumSamples() ||
                    position >= currentPositionSamples + samplesPerBeat)
                {
                    position = positionSamples;
                }
                retainedCurrentBuffer->position = position;
            }
        }
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenderAudio)
};
