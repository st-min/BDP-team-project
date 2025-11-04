
# ABSC Offline Harness (Deterministic WAV → DSP → WAV)

C++17 + libsndfile. Offline "virtual callback" to test fixed vs adaptive internal block sizes.

## Build
mkdir -p build && cd build
cmake ..
cmake --build . -j

## Run
./absc_offline ../assets/input_48k.wav ../assets/out_fixed.wav 256 256 0.5
# args: <input.wav> <output.wav> [hostFrames] [internalBlock] [overlap]

## Plot
python3 ../scripts/experiments.py
