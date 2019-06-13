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

#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

class ReferenceCountedBuffer : public ReferenceCountedObject {
public:
    typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

    ReferenceCountedBuffer(int numChannels, int64 numSamples)
        : position(0), buffer(numChannels, numSamples) {}

    ~ReferenceCountedBuffer() {}

    AudioSampleBuffer* getAudioSampleBuffer() { return &buffer; }

    int position;

private:
    AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReferenceCountedBuffer)
};
