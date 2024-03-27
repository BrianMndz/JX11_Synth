//
// Created by Brian Mendoza on 26/03/24.
//

#ifndef JX11SYNTH_VOICE_H
#define JX11SYNTH_VOICE_H

struct Voice
{
    int note;
    int velocity;

    void reset()
    {
        note = 0;
        velocity = 0;
    }
};

#endif //JX11SYNTH_VOICE_H
