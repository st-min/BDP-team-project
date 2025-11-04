
#include "file_source.hpp"
#include <cstring>
bool FileSource::open(const std::string& path) {
    close(); std::memset(&info_, 0, sizeof(info_));
    f_ = sf_open(path.c_str(), SFM_READ, &info_);
    return f_ != nullptr;
}
void FileSource::close() { if (f_) { sf_close(f_); f_ = nullptr; } }
size_t FileSource::readFrames(float* interleaved, size_t frames) {
    if (!f_) return 0; sf_count_t r = sf_readf_float(f_, interleaved, (sf_count_t)frames); return (size_t)r;
}
