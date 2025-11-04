
#pragma once
#include <string>
#include <sndfile.h>

class FileSource {
public:
    ~FileSource() { close(); }
    bool open(const std::string& path);
    void close();
    int  sampleRate() const { return info_.samplerate; }
    int  channels()   const { return info_.channels; }
    long long frames() const { return info_.frames; }
    size_t readFrames(float* interleaved, size_t frames);
private:
    SNDFILE* f_ = nullptr;
    SF_INFO info_{};
};
