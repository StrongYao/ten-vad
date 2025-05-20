
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>

#include "wave_io/audio_wave.h"
#include "../include/ten_vad.h"

static SWavFile* wave_file_in = nullptr;
static FILE* dat_file_out = nullptr;
static int16_t* file_buf;
static uint32_t sr;
static uint32_t samples_per_channel;
static uint16_t channels;
static size_t bytes_per_sample;

void* ten_vad_handle;

static void InitPcmIO(char* inFileName, char* outFileName) {
  wave_file_in = wav_open(inFileName, "rb");
  sr = wav_get_sample_rate(wave_file_in);
  samples_per_channel = sr / 100;
  channels = wav_get_num_channels(wave_file_in);
  bytes_per_sample = wav_get_sample_size(wave_file_in) / 8;

  dat_file_out = fopen(outFileName, "wb");

  file_buf = (int16_t*)malloc(samples_per_channel * sizeof(int16_t) * channels);

  if (!wave_file_in || !dat_file_out || !file_buf) {
    exit(-1);
  }
}

static void ClosePcmIO() {
  if (wave_file_in) {
    wav_close(wave_file_in);
    wave_file_in = nullptr;
  }

  if (dat_file_out) {
    fclose(dat_file_out);
    dat_file_out = nullptr;
  }
}

int main(int argc, char* argv[]) {
#if defined(__APPLE__)
  char inFileName[512] = "/Users/bytedance/Work/ten-vad/testset/testset-audio-07.wav";
  char outFileName[512] = "/Users/bytedance/Downloads/output.dat";
#elif defined(__ANDROID__)
  char inFileName[512] = "./input_stereo.wav";
  char outFileName[512] = "./output.wav";
#elif defined(_WIN32)
  char inFileName[512] = "C:/Work/TrueVolume/input_mono.wav";
  char outFileName[512] = "C:/Work/TrueVolume/output.wav";
#else
  char inFileName[512];
  char outFileName[512];
#endif
  if (argc >= 2) {
    strcpy(inFileName, argv[1]);
  }
  if (argc >= 3) {
    strcpy(outFileName, argv[2]);
  }

  InitPcmIO(inFileName, outFileName);  // Preparing file I/O

  ten_vad_create(&ten_vad_handle, samples_per_channel, 0.5);

  std::chrono::microseconds total_exe_time_4_first_resampler(0);

  uint64_t running_counts = 0;
  float period_duration = samples_per_channel * 1000.0f / sr;
  float probability;
  int flag;

  while (1) {
    size_t byte_size = samples_per_channel * channels * bytes_per_sample;
    auto byte_size_read = wav_read_interleave(wave_file_in, file_buf, byte_size);

    auto start = std::chrono::high_resolution_clock::now();
    if (byte_size_read == byte_size) {
      ten_vad_process(ten_vad_handle, file_buf, samples_per_channel, &probability, &flag);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    total_exe_time_4_first_resampler += duration;

    if (byte_size_read < byte_size) {
      break;
    }

    ++running_counts;

    fwrite(&probability, sizeof(float), 1, dat_file_out);
    float f_flag = flag;
    fwrite(&f_flag, sizeof(float), 1, dat_file_out);
  }

  std::cout << "Total execution time of Echo Finder is ("
            << total_exe_time_4_first_resampler.count() * 0.001 << ") ms" << std::endl
            << "RTF factor is "
            << total_exe_time_4_first_resampler.count() * 0.001 /
                   (running_counts * period_duration)
            << std::endl;

  ClosePcmIO();

  ten_vad_destroy(&ten_vad_handle);
  ten_vad_handle = nullptr;

  return 0;
}
