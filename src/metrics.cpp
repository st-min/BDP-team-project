
#include "metrics.hpp"
#include <iomanip>
void Metrics::openCSV(const std::string& path) {
    csv_.open(path, std::ios::out | std::ios::trunc);
    csv_ << "frame_index,cb_ms,underrun,block_size\n";
}
void Metrics::onCallbackEnd(double cb_ms, bool underrun, int blockSize) {
    csv_ << frameIndex_ << "," << std::fixed << std::setprecision(4) << cb_ms
         << "," << (underrun ? 1 : 0) << "," << blockSize << "\n";
    frameIndex_ += hostFrames_;
}
void Metrics::closeCSV() { if (csv_.is_open()) csv_.close(); }
