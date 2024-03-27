//
// Created by Brian Mendoza on 26/03/24.
//

#include "Synth.h"

Synth::Synth()
{
    sampleRate = 44100.0f;
}

void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/)
{
    sampleRate = static_cast<float>(sampleRate_);
}

void Synth::deallocateResources()
{
}

void Synth::reset()
{
    voice.reset();
}

void Synth::render(float** outputBuffers, int sampleCount)
{
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0) {
        //note off
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
        //note on
        case 0x90: {
            uint8_t note = data1 & 0x7F;
            uint8_t velocity = data2 & 0x7F;
            if (velocity > 0) {
                noteOn(note, velocity);
            } else {
                noteOff(note);
            }
            break;
        }
    }
}

void Synth::noteOff(uint8_t note)
{
    if (voice.note == note) {
        voice.note = 0;
        voice.velocity = 0;
    }
}

void Synth::noteOn(uint8_t note, uint8_t velocity)
{
    voice.note = note;
    voice.velocity = velocity;
}

