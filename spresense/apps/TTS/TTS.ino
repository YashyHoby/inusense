#include <fcntl.h>    // ファイル操作用
#include <unistd.h>   // ファイル操作用
#include <sys/stat.h> // ファイルステータス用
#include <Audio.h>    // Spresenseオーディオ用
#include <speak_lib.h> // eSpeak用

AudioClass *theAudio = AudioClass::getInstance();

void setup() {
    // シリアルモニタの初期化
    Serial.begin(115200);

    // Spresenseオーディオ初期化
    theAudio->begin();
    theAudio->setVolume(0.5);  // 音量を50%に設定

    // eSpeak初期化
    espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 500, NULL, 0);
    espeak_SetVoiceByName("jp");  // 日本語音声の設定
}

void loop() {
    const char* text = "こんにちは、Spresenseでの音声合成です。";
    
    // 音声合成の実行とエラーチェック
    int synthResult = espeak_Synth(text, strlen(text), 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL);
    if (synthResult != EE_OK) {
        Serial.println("Failed to synthesize speech");
        return;
    }
    
    delay(5000);  // 音声再生完了を待機
}

void* espeak_mem_map(const char* path, int* size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        Serial.println("Failed to open file");
        return NULL;
    }

    // ファイルサイズの取得
    struct stat st;
    if (fstat(fd, &st) < 0) {
        Serial.println("Failed to get file size");
        close(fd);
        return NULL;
    }
    *size = st.st_size;

    // メモリ確保と読み込み
    void* buffer = malloc(*size);
    if (buffer == NULL) {
        Serial.println("Failed to allocate memory");
        close(fd);
        return NULL;
    }

    read(fd, buffer, *size);
    close(fd);
    return buffer;
}
