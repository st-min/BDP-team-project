
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include "file_source.hpp"
#include "file_sink.hpp"
#include "frame_router.hpp"
#include "dsp_ops.hpp"
#include "metrics.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::printf("Usage: %s <input.wav> <output.wav> [hostFrames=256] [internalBlock=256] [overlap=0.5]\n", argv[0]);
        return 1;
    }
    const std::string inPath = argv[1];
    const std::string outPath = argv[2];
    const int hostFrames = (argc >= 4) ? std::atoi(argv[3]) : 256;
    const int internalBlock = (argc >= 5) ? std::atoi(argv[4]) : 256;
    const float overlap = (argc >= 6) ? std::atof(argv[5]) : 0.5f;

    FileSource src;
    if (!src.open(inPath)) { std::fprintf(stderr, "Failed to open input\n"); return 2; }
    const int sampleRate = src.sampleRate();
    const int channels   = src.channels();

    FileSink sink;
    if (!sink.open(outPath, sampleRate, channels)) { std::fprintf(stderr, "Failed to open output\n"); return 3; }

    FrameRouter router(sampleRate, channels);
    router.setInternalBlockSize(internalBlock);
    router.setOverlap(overlap);

    DspOps dsp(channels);
    Metrics metrics(sampleRate, hostFrames);
    metrics.openCSV("metrics.csv");

    std::vector<float> inBuf(hostFrames * channels);
    std::vector<float> outBuf(hostFrames * channels);

    size_t totalFrames = src.frames();
    size_t processed = 0;

    auto wallStart = std::chrono::high_resolution_clock::now();
    while (processed < totalFrames) {
        size_t need = hostFrames;
        size_t got = src.readFrames(inBuf.data(), need);
        if (got < need) std::fill(inBuf.begin() + got*channels, inBuf.end(), 0.0f);

        auto t0 = std::chrono::high_resolution_clock::now();
        router.pushInput(inBuf.data(), hostFrames);
        router.processPendingBlocks(dsp);
        router.pullOutput(outBuf.data(), hostFrames);
        auto t1 = std::chrono::high_resolution_clock::now();

        sink.writeFrames(outBuf.data(), hostFrames);
        double cb_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        metrics.onCallbackEnd(cb_ms, false, router.currentBlockSize());
        processed += hostFrames;
    }
    auto wallEnd = std::chrono::high_resolution_clock::now();
    double wall_ms = std::chrono::duration<double, std::milli>(wallEnd - wallStart).count();
    std::printf("Done. Total frames: %zu, duration: %.2f sec, wall: %.2f ms\n",
        processed, processed / double(sampleRate), wall_ms);
    metrics.closeCSV();
    return 0;
}
