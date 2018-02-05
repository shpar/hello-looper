/*
  ==============================================================================

    RenderAudio.h
    Created: 15 Nov 2017 8:51:30pm
    Author:  Dan

  ==============================================================================
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
                       int samplesPerBeat, int& positionSamples, int syncOffsetSamples, bool syncBeat)
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
