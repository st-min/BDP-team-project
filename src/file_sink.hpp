
#pragma once
#include <string>
#include <sndfile.h>
class FileSink {
public:
    ~FileSink() { close(); }
    bool open(const std::string& path, int sampleRate, int channels);
    void close();
    void writeFrames(const float* interleaved, size_t frames);
private:
    SNDFILE* f_ = nullptr;
    SF_INFO info_{};
};
