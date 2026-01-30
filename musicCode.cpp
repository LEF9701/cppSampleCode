/**
 * C++ 音樂代碼播放器
 * 用戶輸入音樂代碼，程式播放對應的音符
 * 
 * 音樂代碼格式：
 *   音符: C D E F G A B (可加數字表示八度，如 C4, D5)
 *   升降: # 升半音, b 降半音 (如 C#4, Bb3)
 *   休止符: R 或 -
 *   節拍: 數字後綴表示時值 (1=全音符, 2=二分, 4=四分, 8=八分, 16=十六分)
 *   
 * 範例輸入: "C4 D4 E4 F4 G4 A4 B4 C5"  (C大調音階)
 *          "E4 E4 F4 G4 G4 F4 E4 D4"  (歡樂頌開頭)
 * 
 * macOS 版本：使用 afplay 播放生成的音頻文件
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>

// 音符頻率對照表 (以 A4 = 440Hz 為標準)
class MusicPlayer {
private:
    std::map<std::string, double> noteFrequencies;
    int sampleRate = 44100;
    int defaultOctave = 4;
    int defaultDuration = 4;  // 四分音符
    int tempo = 120;          // BPM
    
    // 初始化音符頻率
    void initFrequencies() {
        // 基準頻率 (A4 = 440Hz)
        std::vector<std::string> notes = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        
        for (int octave = 0; octave <= 8; octave++) {
            for (int i = 0; i < 12; i++) {
                // A4 = 440Hz, 計算其他音符頻率
                // 半音比例 = 2^(1/12)
                int semitones = (octave - 4) * 12 + (i - 9);  // 相對於 A4 的半音數
                double freq = 440.0 * std::pow(2.0, semitones / 12.0);
                
                std::string noteName = notes[i] + std::to_string(octave);
                noteFrequencies[noteName] = freq;
                
                // 添加降調別名
                if (notes[i].length() == 2) {  // 升調音符
                    std::string flatNote;
                    if (notes[i] == "C#") flatNote = "Db";
                    else if (notes[i] == "D#") flatNote = "Eb";
                    else if (notes[i] == "F#") flatNote = "Gb";
                    else if (notes[i] == "G#") flatNote = "Ab";
                    else if (notes[i] == "A#") flatNote = "Bb";
                    noteFrequencies[flatNote + std::to_string(octave)] = freq;
                }
            }
        }
    }
    
    // 解析單個音符代碼
    struct Note {
        double frequency;
        double duration;  // 秒
        bool isRest;
    };
    
    Note parseNote(const std::string& code) {
        Note note = {0, 0.5, false};
        
        if (code.empty()) {
            note.isRest = true;
            return note;
        }
        
        // 休止符
        if (code[0] == 'R' || code[0] == 'r' || code[0] == '-') {
            note.isRest = true;
            // 解析時值
            if (code.length() > 1) {
                int duration = std::stoi(code.substr(1));
                note.duration = (60.0 / tempo) * (4.0 / duration);
            } else {
                note.duration = (60.0 / tempo);
            }
            return note;
        }
        
        // 解析音符
        std::string noteName = "";
        int octave = defaultOctave;
        int duration = defaultDuration;
        
        size_t i = 0;
        
        // 音符名稱 (C-G, A-B)
        if (i < code.length() && ((code[i] >= 'A' && code[i] <= 'G') || (code[i] >= 'a' && code[i] <= 'g'))) {
            noteName += std::toupper(code[i]);
            i++;
        }
        
        // 升降號
        if (i < code.length() && (code[i] == '#' || code[i] == 'b')) {
            noteName += code[i];
            i++;
        }
        
        // 八度
        if (i < code.length() && code[i] >= '0' && code[i] <= '8') {
            octave = code[i] - '0';
            i++;
        }
        
        // 時值 (用 _ 分隔)
        if (i < code.length() && code[i] == '_') {
            i++;
            if (i < code.length()) {
                duration = std::stoi(code.substr(i));
            }
        }
        
        // 組合完整音符名
        std::string fullNote = noteName + std::to_string(octave);
        
        if (noteFrequencies.find(fullNote) != noteFrequencies.end()) {
            note.frequency = noteFrequencies[fullNote];
        } else {
            std::cerr << "未知音符: " << fullNote << std::endl;
            note.isRest = true;
        }
        
        // 計算時長 (基於 BPM)
        note.duration = (60.0 / tempo) * (4.0 / duration);
        
        return note;
    }
    
    // 生成正弦波音頻數據
    std::vector<int16_t> generateTone(double frequency, double duration) {
        int numSamples = static_cast<int>(sampleRate * duration);
        std::vector<int16_t> samples(numSamples);
        
        double fadeTime = 0.02;  // 20ms 淡入淡出
        int fadeSamples = static_cast<int>(sampleRate * fadeTime);
        
        for (int i = 0; i < numSamples; i++) {
            double t = static_cast<double>(i) / sampleRate;
            
            // 基礎正弦波
            double sample = std::sin(2.0 * M_PI * frequency * t);
            
            // 添加一些泛音使聲音更豐富
            sample += 0.3 * std::sin(4.0 * M_PI * frequency * t);
            sample += 0.1 * std::sin(6.0 * M_PI * frequency * t);
            
            // 正規化
            sample /= 1.4;
            
            // 淡入淡出
            double envelope = 1.0;
            if (i < fadeSamples) {
                envelope = static_cast<double>(i) / fadeSamples;
            } else if (i > numSamples - fadeSamples) {
                envelope = static_cast<double>(numSamples - i) / fadeSamples;
            }
            
            // ADSR 包絡
            double attackTime = 0.05;
            double decayTime = 0.1;
            double sustainLevel = 0.7;
            double releaseTime = 0.1;
            
            int attackSamples = static_cast<int>(sampleRate * attackTime);
            int decaySamples = static_cast<int>(sampleRate * decayTime);
            int releaseSamples = static_cast<int>(sampleRate * releaseTime);
            int sustainSamples = numSamples - attackSamples - decaySamples - releaseSamples;
            
            if (i < attackSamples) {
                envelope = static_cast<double>(i) / attackSamples;
            } else if (i < attackSamples + decaySamples) {
                double decayProgress = static_cast<double>(i - attackSamples) / decaySamples;
                envelope = 1.0 - (1.0 - sustainLevel) * decayProgress;
            } else if (i < numSamples - releaseSamples) {
                envelope = sustainLevel;
            } else {
                double releaseProgress = static_cast<double>(i - (numSamples - releaseSamples)) / releaseSamples;
                envelope = sustainLevel * (1.0 - releaseProgress);
            }
            
            samples[i] = static_cast<int16_t>(sample * envelope * 32767 * 0.5);
        }
        
        return samples;
    }
    
    // 生成靜音
    std::vector<int16_t> generateSilence(double duration) {
        int numSamples = static_cast<int>(sampleRate * duration);
        return std::vector<int16_t>(numSamples, 0);
    }
    
    // 寫入 WAV 文件
    void writeWav(const std::string& filename, const std::vector<int16_t>& samples) {
        std::ofstream file(filename, std::ios::binary);
        
        // WAV 文件頭
        int dataSize = samples.size() * 2;
        int fileSize = dataSize + 36;
        
        // RIFF header
        file.write("RIFF", 4);
        file.write(reinterpret_cast<char*>(&fileSize), 4);
        file.write("WAVE", 4);
        
        // fmt chunk
        file.write("fmt ", 4);
        int fmtSize = 16;
        file.write(reinterpret_cast<char*>(&fmtSize), 4);
        int16_t audioFormat = 1;  // PCM
        file.write(reinterpret_cast<char*>(&audioFormat), 2);
        int16_t numChannels = 1;  // Mono
        file.write(reinterpret_cast<char*>(&numChannels), 2);
        file.write(reinterpret_cast<char*>(&sampleRate), 4);
        int byteRate = sampleRate * 2;
        file.write(reinterpret_cast<char*>(&byteRate), 4);
        int16_t blockAlign = 2;
        file.write(reinterpret_cast<char*>(&blockAlign), 2);
        int16_t bitsPerSample = 16;
        file.write(reinterpret_cast<char*>(&bitsPerSample), 2);
        
        // data chunk
        file.write("data", 4);
        file.write(reinterpret_cast<char*>(&dataSize), 4);
        file.write(reinterpret_cast<const char*>(samples.data()), dataSize);
        
        file.close();
    }
    
public:
    MusicPlayer() {
        initFrequencies();
    }
    
    void setTempo(int bpm) {
        tempo = bpm;
    }
    
    // 解析並播放音樂代碼
    void play(const std::string& musicCode) {
        std::vector<int16_t> allSamples;
        
        std::istringstream iss(musicCode);
        std::string noteCode;
        
        std::cout << "\n🎵 解析音樂代碼...\n" << std::endl;
        
        while (iss >> noteCode) {
            Note note = parseNote(noteCode);
            
            if (note.isRest) {
                std::cout << "  休止符 (" << note.duration << "s)" << std::endl;
                auto silence = generateSilence(note.duration);
                allSamples.insert(allSamples.end(), silence.begin(), silence.end());
            } else {
                std::cout << "  " << noteCode << " -> " << note.frequency << " Hz (" << note.duration << "s)" << std::endl;
                auto tone = generateTone(note.frequency, note.duration);
                allSamples.insert(allSamples.end(), tone.begin(), tone.end());
            }
        }
        
        // 寫入臨時 WAV 文件
        std::string filename = "/tmp/music_output.wav";
        writeWav(filename, allSamples);
        
        std::cout << "\n🎵 正在播放... (按 Ctrl+C 停止)\n" << std::endl;
        
        // 使用系統命令播放
        #ifdef __APPLE__
            std::string cmd = "afplay " + filename;
        #elif __linux__
            std::string cmd = "aplay " + filename + " 2>/dev/null || paplay " + filename + " 2>/dev/null || play " + filename + " 2>/dev/null";
        #else
            std::string cmd = "start " + filename;  // Windows
        #endif
        
        system(cmd.c_str());
        
        std::cout << "✅ 播放完成！" << std::endl;
    }
    
    // 顯示幫助信息
    void showHelp() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════╗
║              🎹 C++ 音樂代碼播放器 🎹                        ║
╠═══════════════════════════════════════════════════════════╣
║  音符格式: [音名][升降號][八度][_時值]                 　　  　║
║                                                           ║
║  音名:     C D E F G A B                                   ║
║  升降號:   # (升) 或 b (降)                                 ║
║  八度:     0-8 (預設為 4, A4=440Hz)                         ║
║  時值:     1=全音符, 2=二分, 4=四分, 8=八分, 16=十六分         ║
║  休止符:   R 或 -                                          ║
║                                                           ║
╠═══════════════════════════════════════════════════════════╣
║  範例:                                                     ║
║    C4 D4 E4 F4 G4 A4 B4 C5     (C大調音階)                  ║
║    E4 E4 F4 G4 G4 F4 E4 D4     (歡樂頌)                    ║
║    C4_8 C4_8 G4_8 G4_8 A4_8 A4_8 G4_4  (小星星)            ║
║    C4 E4 G4 C5                 (C和弦琶音)                  ║
║                                                           ║
║  命令:                                                     ║
║    tempo <數字>   設定速度 (BPM)                            ║
║    help           顯示此說明                                ║
║    quit / exit    退出程式                                 ║
╚═══════════════════════════════════════════════════════════╝
)" << std::endl;
    }
    
    // 播放預設曲目
    void playDemo(const std::string& name) {
        std::map<std::string, std::string> demos = {
            {"scale", "C4 D4 E4 F4 G4 A4 B4 C5"},
            {"ode", "E4 E4 F4 G4 G4 F4 E4 D4 C4 C4 D4 E4 E4_2 D4_8 D4_2"},
            {"twinkle", "C4_4 C4_4 G4_4 G4_4 A4_4 A4_4 G4_2 F4_4 F4_4 E4_4 E4_4 D4_4 D4_4 C4_2"},
            {"mary", "E4 D4 C4 D4 E4 E4 E4_2 D4 D4 D4_2 E4 G4 G4_2"},
            {"canon", "D4 F#4 A4 D5 C#5 A4 B4 F#4 G4 D4 G4 F#4 E4 G4 F#4 E4"},
            {"happy", "C4_8 C4_8 D4_4 C4_4 F4_4 E4_2 C4_8 C4_8 D4_4 C4_4 G4_4 F4_2"},
        };
        
        if (demos.find(name) != demos.end()) {
            std::cout << "🎵 播放: " << name << std::endl;
            play(demos[name]);
        } else {
            std::cout << "❌ 未知的預設曲目。可用的曲目有:" << std::endl;
            std::cout << "   scale  - C大調音階" << std::endl;
            std::cout << "   ode    - 歡樂頌" << std::endl;
            std::cout << "   twinkle - 小星星" << std::endl;
            std::cout << "   mary   - Mary Had a Little Lamb" << std::endl;
            std::cout << "   canon  - 卡農片段" << std::endl;
            std::cout << "   happy  - 生日快樂" << std::endl;
        }
    }
};

int main() {
    MusicPlayer player;
    
    std::cout << "\n🎹 歡迎使用 C++ 音樂代碼播放器！" << std::endl;
    player.showHelp();
    
    std::string input;
    
    while (true) {
        std::cout << "\n🎵 請輸入音樂代碼 (或輸入 'help' 查看說明): ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        // 處理命令
        if (input == "quit" || input == "exit" || input == "q") {
            std::cout << "👋 再見！" << std::endl;
            break;
        }
        
        if (input == "help" || input == "h") {
            player.showHelp();
            continue;
        }
        
        if (input.length() >= 6 && input.substr(0, 5) == "tempo") {
            try {
                int bpm = std::stoi(input.substr(6));
                player.setTempo(bpm);
                std::cout << "⏱️  速度設定為 " << bpm << " BPM" << std::endl;
            } catch (...) {
                std::cout << "❌ 請輸入有效的 BPM 數字，例如: tempo 120" << std::endl;
            }
            continue;
        }
        
        if (input.length() >= 4 && input.substr(0, 4) == "demo") {
            std::string demoName = "scale";
            if (input.length() > 5) {
                demoName = input.substr(5);
            }
            player.playDemo(demoName);
            continue;
        }
        
        // 播放用戶輸入的音樂代碼
        player.play(input);
    }
    
    return 0;
}
