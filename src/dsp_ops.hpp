
#pragma once
#include <vector>
class DspOps {
public:
    explicit DspOps(int channels);
    void processBlock(const float* inInterleaved, float* outInterleaved, int frames);
private:
    int ch_; std::vector<float> z_; float alpha_ = 0.1f; // simple one-pole lowpass
};
