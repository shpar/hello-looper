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
#include "keyfinder.h"
#include <array>

class Analyzer : public ReferenceCountedObject
{
public:
    Analyzer () {};
    ~Analyzer () {};
    void analyze(int sample_rate, int channels, int duration, ReferenceCountedBuffer::Ptr newBuffer, int current_position) {
        audio_data.setFrameRate(sample_rate);
        audio_data.setChannels(channels);
        audio_data.addToSampleCount(duration);
        DBG("position " << current_position);

        const auto& audio_sample_buffer = newBuffer->getAudioSampleBuffer();
        for (int j = 0; j < channels; ++j) {
            for (int i = 0; i < duration; ++i)
                audio_data.setSample(i, audio_sample_buffer->getSample(j, i + current_position));
        }
    }

    KeyFinder::AudioData audio_data;
    std::map<int, int> sample_analysis;
};

const std::array<std::string, 25> key_name = {
    "A",
    "Am",
    "Bb",
    "Bbm",
    "B",
    "Bm",
    "C",
    "Cm",
    "Db",
    "Dbm",
    "D",
    "Dm",
    "Eb",
    "Ebm",
    "E",
    "Em",
    "F",
    "Fm",
    "Gb",
    "Gbm",
    "G",
    "Gm",
    "Ab",
    "Abm",
    "/"
};
