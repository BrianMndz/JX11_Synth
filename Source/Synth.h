//
// Created by Brian Mendoza on 26/03/24.
//

#ifndef JX11SYNTH_SYNTH_H
#define JX11SYNTH_SYNTH_H

#include <JuceHeader.h>
#include "Voice.h"

class Synth
{
public:
    Synth();

    void allocateResources(double sampleRate, int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render(float** outputBuffers, int sampleCount);
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);

private:
    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note);

    float sampleRate;
    Voice voice;
};

#endif //JX11SYNTH_SYNTH_H
