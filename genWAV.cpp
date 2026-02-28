#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>

constexpr double PI = 3.14159265358979323846;

// Write simple 16-bit PCM WAV file
void writeWav(const std::string& filename,
              const std::vector<int16_t>& samples,
              int sampleRate)
{
    std::ofstream file(filename, std::ios::binary);

    int32_t subchunk2Size = samples.size() * sizeof(int16_t);
    int32_t chunkSize = 36 + subchunk2Size;
    int16_t audioFormat = 1;        // PCM
    int16_t numChannels = 1;        // Mono
    int16_t bitsPerSample = 16;
    int32_t byteRate = sampleRate * numChannels * bitsPerSample / 8;
    int16_t blockAlign = numChannels * bitsPerSample / 8;

    // RIFF header
    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVE", 4);

    // fmt subchunk
    file.write("fmt ", 4);
    int32_t subchunk1Size = 16;
    file.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

    // data subchunk
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&subchunk2Size), 4);
    file.write(reinterpret_cast<const char*>(samples.data()), subchunk2Size);
}

int main()
{
    const int sampleRate = 44100;
    const double durationSeconds = 3.0;
    const double frequency = 440.0;

    int totalSamples = static_cast<int>(sampleRate * durationSeconds);

    std::vector<int16_t> samples;
    samples.reserve(totalSamples);

    for (int i = 0; i < totalSamples; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double value = std::sin(2.0 * PI * frequency * t);

        // Convert to 16-bit
        int16_t sample = static_cast<int16_t>(value * 32767);
        samples.push_back(sample);
    }

    writeWav("sound.wav", samples, sampleRate);

    std::cout << "Generated sound.wav\n";
}
