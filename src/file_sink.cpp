
#include "file_sink.hpp"
#include <cstring>
bool FileSink::open(const std::string& path, int sampleRate, int channels) {
    close(); std::memset(&info_, 0, sizeof(info_));
    info_.samplerate = sampleRate; info_.channels = channels;
    info_.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    f_ = sf_open(path.c_str(), SFM_WRITE, &info_);
    return f_ != nullptr;
}
void FileSink::close() { if (f_) { sf_close(f_); f_ = nullptr; } }
void FileSink::writeFrames(const float* interleaved, size_t frames) {
    if (!f_) return; sf_writef_float(f_, interleaved, (sf_count_t)frames);
}
