import soundfile as sf
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

wav_file = "/Users/bytedance/Work/ten-vad/testset/testset-audio-07.wav"
data, samplerate = sf.read(wav_file)
plt.subplot(2, 1, 1)
plt.plot(np.arange(len(data)) / samplerate, data)
plt.grid(True)
plt.title("Waveform of WAV file")
plt.ylabel("Amplitude")

bin_file = "/Users/bytedance/Downloads/output.dat"
floats = np.fromfile(bin_file, dtype=np.float32)
prob = floats[::2]
flag = floats[1::2]

prob = np.repeat(prob, samplerate / 100)
flag = np.repeat(flag, samplerate / 100)

plt.plot(np.arange(len(prob)) / samplerate, prob, color="red", linestyle="--")
plt.plot(np.arange(len(flag)) / samplerate, flag, color="blue", linestyle=":")
plt.xlim([0, len(data) / samplerate])

frequencies, times, Sxx = signal.spectrogram(data, samplerate)

plt.subplot(2, 1, 2)
plt.pcolormesh(times, frequencies, 10 * np.log10(Sxx), shading="gouraud")
plt.ylabel("Frequency [Hz]")
plt.xlabel("Time [sec]")
plt.xlim([0, len(data) / samplerate])
plt.show()
