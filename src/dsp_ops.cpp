
#include "dsp_ops.hpp"
#include <cstring>
DspOps::DspOps(int channels) : ch_(channels), z_(channels, 0.0f) {}
void DspOps::processBlock(const float* in, float* out, int frames) {
    for (int n=0;n<frames;++n) {
        for (int c=0;c<ch_;++c) {
            float x = in[n*ch_ + c];
            z_[c] = z_[c] + alpha_ * (x - z_[c]);
            out[n*ch_ + c] = z_[c];
        }
    }
}
