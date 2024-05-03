//
// Created by Brian Mendoza on 03/04/24.
//

#ifndef JX11SYNTH_NOISEGENERATOR_H
#define JX11SYNTH_NOISEGENERATOR_H

class NoiseGenerator
{
public:
    void reset() {
        seed = 0x12345678;
    }

    float nextValue() {
        seed = (seed * 196314165) + 907633515;
        int temp = static_cast<int>(seed >> 7) - 16777216;
        return static_cast<float>(temp) / 16777216.0f;;
    }

private:
    unsigned int seed;
};

#endif //JX11SYNTH_NOISEGENERATOR_H
