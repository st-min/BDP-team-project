
#pragma once
#include <fstream>
class Metrics {
public:
    Metrics(int sampleRate, int hostFrames) : sr_(sampleRate), hostFrames_(hostFrames) {}
    void openCSV(const std::string& path);
    void onCallbackEnd(double cb_ms, bool underrun, int blockSize);
    void closeCSV();
private:
    int sr_, hostFrames_; std::ofstream csv_; long long frameIndex_ = 0;
};
