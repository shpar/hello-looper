/*
  ==============================================================================

    Hotkey.h
    Created: 15 Nov 2017 7:30:20pm
    Author:  Dan

  ==============================================================================
*/

#pragma once

class Hotkey : public TextButton
{
public:
    Hotkey(int hotkeyId): hotkey(std::to_string(hotkeyId)), position(0), positionSaved(false) {}
    ~Hotkey() {}

    void setPosition(double position)
    {
        this->position = position;
    }

    double getPosition()
    {
        return position;
    }

    void setActive()
    {
        positionSaved = true;
    }

    void setInactive()
    {
        positionSaved = false;
    }

    bool isActive()
    {
        return positionSaved;
    }

private:
    TextButton hotkey;
    int hotkeyId;
    double position;
    bool positionSaved;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Hotkey)
};
