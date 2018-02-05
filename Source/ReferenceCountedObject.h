/*
  ==============================================================================

    ReferenceCountedObject.h
    Created: 15 Nov 2017 7:41:24pm
    Author:  Dan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

class ReferenceCountedBuffer : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

    ReferenceCountedBuffer (int numChannels, int64 numSamples)  :   position (0),
                                                buffer (numChannels, numSamples)
    {

    }

    ~ReferenceCountedBuffer()
    {

    }

    AudioSampleBuffer* getAudioSampleBuffer()
    {
        return &buffer;
    }

    int position;

private:
    AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReferenceCountedBuffer)
};
