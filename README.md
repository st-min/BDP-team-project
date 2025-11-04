# ABSC Offline Harness – README

Adaptive Block Size Control (ABSC) 를 구현하기 위한 오프라인 실험 환경입니다.
동일한 WAV 입력을 블록 기반 파이프라인으로 처리하고, 각 “가상 콜백”의 처리시간과 상태를 기록하여 고정형 vs 적응형(ABSC) 을 공정하게 비교할 수 있도록 설계했습니다.

⸻

##✅ 목표
	•	공정한 실험: 동일 입력(WAV) 기반, 동일 조건으로 반복 가능한 결과 확보
	•	구조적 베이스라인: 이후 ABSC 알고리즘과 JUCE 실시간 으로 이식 가능
	•	지표 기반 평가: 처리시간(cb_ms), underrun 등 핵심 지표를 자동 기록/시각화

⸻

##📦 저장소 구조

absc_offline_harness/
  CMakeLists.txt
  README.md
  assets/
    input_48k.wav            ← 테스트용 오디오 입력 파일(48kHz 권장)
  scripts/
    experiments.py           ← metrics.csv를 그래프로 (cb_ms.png)
  src/
    main.cpp                 ← 오프라인 가상 콜백 루프
    file_source.{hpp,cpp}    ← libsndfile로 WAV 입력
    file_sink.{hpp,cpp}      ← libsndfile로 WAV 출력
    frame_router.{hpp,cpp}   ← 내부 block 단위 분할/합치기 (ABSC가 제어)
    dsp_ops.{hpp,cpp}        ← 임시 DSP(1-pole low-pass)
    metrics.{hpp,cpp}        ← 콜백 처리시간/underrun/block_size 로그 → CSV


⸻

##🛠️ 빌드 환경 구축 & 빌드 시퀀스

0) 필수 요소
	•	C++17 컴파일러 (Clang, GCC, MSVC)
	•	CMake ≥ 3.15
	•	libsndfile (WAV I/O)
	•	pkg-config (권장, macOS/Linux)

1) macOS (Apple Silicon/Intel, Homebrew)

brew update
brew install cmake libsndfile pkg-config

빌드:

cd absc_offline_harness
mkdir -p build && cd build
cmake ..
cmake --build . -j

만약 링크 에러가 나면:
pkg-config --libs sndfile 로 -L/opt/homebrew/lib -lsndfile 가 출력되는지 확인.
필요 시:

export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"



2) Ubuntu/Debian

sudo apt update
sudo apt install -y build-essential cmake pkg-config libsndfile1-dev
cd absc_offline_harness
mkdir -p build && cd build
cmake ..
cmake --build . -j

3) Windows (MSVC + vcpkg 권장)

# vcpkg 설치 후
vcpkg install libsndfile
# CMake 호출 시 툴체인 지정
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg.cmake
cmake --build build --config Release


⸻

##▶️ 실행 & 플로팅
	1.	입력 WAV 준비 (권장: 48kHz 스테레오)
assets/input_48k.wav 로 두세요.
	2.	실행 (오프라인 가상 콜백 루프)

cd build
./absc_offline ../assets/input_48k.wav ../assets/out_fixed.wav 256 256 0.5
# 인자: <입력.wav> <출력.wav> [hostFrames=256] [internalBlock=256] [overlap=0.5]

	•	hostFrames: 장치 콜백 크기를 가상으로 고정 에뮬레이션
	•	internalBlock: 내부 처리 블록 크기 (나중에 ABSC가 동적으로 제어)
	•	결과물: metrics.csv, out_fixed.wav

	3.	그래프 생성

# 처음 한 번만
pip install pandas matplotlib   # 또는 conda install pandas matplotlib -y

python3 ../scripts/experiments.py
# → build/cb_ms.png 생성 (콜백 처리시간의 rolling mean)


⸻

##🧩 코드 구조 & 각 모듈 역할

main	src/main.cpp	가상 콜백 루프를 돌며 hostFrames 단위로 입력을 읽고, 내부 block 단위 처리 후 출력. 각 콜백의 처리시간을 측정하여 metrics에 기록.
FileSource	src/file_source.*	libsndfile로 WAV를 interleaved float로 읽어들임.
FileSink	src/file_sink.*	처리 결과를 WAV로 기록(청감 비교/검증 가능).
FrameRouter	src/frame_router.*	입력 스트림을 내부 blockSize(128/256/512…) 단위로 잘라 DspOps에 넘기고, 결과를 이어붙여 출력. ABSC가 제어할 대상이 바로 이 내부 블록 크기.
DspOps	src/dsp_ops.*	현재는 1차 Low-pass(placeholder) 로 단순 연산을 수행 (CPU 부하를 약간 만들어 baseline 확인). 나중에 실제 노이즈 감쇠 체인(HPF→Gate→EQ→Limiter/FFT)을 이 자리에 넣습니다.
Metrics	src/metrics.*	각 콜백의 처리시간(cb_ms), underrun 발생 여부, block_size 를 metrics.csv 로 기록.
experiments	scripts/experiments.py	metrics.csv를 읽어 통계 출력, cb_ms 롤링 평균 그래프(cb_ms.png) 생성.

현재 FrameRouter 는 오프라인용 간단 FIFO 이므로, 실시간 이식 시엔 RT-safe(락-프리 SPSC/AbstractFifo 래핑) 로 교체합니다.

⸻

###📊 우리가 평가하는 지표(왜 이걸 쓰는가?)

지표	의미	왜 중요한가?	ABSC에서의 역할
frame_index	“가상 콜백”의 순번(시간 인덱스)	시간에 따른 상태 변화를 분석하기 위한 기준축	block_size 전환 시점, 스파이크 위치 등 변곡점 식별
cb_ms	콜백 1회 처리시간(ms)	실시간 데드라인 여유도를 직접 반영	ABSC의 1차 피드백: 평균/추세가 오르면 block_size 확대 등 조치
underrun	버퍼 언더런(끊김) 발생 여부	청감 품질과 1:1 대응되는 실패 지표	fail-safe: 발생 전에 예방하도록 제어
block_size	내부 처리 블록 크기	지연 vs CPU 효율의 핵심 스위치	제어 변수: adaptive 전환의 기록 및 효과 검증

한 줄 요약:
cb_ms = 성능(속도), underrun = 안정성(신뢰성), block_size = 제어 레버, frame_index = 시간축 기준.

⸻

🔍 Baseline 결과 해석 (예시)
	•	입력 길이: 3,180,544 프레임 (≈ 72.12초 @ 48 kHz)
	•	설정: hostFrames=256, internalBlock=256, overlap=0.5
	•	평균 cb_ms: ≈ 0.014 ms (매우 낮고 안정적)
	•	표준편차: ≈ 0.0011 ms (작은 변동), 최대스파이크: ≈ 0.0566 ms (드묾)
	•	underrun: 0 (끊김 없음)

의미: 파이프라인은 안정적이며, 이후 ABSC 적용 시 부하 변화 상황에서 cb_ms 안정화·underrun 방지 효과를 검증할 참조선(reference) 으로 적합.

⸻

###🧭 앞으로의 방향성 (로드맵)
	1.	ABSC Controller 모듈 추가
	•	50–100ms 주기로 cb_ms 이동평균과 underrun을 읽어 block_size(128/256/512) 를 히스테리시스+쿨다운으로 전환
	•	파일: absc_controller.{hpp,cpp} (입력: 지표 / 출력: 다음 block_size)
	2.	Adaptive 모드 실험
	•	고정형 vs ABSC 비교: 평균/중앙/95·99p cb_ms, underrun 비율, 전환 빈도/핀퐁 여부
	•	그래프: cb_ms 타임라인 + block_size 타임라인을 겹쳐 adaptive 반응 시각화

  ⸻
